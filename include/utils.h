#ifndef SYMSPELL_UTILS_H
#define SYMSPELL_UTILS_H

#include <memory>
#include <stdint.h>
#include <vector>
#include <functional>
#include <cstring>
#include <exception>
#include <limits>
#include <stdio.h>
#include <algorithm>
#include <queue>
#include <mutex>
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <math.h>
#include <codecvt>
#include <locale>

// #include <sparsepp/spp.h>
#define _strdup strdup

#define CUSTOM_MAP unordered_map
#define CUSTOM_SET unordered_set

// typedef unsigned int u_int8_t;
// 
// typedef long int;
// typedef unsigned long u_int;
// 
// typedef long long long;
// typedef unsigned long long u_long;

// #define SPP_USE_SPP_ALLOC 1
// #define CUSTOM_MAP sparse_hash_map
// #define CUSTOM_SET sparse_hash_set
// 
// using spp::sparse_hash_map;
// using spp::sparse_hash_set;
// 
using namespace std;

namespace symspell {
#define defaultMaxEditDistance 2
#define defaultPrefixLength 7
#define defaultCountThreshold 1
#define defaultInitialCapacity 1024
#define defaultCompactLevel 16
#define mini(a, b, c) (min(a, min(b, c)))



    enum class Verbosity
    {
        /// <summary>Top suggestion with the highest term frequency of the suggestions of smallest edit distance found.</summary>
        Top,
        /// <summary>All suggestions of smallest edit distance found, suggestions ordered by term frequency.</summary>
        Closest,
        /// <summary>All suggestions within maxEditDistance, suggestions ordered by edit distance
        /// , then by term frequency (slower, no early termination).</summary>
        All
    };
  
    class Node
    {
    public:
        string suggestion;
        long next;
    };

    class Entry
    {
    public:
        long count;
        long first;
    };  
    namespace {
        int findCharLocation(string & text, char ch)
        {
            return (int)text.find(ch);
            
//             if (finded == nullptr)
//                 return -1;
// 
//             return finded - text + 1;
        }
    }

    struct Hash64 {
        size_t operator()(ulong k) const { return (k ^ 14695981039346656037ULL) * 1099511628211ULL; }
    };

    struct comp_c_string {
        bool operator()(string& s1, string& s2) const {
            return (s1 == s2) || (s1.compare(s2) == 0);
        }
    };

    struct hash_c_string {
        void hash_combine(size_t& seed, const char& v)
        {
            seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        std::size_t operator() (string& p) const
        {
            size_t hash = 0;
            for (auto inc_p = p.begin(); inc_p != p.end(); inc_p++)
                hash ^= *inc_p + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            return hash;
        }
    };
//     char* strndup(string s1, size_t n);
    inline string strndup(string& s1, size_t n)
    {
        return s1.substr(0,n);
//         char *copy = (char*)malloc(n + 1);
//         memcpy(copy, s1, n);
//         copy[n] = 0;
//         return copy;
    }

    /*
 * Copied from https://github.com/PierreBoyeau/levenshtein_distance
 * ########## BEGIN ##########
 */

    int levenshtein_dist(string& word1, string& word2);
    int dl_dist(string& word1, string& word2);
    int dl_dist_spe(string& word1, string& word2);
    float dl_dist_float(string& word1, string& word2);
    
}

#endif // SYMSPELL_UTILS_H
