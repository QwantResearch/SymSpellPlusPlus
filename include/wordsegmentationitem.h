#ifndef SYMSPELL_WORDSEGMENTATIONITEM_H
#define SYMSPELL_WORDSEGMENTATIONITEM_H

#include "utils.h"
using namespace std;

namespace symspell {


class WordSegmentationItem
{
public:
    const char* segmentedString{ nullptr };
    const char* correctedString{ nullptr };
    u_int8_t distanceSum = 0;
    double probabilityLogSum = 0;

    WordSegmentationItem() { }
    WordSegmentationItem(const symspell::WordSegmentationItem & p);
    
    WordSegmentationItem& operator=(const WordSegmentationItem&) { return *this; }
    WordSegmentationItem& operator=(WordSegmentationItem&&) { return *this; }

    void set(const char* pSegmentedString, const char* pCorrectedString, u_int8_t pDistanceSum, double pProbabilityLogSum);
    ~WordSegmentationItem();
};
}
#endif // SYMSPELL_WORDSEGMENTATIONITEM_H
