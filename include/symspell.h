#ifndef SYMSPELL6_H
#define SYMSPELL6_H


#include "utils.h"
#include "suggestitem.h"
#include "suggestionstage.h"
#include "wordsegmentationitem.h"
#include "editdistance.h"



#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 1

using namespace std;

namespace symspell {

    class SymSpell {
    public:
        SymSpell(int32_t initialCapacity = defaultInitialCapacity, int32_t maxDictionaryEditDistance = defaultMaxEditDistance, int32_t prefixLength = defaultPrefixLength, int32_t countThreshold = defaultCountThreshold, int32_t compactLevel = defaultCompactLevel);
        ~SymSpell();
        bool CreateDictionaryEntry(const char * key, int64_t count, SuggestionStage * staging = nullptr);
        void EditsPrefix(const char* key, CUSTOM_SET<size_t>& hashSet);
        void Edits(const char * word, int32_t editDistance, CUSTOM_SET<size_t> & deleteWords);
        void PurgeBelowThresholdWords();
        void CommitStaged(SuggestionStage staging);
        void Lookup(const char * input, Verbosity verbosity, vector<std::unique_ptr<symspell::SuggestItem>> & items);
        void Lookup(const char * input, Verbosity verbosity, int32_t maxEditDistance, vector<std::unique_ptr<symspell::SuggestItem>> & items);
        void Lookup(const char * input, Verbosity verbosity, int32_t maxEditDistance, bool includeUnknown, vector<std::unique_ptr<symspell::SuggestItem>> & suggestions);
        bool LoadDictionary(const char* corpus, int termIndex, int countIndex);
        void rempaceSpaces(char* source);
        shared_ptr<WordSegmentationItem> WordSegmentation(const char* input);
        shared_ptr<WordSegmentationItem> WordSegmentation(const char* input, size_t maxEditDistance);
        shared_ptr<WordSegmentationItem> WordSegmentation(const char* input, size_t maxEditDistance, size_t maxSegmentationWordLength);
        /// <summary>Maximum edit distance for dictionary precalculation.</summary>
        size_t MaxDictionaryEditDistance() { return this->maxDictionaryEditDistance; }

        /// <summary>Length of prefix, from which deletes are generated.</summary>
        size_t PrefixLength() { return this->prefixLength; }

        /// <summary>Length of longest word in the dictionary.</summary>
        size_t MaxLength() { return this->maxDictionaryWordLength; }

        /// <summary>Count threshold for a word to be considered a valid word for spelling correction.</summary>
        int64_t CountThreshold() { return this->countThreshold; }

        /// <summary>Number of unique words in the dictionary.</summary>
        size_t WordCount() { return this->words.size(); }

        /// <summary>Number of word prefixes and intermediate word deletes encoded in the dictionary.</summary>
        size_t EntryCount() { return this->deletes.size(); }

    private:
        int32_t initialCapacity;
        int32_t maxDictionaryEditDistance;
        int32_t prefixLength; //prefix length  5..7
        int64_t countThreshold; //a treshold might be specifid, when a term occurs so frequently in the corpus that it is considered a valid word for spelling correction
        uint32_t compactMask;
        EditDistance::DistanceAlgorithm distanceAlgorithm = EditDistance::DistanceAlgorithm::DamerauOSA;
        size_t maxDictionaryWordLength; //maximum dictionary term length
        std::mutex mtx;

        vector<const char*> candidates;

        EditDistance* distanceComparer{ nullptr };
        CUSTOM_SET<size_t> edits;
        CUSTOM_SET<size_t> hashset1; //TODO: use CUSTOM_SET<size_t> hashset1;
        CUSTOM_SET<size_t> hashset2; //TODO: use CUSTOM_SET<size_t> hashset1;
        CUSTOM_SET<size_t>::iterator hashset2End;  //TODO: use CUSTOM_SET<size_t>::iterator hashset2End; 
        hash_c_string stringHash;
        long N = 1024908267229;

        CUSTOM_MAP<size_t, vector<const char*>> deletes;
        CUSTOM_MAP<size_t, vector<const char*>>::iterator deletesEnd;

        // Dictionary of unique correct spelling words, and the frequency count for each word.
        CUSTOM_MAP<const char*, int64_t, hash_c_string, comp_c_string> words;
        CUSTOM_MAP<const char*, int64_t, hash_c_string, comp_c_string>::iterator wordsEnd;

        // Dictionary of unique words that are below the count threshold for being considered correct spellings.
        CUSTOM_MAP<const char*, int64_t, hash_c_string, comp_c_string> belowThresholdWords;
        CUSTOM_MAP<const char*, int64_t, hash_c_string, comp_c_string>::iterator belowThresholdWordsEnd;

        bool DeleteInSuggestionPrefix(char const* del, int deleteLen, char const* suggestion, int suggestionLen);
    };
}

#endif // SYMSPELL6_H
