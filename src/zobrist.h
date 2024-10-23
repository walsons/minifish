#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "types.h"
#include <random>
#include <unordered_map>

class Zobrist
{
public:
    static U64 PieceSquareZobrist(Piece p, Square s)
    {
        return GetZobrist().piece_quare_zobrist_[p][s];
    }

    static U64 BlackToMoveZobrist()
    {
        return GetZobrist().black_to_move_zobrist_;
    }

private:
    Zobrist()
    {
        std::default_random_engine e{std::random_device{}()};
        auto rand64 = [&e]() { return e() | (static_cast<U64>(e()) << 32); };
        // Init Zobrist
        for (int i = W_ROOK; i < PT_NUM; ++i)
        {
            for (Square s = SQ_A0; s < SQ_NUM; s += SQ_EAST)
            {
                piece_quare_zobrist_[i][s] = rand64();
            }
        }
        black_to_move_zobrist_ = rand64();
    }

    static Zobrist& GetZobrist()
    {
        static Zobrist zob;
        return zob;
    }

public:
    U64 piece_quare_zobrist_[PT_NUM][90];
    U64 black_to_move_zobrist_;
};

inline std::unordered_map<char, PieceType> PieceTypeMap{
    {'R', W_ROOK},
    { 'N', W_KNIGHT },
    { 'B', W_BISHOP },
    { 'A', W_ADVISOR },
    { 'K', W_KING },
    { 'C', W_CANNON },
    { 'P', W_PAWN },

    { 'r', B_ROOK },
    { 'n', B_KNIGHT },
    { 'b', B_BISHOP },
    { 'a', B_ADVISOR },
    { 'k', B_KING },
    { 'c', B_CANNON },
    { 'p', B_PAWN },

    { '0', NO_PIECE }
};

#endif