#ifndef SYMSPELL_CHUNKARRAY_H
#define SYMSPELL_CHUNKARRAY_H

#include "utils.h"

using namespace std;
namespace symspell {

template<typename T>
class ChunkArray
{
    public:
        vector<vector<T>> Values; //todo: use pointer array
        size_t Count;

        ChunkArray()
        {
            Count = 0;
        }

        void Reserve(size_t initialCapacity)
        {
            size_t chunks = (initialCapacity + ChunkSize - 1) / ChunkSize;
            Values.resize(chunks);
            for (size_t i = 0; i < chunks; ++i)
            {
                Values[i].resize(ChunkSize);
            }
        }

        size_t Add(T & value)
        {
            if (Count == Capacity())
            {
                Values.push_back(vector<T>());
                Values[Values.size() - 1].resize(ChunkSize);
            }

            int row = Row(Count);
            int col = Col(Count);

            Values[row][col] = value;
            return Count++;
        }

        void Clear()
        {
            Count = 0;
        }

        T& at(size_t index)
        {
            return Values[Row(index)][Col(index)];
        }

        void set(size_t index, T &value)
        {
            Values[Row(index)][Col(index)] = value;
        }

    private:
        const int32_t ChunkSize = 4096; //this must be a power of 2, otherwise can't optimize Row and Col functions
        const int32_t DivShift = 12; // number of bits to shift right to do division by ChunkSize (the bit position of ChunkSize)
        int Row(uint32_t index) { return index >> DivShift; } // same as index / ChunkSize
        int32_t Col(uint32_t index) { return index & (ChunkSize - 1); } //same as index % ChunkSize
        int32_t Capacity() { return Values.size() * ChunkSize; }
};

}
#endif // SYMSPELL_CHUNKARRAY_H