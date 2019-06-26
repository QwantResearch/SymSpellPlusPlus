#include "symspell.h"

namespace symspell {

    SymSpell::SymSpell(int initialCapacity, int maxDictionaryEditDistance, int prefixLength, int countThreshold, int compactLevel)
    {
        if (initialCapacity < 0) throw std::invalid_argument("initialCapacity");
        if (maxDictionaryEditDistance < 0) throw std::invalid_argument("maxDictionaryEditDistance");
        if (prefixLength < 1 || prefixLength <= maxDictionaryEditDistance) throw std::invalid_argument("prefixLength");
        if (countThreshold < 0) throw std::invalid_argument("countThreshold");
        if (compactLevel > 16) throw std::invalid_argument("compactLevel");

        this->words.reserve(initialCapacity);
        this->deletes.reserve(initialCapacity);
        this->initialCapacity = initialCapacity;
        this->distanceComparer = new EditDistance(this->distanceAlgorithm);

        this->maxDictionaryEditDistance = maxDictionaryEditDistance;
        this->prefixLength = prefixLength;
        this->countThreshold = countThreshold;
        if (compactLevel > 16) compactLevel = 16;
        this->compactMask = ((std::numeric_limits<int>::max)() >> (3 + compactLevel)) << 2;
        this->deletesEnd = this->deletes.end();
        this->wordsEnd = this->words.end();
        this->belowThresholdWordsEnd = this->belowThresholdWords.end();
        this->candidates.reserve(32);
        //this->maxDictionaryWordLength = 0;
    }

    SymSpell::~SymSpell()
    {
        vector<string>::iterator vecEnd;
        auto deletesEnd = this->deletes.end();
        for (auto it = this->deletes.begin(); it != deletesEnd; ++it)
        {
            vecEnd = it->second.end();
            for (auto vecIt = it->second.begin(); vecIt != vecEnd; ++vecIt)
            {
                delete[] * vecIt;
            }
        }

        delete this->distanceComparer;
    }

    bool SymSpell::CreateDictionaryEntry(const char * key, long count, SuggestionStage * staging)
    {
        int keyLen = strlen(key);
        if (count <= 0)
        {
            if (this->countThreshold > 0) return false; // no point doing anything if count is zero, as it can't change anything
            count = 0;
        }

        long countPrevious = -1;
        auto belowThresholdWordsFinded = belowThresholdWords.find(key);
        auto wordsFinded = words.find(key);

        // look first in below threshold words, update count, and allow promotion to correct spelling word if count reaches threshold
        // threshold must be >1 for there to be the possibility of low threshold words
        if (countThreshold > 1 && belowThresholdWordsFinded != belowThresholdWordsEnd)
        {
            countPrevious = belowThresholdWordsFinded->second;
            // calculate new count for below threshold word
            count = ((std::numeric_limits<long>::max)() - countPrevious > count) ? countPrevious + count : (std::numeric_limits<long>::max)();
            // has reached threshold - remove from below threshold collection (it will be added to correct words below)
            if (count >= countThreshold)
            {
                belowThresholdWords.erase(key);
                belowThresholdWordsEnd = belowThresholdWords.end();
            }
            else
            {
                belowThresholdWords[key] = count;
                belowThresholdWordsEnd = belowThresholdWords.end();
                return false;
            }
        }
        else if (wordsFinded != wordsEnd)
        {
            countPrevious = wordsFinded->second;
            count = ((std::numeric_limits<long>::max)() - countPrevious > count) ? countPrevious + count : (std::numeric_limits<long>::max)();
            words[key] = count;
            return false;
        }
        else if (count < CountThreshold())
        {
            belowThresholdWords[key] = count;
            belowThresholdWordsEnd = belowThresholdWords.end();
            return false;
        }

        words[key] = count;
      
        wordsEnd = words.end();

        if (keyLen > maxDictionaryWordLength)
            maxDictionaryWordLength = keyLen;

        EditsPrefix(key, edits);

        if (staging != nullptr)
        {
            auto editsEnd = edits.end();
            for (auto it = edits.begin(); it != editsEnd; ++it)
            {
                staging->Add(*it, _strdup(key));
            }
        }
        else
        {
            auto editsEnd = edits.end();
            for (auto it = edits.begin(); it != editsEnd; ++it)
            {
                size_t deleteHash = *it;
                auto deletesFinded = deletes.find(deleteHash);
                if (deletesFinded != deletesEnd)
                {
                    char* tmp = new char[keyLen + 1];
                    std::memcpy(tmp, key, keyLen);
                    tmp[keyLen] = '\0';

                    //delete[] deletes[deleteHash][deletesFinded->second.size() - 1];
                    deletes[deleteHash].push_back(tmp);
                    deletesEnd = deletes.end();
                }
                else
                {
                    char* tmp = new char[keyLen + 1];
                    std::memcpy(tmp, key, keyLen);
                    tmp[keyLen] = '\0';

                    deletes[deleteHash] = vector<string>();
                    //deletes[deleteHash].resize(1);
                    deletes[deleteHash].push_back(tmp);
                    deletesEnd = deletes.end();
                }
            }
        }

        edits.clear();
        return true;
    }

