#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <unordered_map>
#include "types.h"

class TTEntry
{
public:
    TTEntry(U64 p_key, int p_value, int p_depth)
        : key(p_key), value(p_value), depth(p_depth)
    {
    }

public:
    U64 key;
    int value;
    int depth;
};

class TranspositionTable
{
public:
    TranspositionTable() = default;
    void Clear() { tt_entries_.clear(); }
    void Store(U64 key, int value, int depth)
    {
        tt_entries_.insert({key, TTEntry(key, value, depth)});
    }
    TTEntry& operator[](U64 key)
    {
        return tt_entries_[key];
    }

private:
    std::unordered_map<U64, TTEntry> tt_entries_;
};

inline TranspositionTable TT;

#endif