#include "editdistance.h"
namespace symspell {
    int EditDistance::Compare(string & string1, string & string2, int maxDistance)
    {
        return this->distanceComparer(string1, string2); 
    }
}