    void SymSpell::EditsPrefix(string key, unordered_set<size_t>& hashSet)
    {
        size_t len = strlen(key);
        char* tmp = nullptr;
        /*if (len <= maxDictionaryEditDistance) //todo fix
            hashSet.insert("");*/

        if (len > prefixLength)
        {
            tmp = new char[prefixLength + 1];
            std::memcpy(tmp, key, prefixLength);
            tmp[prefixLength] = '\0';
        }
        else
        {
            tmp = new char[len + 1];
            std::memcpy(tmp, key, len);
            tmp[len] = '\0';
        }

        hashSet.insert(stringHash(tmp));
        Edits(tmp, 0, hashSet);
    }

    void SymSpell::Edits(const char * word, int editDistance, unordered_set<size_t> & deleteWords)
    {
        auto deleteWordsEnd = deleteWords.end();
        ++editDistance;
        size_t wordLen = strlen(word);
        if (wordLen > 1)
        {
            for (size_t i = 0; i < wordLen; ++i)
            {
                char* tmp = new char[wordLen];
                std::memcpy(tmp, word, i);
                std::memcpy(tmp + i, word + i + 1, wordLen - 1 - i);
                tmp[wordLen - 1] = '\0';

                if (deleteWords.insert(stringHash(tmp)).second)
                {
                    //recursion, if maximum edit distance not yet reached
                    if (editDistance < maxDictionaryEditDistance && (wordLen - 1) > 1)
                        Edits(tmp, editDistance, deleteWords);
                }
                else {
                    delete[] tmp;
                }
            }
        }
    }

    void SymSpell::PurgeBelowThresholdWords()
    {
        belowThresholdWords.clear();
        belowThresholdWordsEnd = belowThresholdWords.end();
    }

    void SymSpell::CommitStaged(SuggestionStage staging)
    {
        staging.CommitTo(deletes);
    }

    void SymSpell::Lookup(const char * input, Verbosity verbosity, vector<std::unique_ptr<symspell::SuggestItem>> & items)
    {
        this->Lookup(input, verbosity, this->maxDictionaryEditDistance, false, items);
    }

    void SymSpell::Lookup(const char * input, Verbosity verbosity, int maxEditDistance, vector<std::unique_ptr<symspell::SuggestItem>> & items)
    {
        this->Lookup(input, verbosity, maxEditDistance, false, items);
    }

