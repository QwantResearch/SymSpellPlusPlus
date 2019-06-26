#ifndef SYMSPELL_SUGGESTITEM_H
#define SYMSPELL_SUGGESTITEM_H

#include "utils.h"
using namespace std;

namespace symspell {


class SuggestItem
{
public:
    /// <summary>The suggested correctly spelled word.</summary>
    const char* term;
    /// <summary>Edit distance between searched for word and suggestion.</summary>
    u_int8_t distance = 0;
    /// <summary>Frequency of suggestion in the dictionary (a measure of how common the word is).</summary>
    int64_t count = 0;

    SuggestItem() { }
    SuggestItem(const symspell::SuggestItem & p);

    SuggestItem(const char* term, int32_t distance, int64_t count);

    ~SuggestItem();

    bool CompareTo(SuggestItem const& other);

    bool operator == (const SuggestItem &ref) const;

    std::size_t GetHashCode();

    SuggestItem ShallowCopy();
};

}
#endif // SYMSPELL_SUGGESTITEM_H