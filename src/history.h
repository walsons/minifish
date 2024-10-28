#ifndef HISTORY_H
#define HISTORY_H

#include "function.h"
#include "position.h"

class History {
public:
    History();
    void Clear();
    void Success(const Position &position, Move move, int depth);
    int HistoryValue(const Position &position, Move move) const;

public:
    static const int kHistoryMax = 1 << 14;

private:
    int history_[PieceIndex(Piece::PIECE_NUM)][Square::SQ_NUM];
};

#endif