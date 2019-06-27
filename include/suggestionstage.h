#ifndef SYMSPELL_SUGGESTIONSTAGE_H
#define SYMSPELL_SUGGESTIONSTAGE_H

#include "utils.h"
# include "chunkarray.h"

using namespace std;

namespace symspell {

class SuggestionStage
{
public:
    unordered_map<size_t, Entry> Deletes;
    unordered_map<size_t, Entry>::iterator DeletesEnd;

    ChunkArray<Node> Nodes;
    SuggestionStage(size_t initialCapacity);
    size_t DeleteCount() { return Deletes.size(); }
    size_t NodeCount() { return Nodes.Count; }
    void Clear();
    void Add(size_t deleteHash, string suggestion);
    void CommitTo(unordered_map<size_t, vector<string>> & permanentDeletes);

};
}

#endif  // SYMSPELL_SUGGESTIONSTAGE_H
