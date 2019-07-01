#include "wordsegmentationitem.h"


namespace symspell {

    void WordSegmentationItem::set(string pSegmentedString, string pCorrectedString, u_int8_t pDistanceSum, double pProbabilityLogSum)
    {
        this->segmentedString = pSegmentedString;
        this->correctedString = pCorrectedString;
        this->distanceSum = pDistanceSum;
        this->probabilityLogSum = pProbabilityLogSum;

    }
    WordSegmentationItem::WordSegmentationItem(const WordSegmentationItem& p)
    {
        this->segmentedString = p.segmentedString;
        this->correctedString = p.correctedString;
        this->distanceSum = p.distanceSum;
        this->probabilityLogSum = p.probabilityLogSum;
    }
    WordSegmentationItem::~WordSegmentationItem()
    {
        segmentedString.clear();
        correctedString.clear();
    }
}