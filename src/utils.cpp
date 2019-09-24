#include "utils.h"

using namespace std;

namespace symspell {


    
template <typename T> string toUTF8(const basic_string<T, char_traits<T>, allocator<T>>& source)
{
    string result;

    wstring_convert<codecvt_utf8_utf16<T>, T> convertor;
    result = convertor.to_bytes(source);

    return result;
}

template <typename T> void fromUTF8(const string& source, basic_string<T, char_traits<T>, allocator<T>>& result)
{
    wstring_convert<codecvt_utf8_utf16<T>, T> convertor;
    result = convertor.from_bytes(source);
}


    int levenshtein_dist(string& word1, string& word2) 
    {
        ///
        ///  Please use lower-case strings
        /// word1 : first word
        /// word2 : second word
        /// getPath : bool. If True, sequence of operations to do to go from
        ///           word1 to word2
        ///
        int size1 = (int)word1.size() + 1;
        int size2 = (int)word2.size() + 1;
        int suppr_dist, insert_dist, subs_dist;
        vector<int> dist(size1*size2,0.0);
//         int* dist = new int[(size1)*size2];

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
//         delete[] dist;
        return(res);
    }

    int dl_dist(string& word1, string& word2) 
    {
        /// Damerau-Levenshtein distance
        ///  Please use lower-case strings
        /// word1 : first word
        /// word2 : second word
        ///
        wstring wword1;
        wstring wword2;
        fromUTF8(word1,wword1);
        fromUTF8(word2,wword2);
        int size1 = (int)wword1.size() + 1;
        int size2 = (int)wword2.size() + 1;
        int suppr_dist, insert_dist, subs_dist, val;
        vector<int> dist(size1*size2,0.0);

        for (int i = 0; i < size1; ++i)
            dist[size2*i] = i;
        for (int j = 0; j < size2; ++j)
            dist[j] = j;
        for (int i = 1; i < size1; ++i) {
            for (int j = 1; j < size2; ++j) {
                suppr_dist = dist[size2*(i - 1) + j] + 1;
                insert_dist = dist[size2*i + j - 1] + 1;
                subs_dist = dist[size2*(i - 1) + j - 1];
                if (wword1[i - 1] != wword2[j - 1])  // word indexes are implemented differently.
                    subs_dist += 1;
                val = mini(suppr_dist, insert_dist, subs_dist);
                if (((i >= 2) && (j >= 2)) && ((wword1[i - 1] == wword2[j - 2]) && (wword1[i - 2] == wword2[j - 1])))
                    val = min(dist[size2*(i - 2) + j - 2] + 1, val);
                dist[size2*i + j] = val;
            }
        }

        int res = dist[size1*size2 - 1];
//         delete[] dist;
        return(res);
    }
    int dl_dist_spe(string& word1, string& word2) 
    {
        /// Damerau-Levenshtein distance
        ///  Please use lower-case strings
        /// word1 : first word
        /// word2 : second word
        ///
        wstring wword1;
        wstring wword2;
        fromUTF8(word1,wword1);
        fromUTF8(word2,wword2);
        int size1 = (int)wword1.size() + 1;
        int size2 = (int)wword2.size() + 1;
        int suppr_dist, insert_dist, subs_dist, val;
        vector<int> dist(size1*size2,0.0);

        for (int i = 0; i < size1; ++i)
            dist[size2*i] = i;
        for (int j = 0; j < size2; ++j)
            dist[j] = j;
        for (int i = 1; i < size1; ++i) {
            for (int j = 1; j < size2; ++j) {
                suppr_dist = dist[size2*(i - 1) + j] + 1;
                insert_dist = dist[size2*i + j - 1] + 1;
                subs_dist = dist[size2*(i - 1) + j - 1];
                if (wword1[i - 1] != wword2[j - 1])  // word indexes are implemented differently.
                    if ((wword1[i - 1] == L'é' || wword1[i - 1] == L'è' || wword1[i - 1] == L'ê' || wword1[i - 1] == L'ë' ) && wword2[j - 1] == L'e' ) 
                    {
                        subs_dist += 0;
                    }
                    else
                    if (wword1[i - 1] == L'e'  && (wword2[i - 1] == L'é' || wword2[i - 1] == L'è' || wword2[i - 1] == L'ê' || wword2[i - 1] == L'ë' )) 
                    {
                        subs_dist += 0;
                    }
                    else  
                    if ((wword1[i - 1] == L'à' || wword1[i - 1] == L'â' ) && wword2[j - 1] == L'a' ) 
                    {
                        subs_dist += 0;
                    }
                    else
                    if (wword1[i - 1] == L'a'  && (wword2[i - 1] == L'à' || wword2[i - 1] == L'â' )) 
                    {
                        subs_dist += 0;
                    }
                    else  
                    if ((wword1[i - 1] == L'î' || wword1[i - 1] == L'ï' ) && wword2[j - 1] == L'i' ) 
                    {
                        subs_dist += 0;
                    }
                    else
                    if (wword1[i - 1] == L'i'  && (wword2[i - 1] == L'î' || wword2[i - 1] == L'ï' )) 
                    {
                        subs_dist += 0;
                    }
                    else
                    if ((wword1[i - 1] == L'ô' || wword1[i - 1] == L'ö' ) && wword2[j - 1] == L'o' ) 
                    {
                        subs_dist += 0;
                    }
                    else
                    if (wword1[i - 1] == L'o'  && (wword2[i - 1] == L'ô' || wword2[i - 1] == L'ö' )) 
                    {
                        subs_dist += 0;
                    }
                    else  
                    if ((wword1[i - 1] == L'ü' || wword1[i - 1] == L'ù' ) && wword2[j - 1] == L'u' ) 
                    {
                        subs_dist += 0;
                    }
                    else
                    if (wword1[i - 1] == L'u'  && (wword2[i - 1] == L'ü' || wword2[i - 1] == L'ù' )) 
                    {
                        subs_dist += 0;
                    }
                    else
                        subs_dist += 1;
                val = mini(suppr_dist, insert_dist, subs_dist);
                if (((i >= 2) && (j >= 2)) && ((wword1[i - 1] == wword2[j - 2]) && (wword1[i - 2] == wword2[j - 1])))
                    val = min(dist[size2*(i - 2) + j - 2] + 1, val);
                dist[size2*i + j] = val;
            }
        }

        int res = dist[size1*size2 - 1];
//         delete[] dist;
        return(res);
    }
    float dl_dist_float(string& word1, string& word2) 
    {
        /// Damerau-Levenshtein distance
        ///  Please use lower-case strings
        /// word1 : first word
        /// word2 : second word
        ///
        int size1 = (int)word1.size() + 1;
        int size2 = (int)word2.size() + 1;
        float suppr_dist, insert_dist, subs_dist, val;
        vector<float> dist(size1*size2,0.0);

        for (int i = 0; i < size1; ++i)
            dist[size2*i] = i;
        for (int j = 0; j < size2; ++j)
            dist[j] = j;
        for (int i = 1; i < size1; ++i) {
            for (int j = 1; j < size2; ++j) {
                suppr_dist = dist[size2*(i - 1) + j] + 1;
                insert_dist = dist[size2*i + j - 1] + 1;
                subs_dist = dist[size2*(i - 1) + j - 1];
//                 if (int(word1[i - 1]) < 0) cerr << word1[i - 1] << " " << int(word1[i - 1])<< endl;
                if (word1[i - 1] != word2[j - 1])  // word indexes are implemented differently.
                        subs_dist += 1;
                val = mini(suppr_dist, insert_dist, subs_dist);
                if (((i >= 2) && (j >= 2)) && ((word1[i - 1] == word2[j - 2]) && (word1[i - 2] == word2[j - 1])))
                    val = min(dist[size2*(i - 2) + j - 2] + 1, val);
                dist[size2*i + j] = val;
            }
        }

        float res = dist[size1*size2 - 1];
//         delete[] dist;
        return(res);
    }
}
