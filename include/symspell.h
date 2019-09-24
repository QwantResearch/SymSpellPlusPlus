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
        SymSpell(int initialCapacity = defaultInitialCapacity, int maxDictionaryEditDistance = defaultMaxEditDistance, int prefixLength = defaultPrefixLength, int countThreshold = defaultCountThreshold, int compactLevel = defaultCompactLevel);
        ~SymSpell();
        bool CreateDictionaryEntry(string key, long count, SuggestionStage * staging = nullptr);
        void EditsPrefix(string key, unordered_set<size_t>& hashSet);
        void Edits(string word, int editDistance, unordered_set<size_t> & deleteWords);
        void PurgeBelowThresholdWords();
        void CommitStaged(SuggestionStage staging);
        void Lookup(string& input, Verbosity verbosity, vector<std::unique_ptr<symspell::SuggestItem>> & items);
        void Lookup(string& input, Verbosity verbosity, int maxEditDistance, vector<std::unique_ptr<symspell::SuggestItem>> & items);
        void Lookup(string& input, Verbosity verbosity, int maxEditDistance, bool includeUnknown, vector<std::unique_ptr<symspell::SuggestItem>> & suggestions);
        bool LoadDictionary(string corpus, int termIndex, int countIndex);
        void rempaceSpaces(string& source);
        shared_ptr<WordSegmentationItem> WordSegmentation(string& input);
        shared_ptr<WordSegmentationItem> WordSegmentation(string& input, size_t maxEditDistance);
        shared_ptr<WordSegmentationItem> WordSegmentation(string& input, size_t maxEditDistance, size_t maxSegmentationWordLength);
        /// <summary>Maximum edit distance for dictionary precalculation.</summary>
        size_t MaxDictionaryEditDistance() { return this->maxDictionaryEditDistance; }

        /// <summary>Length of prefix, from which deletes are generated.</summary>
        size_t PrefixLength() { return this->prefixLength; }

        /// <summary>Length of longest word in the dictionary.</summary>
        size_t MaxLength() { return this->maxDictionaryWordLength; }

        /// <summary>Count threshold for a word to be considered a valid word for spelling correction.</summary>
        long CountThreshold() { return this->countThreshold; }

        /// <summary>Number of unique words in the dictionary.</summary>
        size_t WordCount() { return this->words.size(); }

        /// <summary>Number of word prefixes and intermediate word deletes encoded in the dictionary.</summary>
        size_t EntryCount() { return this->deletes.size(); }
        
        inline void setDistanceAlgorithm(EditDistance::DistanceAlgorithm ed) {this->distanceComparer = new EditDistance(ed);} 

    private:
        int initialCapacity;
        int maxDictionaryEditDistance;
        int prefixLength; //prefix length  5..7
        long countThreshold; //a treshold might be specifid, when a term occurs so frequently in the corpus that it is considered a valid word for spelling correction
        uint compactMask;
        EditDistance::DistanceAlgorithm distanceAlgorithm = EditDistance::DistanceAlgorithm::DamerauOSA;
        size_t maxDictionaryWordLength; //maximum dictionary term length
        std::mutex mtx;

        vector<string> candidates;

        EditDistance* distanceComparer{ nullptr };
        unordered_set<size_t> edits;
        unordered_set<size_t> hashset1; //TODO: use unordered_set<size_t> hashset1;
        unordered_set<size_t> hashset2; //TODO: use unordered_set<size_t> hashset1;
        unordered_set<size_t>::iterator hashset2End;  //TODO: use unordered_set<size_t>::iterator hashset2End; 
        hash_c_string stringHash;
        long N = 1024908267229;

        unordered_map<size_t, vector<string>> deletes;
        unordered_map<size_t, vector<string>>::iterator deletesEnd;

        // Dictionary of unique correct spelling words, and the frequency count for each word.
        unordered_map<string, long> words;
        unordered_map<string, long>::iterator wordsEnd;

        // Dictionary of unique words that are below the count threshold for being considered correct spellings.
        unordered_map<string, long> belowThresholdWords;
        unordered_map<string, long>::iterator belowThresholdWordsEnd;

        bool DeleteInSuggestionPrefix(string del, int deleteLen, string suggestion, int suggestionLen);
    };
}

#endif // SYMSPELL6_H
