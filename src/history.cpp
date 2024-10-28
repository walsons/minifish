#include "history.h"

void History::Success(const Position &position, Move move, int depth) 
{
    auto from = move.MoveFrom();
    auto to = move.MoveTo();
    auto piece = position.piece_from_square(from);
    history_[PieceIndex(piece)][to] += depth * depth;
    if (history_[PieceIndex(piece)][to] > kHistoryMax) 
    {
        for (int i = 0; i < PieceIndex(Piece::PIECE_NUM); ++i) 
        {
            for (int j = 0; j < Square::SQ_NUM; ++j) 
            {
                history_[i][j] >>= 1;
            }
        }
    }
}

int History::HistoryValue(const Position &position, Move move) const 
{
    auto from = move.MoveFrom();
    auto to = move.MoveTo();
    auto piece = position.piece_from_square(from);
    return history_[PieceIndex(piece)][to];
}

History HISTORY;