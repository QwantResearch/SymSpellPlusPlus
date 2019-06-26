#ifndef SYMSPELL_SUGGESTIONSTAGE_H
#define SYMSPELL_SUGGESTIONSTAGE_H

#include "utils.h"

using namespace std;

namespace symspell {

class SuggestionStage
{
public:
    CUSTOM_MAP<size_t, Entry*> Deletes;
    CUSTOM_MAP<size_t, Entry*>::iterator DeletesEnd;

    ChunkArray<Node> Nodes;
    SuggestionStage(size_t initialCapacity);
    size_t DeleteCount() { return Deletes.size(); }
    size_t NodeCount() { return Nodes.Count; }
    void Clear();
    void Add(size_t deleteHash, const char* suggestion);
    void CommitTo(CUSTOM_MAP<size_t, vector<const char*>> & permanentDeletes);

};
}

#endif  // SYMSPELL_SUGGESTIONSTAGE_H
