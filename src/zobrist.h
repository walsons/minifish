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
        return GetZobrist().piece_quare_zobrist_[PieceIndex(p)][s];
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
        for (int i = PieceIndex(Piece::START); i < PieceIndex(Piece::PIECE_NUM); ++i)
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
    U64 piece_quare_zobrist_[PieceIndex(Piece::PIECE_NUM)][90];
    U64 black_to_move_zobrist_;
};

#endif