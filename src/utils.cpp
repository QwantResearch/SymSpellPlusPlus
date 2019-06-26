#include "utils.h"

namespace symspell {
    int levenshtein_dist(char const* word1, char const* word2) 
    {
        ///
        ///  Please use lower-case strings
        /// word1 : first word
        /// word2 : second word
        /// getPath : bool. If True, sequence of operations to do to go from
        ///           word1 to word2
        ///
        int size1 = strlen(word1) + 1, size2 = strlen(word2) + 1;
        int suppr_dist, insert_dist, subs_dist;
        int* dist = new int[(size1)*size2];

        for (int i = 0; i < size1; ++i)
            dist[size2*i] = i;
        for (int j = 0; j < size2; ++j)
            dist[j] = j;
        for (int i = 1; i < size1; ++i) {
            for (int j = 1; j < size2; ++j) {
                suppr_dist = dist[size2*(i - 1) + j] + 1;
                insert_dist = dist[size2*i + j - 1] + 1;
                subs_dist = dist[size2*(i - 1) + j - 1];
                if (word1[i - 1] != word2[j - 1]) {  // word indexes are implemented differently.
                    subs_dist += 1;
                }
                dist[size2*i + j] = mini(suppr_dist, insert_dist, subs_dist);
            }
        }

        // --------------------------------------------------------
        int res = dist[size1 * size2 - 1];
        delete[] dist;
        return(res);
    }

    inline int dl_dist(char const* word1, char const* word2) 
    {
        /// Damerau-Levenshtein distance
        ///  Please use lower-case strings
        /// word1 : first word
        /// word2 : second word
        ///
        int size1 = strlen(word1) + 1, size2 = strlen(word2) + 1;
        int suppr_dist, insert_dist, subs_dist, val;
        int* dist = new int[size1*size2];

        for (int i = 0; i < size1; ++i)
            dist[size2*i] = i;
        for (int j = 0; j < size2; ++j)
            dist[j] = j;
        for (int i = 1; i < size1; ++i) {
            for (int j = 1; j < size2; ++j) {
                suppr_dist = dist[size2*(i - 1) + j] + 1;
                insert_dist = dist[size2*i + j - 1] + 1;
                subs_dist = dist[size2*(i - 1) + j - 1];
                if (word1[i - 1] != word2[j - 1])  // word indexes are implemented differently.
                    subs_dist += 1;
                val = mini(suppr_dist, insert_dist, subs_dist);
                if (((i >= 2) && (j >= 2)) && ((word1[i - 1] == word2[j - 2]) && (word1[i - 2] == word2[j - 1])))
                    val = min(dist[size2*(i - 2) + j - 2] + 1, val);
                dist[size2*i + j] = val;
            }
        }

        int res = dist[size1*size2 - 1];
        delete[] dist;
        return(res);
    }
}