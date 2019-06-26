#include "editdistance.h"
namespace symspell {
    int EditDistance::Compare(const char* string1, const char* string2, int maxDistance)
    {
        return this->distanceComparer(string1, string2); 
    }
}