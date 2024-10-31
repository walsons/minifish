#include "move_picker.h"
#include <algorithm>

extern History HISTORY;

MovePicker::MovePicker(const Position& position, Move ttMove)
    : position_(position)
    , tt_move_(ttMove)
    , move_generator_(position)
    , phase_(Phase::TT)
{
}
    
std::list<ScoreMove> MovePicker::GenerateCheckMove()
{
    std::list<ScoreMove> moves;
    auto checkMoves = move_generator_.check_moves();
    for (auto move : checkMoves)
    {
        moves.push_back({ move, 0 });
    }
    return moves;
}

std::list<ScoreMove> MovePicker::GenerateCaptureMove()
{
    std::list<ScoreMove> moves;
    auto captureMoves = move_generator_.capture_moves();
    for (auto move : captureMoves)
    {
        int score = MVV_LVA[PieceIndex(position_.piece_from_square(move.MoveFrom()))][PieceIndex(position_.piece_from_square(move.MoveTo()))];
        moves.push_back({ move, score });
    }
    return moves;
}

std::list<ScoreMove> MovePicker::GenerateNonCaptureMove()
{
    std::list<ScoreMove> moves;
    auto nonCaptureMoves = move_generator_.non_capture_moves();
    for (auto move : nonCaptureMoves)
    {
        int score = HISTORY.HistoryValue(position_, move);
        moves.push_back({ move, score });
    }
    return moves;
}

Move MovePicker::NextMove()
{
    Move selectedMove;
    switch (phase_)
    {
    case Phase::TT:
    {
        phase_ = Phase::CHECK;
        moves_ = GenerateCheckMove();
        if (tt_move_ != 0)
        {
            selectedMove = tt_move_;
            break;
        }
        else
        {
            // continue to the next case, so there is not break here
        }
    }
    case Phase::CHECK:
    {
        if (!moves_.empty())
        {
            auto iter = moves_.begin();
            selectedMove = iter->move;
            moves_.erase(iter);
            break;
        }
        else
        {
            phase_ = Phase::CAPTURE;
            moves_ = GenerateCaptureMove();
            // continue to the next case, so there is not break here
        }
    }
    case Phase::CAPTURE:
    {
        if (!moves_.empty())
        {
            auto bestMoveIter = moves_.begin();
            auto iter = moves_.begin();
            ++iter;
            for (; iter != moves_.end(); ++iter)
            {
                if (iter->score > bestMoveIter->score)
                {
                    bestMoveIter = iter;
                }
            }
            selectedMove = bestMoveIter->move;
            moves_.erase(bestMoveIter);
            break;
        }
        else
        {
            phase_ = Phase::NON_CAPTURE;
            moves_ = GenerateNonCaptureMove();
            // continue to the next case, so there is not break here
        }
    }
    case Phase::NON_CAPTURE:
    {
        if (!moves_.empty())
        {
            auto bestMoveIter = moves_.begin();
            auto iter = moves_.begin();
            ++iter;
            for (; iter != moves_.end(); ++iter)
            {
                if (iter->score > bestMoveIter->score)
                {
                    bestMoveIter = iter;
                }
            }
            selectedMove = bestMoveIter->move;
            moves_.erase(bestMoveIter);
            break;
        }
        else
        {
            phase_ = Phase::END;
            // continue to the next case, so there is not break here
        }
    }
    default:
        break;
    }
    return selectedMove;
}

bool MovePicker::NoLegalMove()
{
    return move_generator_.check_moves().empty() 
        && move_generator_.capture_moves().empty() 
        && move_generator_.non_capture_moves().empty();
}