    void SymSpell::Lookup(const char * input, Verbosity verbosity, int maxEditDistance, bool includeUnknown, vector<std::unique_ptr<symspell::SuggestItem>> & suggestions)
    {
        mtx.lock();
        suggestions.clear();
        edits.clear();
        candidates.reserve(32);

        //verbosity=Top: the suggestion with the highest term frequency of the suggestions of smallest edit distance found
        //verbosity=Closest: all suggestions of smallest edit distance found, the suggestions are ordered by term frequency
        //verbosity=All: all suggestions <= maxEditDistance, the suggestions are ordered by edit distance, then by term frequency (slower, no early termination)

        // maxEditDistance used in Lookup can't be bigger than the maxDictionaryEditDistance
        // used to construct the underlying dictionary structure.
        if (maxEditDistance > MaxDictionaryEditDistance())  throw std::invalid_argument("maxEditDistance");
        long suggestionCount = 0;
        size_t suggestionsLen = 0;
        auto wordsFinded = words.find(input);
        int inputLen = strlen(input);
        // early exit - word is too big to possibly match any words
        if (inputLen - maxEditDistance > maxDictionaryWordLength)
        {
            if (includeUnknown && (suggestionsLen == 0))
            {
                std::unique_ptr<SuggestItem> unq(new SuggestItem(_strdup(input), maxEditDistance + 1, 0));
                suggestions.push_back(std::move(unq));
            }

            mtx.unlock();
            return;
        }

        // quick look for exact match

        if (wordsFinded != wordsEnd)
        {
            suggestionCount = wordsFinded->second;
            cerr << "Exact Match "<< input << suggestionCount << endl;
            {
                std::unique_ptr<SuggestItem> unq(new SuggestItem(_strdup(input), 0, suggestionCount));
                unq->ToString();
                suggestions.push_back(std::move(unq));
            }

            ++suggestionsLen;
            // early exit - return exact match, unless caller wants all matches
            if (verbosity != Verbosity::All)
            {
                if (includeUnknown && (suggestionsLen == 0))
                {
                    std::unique_ptr<SuggestItem> unq(new SuggestItem(_strdup(input), maxEditDistance + 1, 0));
                    suggestions.push_back(std::move(unq));
                    ++suggestionsLen;
                }

                mtx.unlock();
                return;
            }
        }

        //early termination, if we only want to check if word in dictionary or get its frequency e.g. for word segmentation
        if (maxEditDistance == 0)
        {
            if (includeUnknown && (suggestionsLen == 0))
            {
                std::unique_ptr<SuggestItem> unq(new SuggestItem(_strdup(input), maxEditDistance + 1, 0));
                suggestions.push_back(std::move(unq));

                ++suggestionsLen;
            }

            mtx.unlock();
            return;
        }


        hashset2.insert(stringHash(input));

        int maxEditDistance2 = maxEditDistance;
        int candidatePointer = 0;

        //add original prefix
        int inputPrefixLen = inputLen;
        if (inputPrefixLen > prefixLength)
        {
            inputPrefixLen = prefixLength;
            candidates.push_back(strndup(input, inputPrefixLen));
        }
        else
        {
            candidates.push_back(_strdup(input));
        }

        size_t candidatesLen = 1; // candidates.size();
        while (candidatePointer < candidatesLen)
        {
            string candidate = candidates[candidatePointer++];
            int candidateLen = strlen(candidate);
            int lengthDiff = inputPrefixLen - candidateLen;

            //save some time - early termination
            //if canddate distance is already higher than suggestion distance, than there are no better suggestions to be expected
            if (lengthDiff > maxEditDistance2)
            {
                // skip to next candidate if Verbosity.All, look no further if Verbosity.Top or Closest
                // (candidates are ordered by delete distance, so none are closer than current)
                if (verbosity == Verbosity::All) continue;
                break;
            }

            auto deletesFinded = deletes.find(stringHash(candidate));
            vector<string>* dictSuggestions = nullptr;

            //read candidate entry from dictionary
            if (deletesFinded != deletesEnd)
            {
                dictSuggestions = &deletesFinded->second;
                size_t dictSuggestionsLen = dictSuggestions->size();
                //iterate through suggestions (to other correct dictionary items) of delete item and add them to suggestion list
                for (int i = 0; i < dictSuggestionsLen; ++i)
                {
                    string suggestion = dictSuggestions->at(i);
                    int suggestionLen = strlen(suggestion);
                    if (strcmp(suggestion, input) == 0) continue;
                    if ((abs(suggestionLen - inputLen) > maxEditDistance2) // input and sugg lengths diff > allowed/current best distance
                        || (suggestionLen < candidateLen) // sugg must be for a different delete string, in same bin only because of hash collision
                        || (suggestionLen == candidateLen && strcmp(suggestion, candidate) != 0)) // if sugg len = delete len, then it either equals delete or is in same bin only because of hash collision
                        continue;
                    auto suggPrefixLen = min(suggestionLen, prefixLength);
                    if (suggPrefixLen > inputPrefixLen && (suggPrefixLen - candidateLen) > maxEditDistance2) continue;

                    //True Damerau-Levenshtein Edit Distance: adjust distance, if both distances>0
                    //We allow simultaneous edits (deletes) of maxEditDistance on on both the dictionary and the input term.
                    //For replaces and adjacent transposes the resulting edit distance stays <= maxEditDistance.
                    //For inserts and deletes the resulting edit distance might exceed maxEditDistance.
                    //To prevent suggestions of a higher edit distance, we need to calculate the resulting edit distance, if there are simultaneous edits on both sides.
                    //Example: (bank==bnak and bank==bink, but bank!=kanb and bank!=xban and bank!=baxn for maxEditDistance=1)
                    //Two deletes on each side of a pair makes them all equal, but the first two pairs have edit distance=1, the others edit distance=2.
                    int distance = 0;
                    int _min = 0;
                    if (candidateLen == 0)
                    {
                        //suggestions which have no common chars with input (inputLen<=maxEditDistance && suggestionLen<=maxEditDistance)
                        distance = max(inputLen, suggestionLen);
                        if (distance > maxEditDistance2 || !hashset2.insert(stringHash(suggestion)).second)
                            continue;
                    }
                    else if (suggestionLen == 1)
                    {
                        if (findCharLocation(input, suggestion[0]) < 0) distance = inputLen; else distance = inputLen - 1;
                        distance = max(inputLen, suggestionLen);
                        if (distance > maxEditDistance2 || !hashset2.insert(stringHash(suggestion)).second)
                            continue;
                    }
                    else
                        if ((prefixLength - maxEditDistance == candidateLen)
                            && (((_min = min(inputLen, suggestionLen) - prefixLength) > 1)
                                && (std::strncmp(input, suggestion, max(inputLen + 1 - _min, suggestionLen + 1 - _min)) != 0) /*(input.substr(inputLen + 1 - _min) != suggestion.substr(suggestionLen + 1 - _min))*/)
                            || ((_min > 0) && (input[inputLen - _min] != suggestion[suggestionLen - _min])
                                && ((input[inputLen - _min - 1] != suggestion[suggestionLen - _min])
                                    || (input[inputLen - _min] != suggestion[suggestionLen - _min - 1]))))
                        {
                            continue;
                        }
                        else
                        {
                            if ((verbosity != Verbosity::All && !DeleteInSuggestionPrefix(candidate, candidateLen, suggestion, suggestionLen)) ||
                                !hashset2.insert(stringHash(suggestion)).second) continue;

                            distance = distanceComparer->Compare(input, suggestion, maxEditDistance2);
                            if (distance < 0) continue;
                        }

                    if (distance <= maxEditDistance2)
                    {
                        auto wordsFindedNew = words.find(strdup(suggestion));
                        if (wordsFindedNew != wordsEnd) cerr << wordsFindedNew->second<<endl;
                        else cerr << "Error, can't find " << suggestion << endl;
                        
                        suggestionCount = words[strdup(suggestion)];
                        cerr << "TEST HERE : " << "\t" << suggestion << "\t" << distance <<  "\t" << suggestionCount<< "\t" <<endl;
                        std::unique_ptr<SuggestItem> si(new SuggestItem(_strdup(suggestion), distance, suggestionCount));
                        if (suggestionsLen > 0)
                        {
                            switch (verbosity)
                            {
                            case Verbosity::Closest:
                            {
                                //we will calculate DamLev distance only to the smallest found distance so far
                                if (distance < maxEditDistance2)
                                {
                                    suggestions.clear();
                                    suggestionsLen = 0;
                                }
                                break;
                            }
                            case Verbosity::Top:
                            {
                                if (distance < maxEditDistance2 || suggestionCount > suggestions[0]->count)
                                {
                                    maxEditDistance2 = distance;
                                    suggestions[0] = std::move(si);
                                }
                                continue;
                            }
                            case Verbosity::All:
                            {
                                break;
                            }
                            }
                        }
                        if (verbosity != Verbosity::All) maxEditDistance2 = distance;
                        suggestions.push_back(std::move(si));
                        ++suggestionsLen;
                    }
                }//end foreach
            }//end if

            //add edits
            //derive edits (deletes) from candidate (input) and add them to candidates list
            //this is a recursive process until the maximum edit distance has been reached
            if ((lengthDiff < maxEditDistance) && (candidateLen <= prefixLength))
            {
                //save some time
                //do not create edits with edit distance smaller than suggestions already found
                if (verbosity != Verbosity::All && lengthDiff >= maxEditDistance2) continue;

                for (int i = 0; i < candidateLen; ++i)
                {
                    char* tmp = new char[candidateLen];
                    std::memcpy(tmp, candidate, i);
                    std::memcpy(tmp + i, candidate + i + 1, candidateLen - 1 - i);
                    tmp[candidateLen - 1] = '\0';

                    if (hashset1.insert(stringHash(tmp)).second)
                    {
                        candidates.push_back(tmp);
                        ++candidatesLen;
                    }
                    else
                        delete[] tmp;
                }
            }
        }//end while

        //sort by ascending edit distance, then by descending word frequency
        if (suggestionsLen > 1)
            std::sort(suggestions.begin(), suggestions.end(), [](std::unique_ptr<symspell::SuggestItem> &l, std::unique_ptr<symspell::SuggestItem> & r)
        {
            return r->CompareTo(*l);
        });


        //cleaning

        //std::cout << hashset2.size() << std::endl;

        auto candidatesEnd = candidates.end();
        for (auto it = candidates.begin(); it != candidatesEnd; ++it)
            delete[] * it;


        candidates.clear();
        hashset1.clear();
        hashset2.clear();

        mtx.unlock();

    }//end if

