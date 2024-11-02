#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "types.h"
// #include <random>
#include <unordered_map>

// <http://vigna.di.unimi.it/ftp/papers/xorshift.pdf>
// Pseudo random number generator
class PRNG {
    using uint64_t = unsigned long long;
    uint64_t s;

    uint64_t rand64() {

        s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
        return s * 2685821657736338717LL;
    }

   public:
    PRNG(uint64_t seed) :
        s(seed) {
    }

    template<typename T>
    T rand() {
        return T(rand64());
    }
};

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
        // std::default_random_engine e{std::random_device{}()};
        // auto rand64 = [&e]() { return e() | (static_cast<U64>(e()) << 32); };
        PRNG randomNumberGenerator(1);
        auto rand64 = [&randomNumberGenerator]() { return randomNumberGenerator.rand<U64>(); };

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