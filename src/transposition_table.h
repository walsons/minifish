#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <unordered_map>
#include "types.h"

class TTEntry
{
public:
    TTEntry() : key(0), value(0), depth(0), generation(0), move(0) {}
    TTEntry(U64 p_key, int p_value, int p_depth, int p_generation, Move p_move)
        : key(p_key), value(p_value), depth(p_depth), generation(p_generation), move(p_move)
    {
    }

public:
    U64 key;
    int value;
    int depth;
    int generation;
    Move move;
};

class TranspositionTable
{
public:
    TranspositionTable(int sizeOfMB);
    void NewSearch();
    void Clear();
    void Store(U64 key, int value, int depth, Move move);
    TTEntry* operator[](U64 key);

private:
    int size_;
    // number of items with the same key in hash table
    const int same_key_item_num = 4;
    TTEntry* tt_entry_;
    int generation_;
};

#endif