    bool SymSpell::LoadDictionary(string corpus, int termIndex, int countIndex)
    {
        ifstream stream;
        stream.open(corpus);
        if (!stream.is_open())
            return false;

        char a, b, c;
        a = stream.get();
        b = stream.get();
        c = stream.get();
        if (a != (char)0xEF || b != (char)0xBB || c != (char)0xBF) {
            stream.seekg(0);
        }

        SuggestionStage staging(16384);

        string line;
        while (getline(stream, line))
        {
            vector<string> lineParts;
            std::stringstream ss(line);
            std::string token;
            while (std::getline(ss, token, '\t')) {
                size_t len = token.size();
                char* tmp = new char[len + 1];
                std::memcpy(tmp, token.c_str(), len);
                tmp[len] = '\0';
                lineParts.push_back(tmp);
            }

            if (lineParts.size() >= 2)
            {
                long count = stoll(lineParts[countIndex]);
                CreateDictionaryEntry(lineParts[termIndex], count/*, &staging*/);
            }

            auto linePartsEnd = lineParts.end();
            for (auto it = lineParts.begin(); it != linePartsEnd; ++it)
                delete[] * it;
        }

        stream.close();



        //CommitStaged(staging);
        return true;
    }

    void SymSpell::rempaceSpaces(char* source)
    {
        char* i = source;
        char* j = source;

        do
        {
            *i = *j;
            if (*i != ' ')
                ++i;
        } while (*j++ != 0);
    }

