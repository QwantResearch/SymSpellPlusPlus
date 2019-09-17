#ifndef SYMSPELL_EDITDISTANCE_H
#define SYMSPELL_EDITDISTANCE_H

#include "utils.h"
using namespace std;

namespace symspell {

class EditDistance {
public:
    /// <summary>Wrapper for third party edit distance algorithms.</summary>

    /// <summary>Supported edit distance algorithms.</summary>
    enum class DistanceAlgorithm {
        /// <summary>Levenshtein algorithm.</summary>
        Levenshtein,
        /// <summary>Damerau optimal string alignment algorithm.</summary>
        DamerauOSA,
        /// <summary>Damerau optimal string alignment algorithm considering 'e' with accent as equivalent.</summary>
        DamerauOSAspe
    };

    EditDistance(DistanceAlgorithm algorithm) 
    {
        this->algorithm = algorithm;
        switch (algorithm) {
        case DistanceAlgorithm::DamerauOSA: this->distanceComparer = dl_dist; break;
        case DistanceAlgorithm::DamerauOSAspe: this->distanceComparer = dl_dist_spe; break;
        case DistanceAlgorithm::Levenshtein: this->distanceComparer = levenshtein_dist; break;
        default: throw std::invalid_argument("Unknown distance algorithm.");
        }
    }

    int Compare(string & string1, string & string2, int maxDistance);

private:
    DistanceAlgorithm algorithm;
    int(*distanceComparer)(string &, string &);
};
}
#endif // SYMSPELL_EDITDISTANCE_H
