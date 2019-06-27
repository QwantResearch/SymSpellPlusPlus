
#include "suggestionstage.h"


namespace symspell {

    SuggestionStage::SuggestionStage(size_t initialCapacity)
    {
        Deletes.reserve(initialCapacity);
        Nodes.Reserve(initialCapacity * 2);
    }
    void SuggestionStage::Clear()
    {
        Deletes.clear();
        Nodes.Clear();
        DeletesEnd = Deletes.end();
    }
    void SuggestionStage::Add(size_t deleteHash, string suggestion)
    {
        auto deletesFinded = Deletes.find(deleteHash);
//        Entry* entry = nullptr;
        Entry entry;
        if (deletesFinded == DeletesEnd) {
//             entry = new Entry;
            entry.count = 0;
            entry.first = -1;
        }
        else
            entry = deletesFinded->second;

        int64_t next = entry.first;
        ++entry.count;
        entry.first = Nodes.Count;
        Deletes[deleteHash] = entry;
        Node item;
        item.next = next;
        item.suggestion = suggestion;
        Nodes.Add(item);
    }
    void SuggestionStage::CommitTo(unordered_map< size_t, vector< string > >& permanentDeletes)
    {
        auto permanentDeletesEnd = permanentDeletes.end();
        for (auto it = Deletes.begin(); it != DeletesEnd; ++it)
        {
            auto permanentDeletesFinded = permanentDeletes.find(it->first);
//             vector<string>* suggestions = nullptr;
            vector<string> suggestions;
            size_t i;
            if (permanentDeletesFinded != permanentDeletesEnd)
            {
                suggestions = permanentDeletesFinded->second;
                i = suggestions.size();
                vector<string> newSuggestions;
                newSuggestions.reserve(suggestions.size() + it->second.count);

                std::copy(suggestions.begin(), suggestions.end(), back_inserter(newSuggestions));
                permanentDeletes[it->first] = newSuggestions;
            }
            else
            {
                i = 0;
//                 suggestions = new vector<string>;
                int32_t count = it->second.count;
                suggestions.reserve(count);
                permanentDeletes[it->first] = suggestions;
            }

            int next = it->second.first;
            while (next >= 0)
            {
                auto node = Nodes.at(next);
                suggestions[i] = node.suggestion;
                next = node.next;
                ++i;
            }
        }
    }

}