    shared_ptr<WordSegmentationItem> SymSpell::WordSegmentation(string input)
    {
        return WordSegmentation(input, this->maxDictionaryEditDistance, this->maxDictionaryWordLength);
    }

    shared_ptr<WordSegmentationItem> SymSpell::WordSegmentation(string input, size_t maxEditDistance)
    {
        return WordSegmentation(input, maxEditDistance, this->maxDictionaryWordLength);
    }

    shared_ptr<WordSegmentationItem> SymSpell::WordSegmentation(string input, size_t maxEditDistance, size_t maxSegmentationWordLength)
    {
        size_t inputLen = strlen(input);
        int arraySize = min(maxSegmentationWordLength, strlen(input));
        std::vector<shared_ptr<WordSegmentationItem>> compositions;
        compositions.resize(arraySize);

        for (size_t i = 0; i < arraySize; ++i)
        {
            std::shared_ptr<WordSegmentationItem> unq(new WordSegmentationItem());
            compositions[i] = std::move(unq);
        }

        int circularIndex = -1;

        for (int j = 0; j < inputLen; ++j)
        {
            //inner loop (row): all possible part lengths (from start position): part can't be bigger than longest word in dictionary (other than long unknown word)
            int imax = min(inputLen - j, maxSegmentationWordLength);
            for (int i = 1; i <= imax; ++i)
            {
                char* part = new char[i + 1];
                std::memcpy(part, input + j, i);
                part[i] = '\0';

                int separatorLength = 0;
                int topEd = 0;
                double topProbabilityLog = 0;
                char* topResult = nullptr;

                if (isspace(part[0]))
                {
                    size_t partLen = strlen(part);
                    char* tmp = new char[partLen];
                    std::memcpy(tmp, part + 1, partLen - 1);
                    tmp[(i - j)] = '\0';

                    delete[] part;
                    part = tmp;
                }
                else
                {
                    //add ed+1: space did not exist, had to be inserted
                    separatorLength = 1;
                }

                //remove space from part1, add number of removed spaces to topEd
                topEd += strlen(part);
                //remove space
                rempaceSpaces(part);
                //add number of removed spaces to ed
                topEd -= strlen(part);
                vector<std::unique_ptr<symspell::SuggestItem>> results;
                Lookup(part, symspell::Verbosity::Top, maxEditDistance, results);
                if (results.size() > 0)
                {
                    size_t termLen = strlen(results[0]->term);
                    topResult = new char[termLen + 1];
                    std::memcpy(topResult, results[0]->term, termLen);
                    topResult[termLen] = '\0';

                    topEd += results[0]->distance;
                    //Naive Bayes Rule
                    //we assume the word probabilities of two words to be independent
                    //therefore the resulting probability of the word combination is the product of the two word probabilities

                    //instead of computing the product of probabilities we are computing the sum of the logarithm of probabilities
                    //because the probabilities of words are about 10^-10, the product of many such small numbers could exceed (underflow) the floating number range and become zero
                    //log(ab)=log(a)+log(b)
                    topProbabilityLog = (double)log10((double)results[0]->count / (double)N);
                }
                else
                {
                    delete[] topResult;
                    topResult = part;
                    //default, if word not found
                    //otherwise long input text would win as long unknown word (with ed=edmax+1 ), although there there should many spaces inserted
                    topEd += strlen(part);
                    topProbabilityLog = (double)log10(10.0 / (N * pow(10.0, strlen(part))));
                }

                int destinationIndex = ((i + circularIndex) % arraySize);

                //set values in first loop
                if (j == 0)
                {
                    compositions[destinationIndex]->set(_strdup(part), _strdup(topResult), topEd, topProbabilityLog);
                }
                else if ((i == maxSegmentationWordLength)
                    //replace values if better probabilityLogSum, if same edit distance OR one space difference
                    || (((compositions[circularIndex]->distanceSum + topEd == compositions[destinationIndex]->distanceSum) || (compositions[circularIndex]->distanceSum + separatorLength + topEd == compositions[destinationIndex]->distanceSum)) && (compositions[destinationIndex]->probabilityLogSum < compositions[circularIndex]->probabilityLogSum + topProbabilityLog))
                    //replace values if smaller edit distance
                    || (compositions[circularIndex]->distanceSum + separatorLength + topEd < compositions[destinationIndex]->distanceSum))
                {
                    string segmented = compositions[circularIndex]->segmentedString;
                    string corrected = compositions[circularIndex]->correctedString;

                    size_t segmentedLen = strlen(segmented);
                    size_t correctedLen = strlen(corrected);
                    size_t partLen = strlen(part);
                    size_t topResultLen = strlen(topResult);

                    char* segmentedTmp = new char[segmentedLen + partLen + 2];
                    char* correctedTmp = new char[correctedLen + topResultLen + 2];

                    std::memcpy(segmentedTmp, segmented, segmentedLen);
                    std::memcpy(segmentedTmp + segmentedLen, " ", 1);
                    std::memcpy(segmentedTmp + segmentedLen + 1, part, partLen);
                    segmentedTmp[segmentedLen + partLen + 1] = '\0';

                    std::memcpy(correctedTmp, corrected, correctedLen);
                    std::memcpy(correctedTmp + correctedLen, " ", 1);
                    std::memcpy(correctedTmp + correctedLen + 1, topResult, topResultLen);
                    correctedTmp[correctedLen + topResultLen + 1] = '\0';

                    if (strlen(compositions[destinationIndex]->segmentedString) > 0)
                        delete[] compositions[destinationIndex]->segmentedString;

                    if (strlen(compositions[destinationIndex]->correctedString) > 0)
                        delete[] compositions[destinationIndex]->correctedString;

                    compositions[destinationIndex]->set(segmentedTmp,
                        correctedTmp,
                        compositions[circularIndex]->distanceSum + separatorLength + topEd,
                        compositions[circularIndex]->probabilityLogSum + topProbabilityLog);
                }
            }
            ++circularIndex; if (circularIndex == arraySize) circularIndex = 0;
        }
        return compositions[circularIndex];
    }

    bool SymSpell::DeleteInSuggestionPrefix(char const* del, int deleteLen, char const* suggestion, int suggestionLen)
    {
        if (deleteLen == 0) return true;
        if (prefixLength < suggestionLen) suggestionLen = prefixLength;
        int j = 0;
        for (int i = 0; i < deleteLen; ++i)
        {
            char delChar = del[i];
            while (j < suggestionLen && delChar != suggestion[j]) ++j;
            if (j == suggestionLen) return false;
        }
        return true;
    }
}
