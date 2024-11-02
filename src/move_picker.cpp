#include "move_picker.h"
#include <algorithm>

extern History HISTORY;

MovePicker::MovePicker(const Position& position, Move ttMove, Move killerMove[])
    : position_(position)
    , tt_move_(ttMove)
    , killer_move1_(killerMove[0])
    , killer_move2_(killerMove[1])
    , move_generator_(position)
    , phase_(Phase::TT)
{
    bool tt_move_valid = false;
    bool killer_move1_valid = false;
    bool killer_move2_valid = false;
    // ensure tt_move and killer_move is valid
    auto validMove = [&](std::vector<Move> moves) {
        for (auto iter = moves.begin(); iter != moves.end() && !(tt_move_valid && killer_move1_valid && killer_move2_valid); ++iter)
        {
            if (tt_move_ == *iter)
                tt_move_valid = true;
            if (killer_move1_ == *iter)
                killer_move1_valid = true;
            if (killer_move2_ == *iter)
                killer_move2_valid = true;
        }

    };
    validMove(move_generator_.check_moves_);
    validMove(move_generator_.capture_moves_);
    validMove(move_generator_.non_capture_moves_);

    if (!tt_move_valid)
        tt_move_valid = 0;
    if (!killer_move1_valid)
        killer_move1_ = 0;
    if (!killer_move2_valid)
        killer_move2_ = 0;
}
    
std::list<ScoreMove> MovePicker::GenerateCheckMove()
{
    std::list<ScoreMove> moves;
    auto &checkMoves = move_generator_.check_moves_;
    for (auto move : checkMoves)
    {
        moves.push_back({ move, 0 });
    }
    return moves;
}

std::list<ScoreMove> MovePicker::GenerateCaptureMove()
{
    std::list<ScoreMove> moves;
    auto &captureMoves = move_generator_.capture_moves_;
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
    auto &nonCaptureMoves = move_generator_.non_capture_moves_;
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
    bool loop = true;

    while (selectedMove == 0 && loop)
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
        }
        else
        {
            switch (phase_)
            {
            case Phase::TT:
            {
                if (tt_move_ != 0)
                    moves_ = { ScoreMove{tt_move_, 0} };
                phase_ = Phase::CHECK;
            }
            break;
            case Phase::CHECK:
            {
                moves_ = GenerateCheckMove();
                phase_ = Phase::CAPTURE;
            }
            break;
            case Phase::CAPTURE:
            {
                moves_ = GenerateCaptureMove();
                phase_ = Phase::KILLER;
            }
            break;
            case Phase::KILLER:
            {
                if (killer_move1_ != 0)
                    moves_.push_back(ScoreMove{killer_move1_, 0});
                if (killer_move2_ != 0)
                    moves_.push_back(ScoreMove{killer_move2_, 0});
                phase_ = Phase::NON_CAPTURE;
            }
            break;
            case Phase::NON_CAPTURE:
            {
                moves_ = GenerateNonCaptureMove();
                phase_ = Phase::END;
            }
            break;
            case Phase::END:
            {
                loop = false;
            }
            break;
            }
        }
    }

    return selectedMove;



    // switch (phase_)
    // {
    // case Phase::TT:
    // {
    //     phase_ = Phase::CHECK;
    //     moves_ = GenerateCheckMove();
    //     if (tt_move_ != 0)
    //     {
    //         selectedMove = tt_move_;
    //         break;
    //     }
    //     else
    //     {
    //         // continue to the next case, so there is not break here
    //     }
    // }
    // case Phase::CHECK:
    // {
    //     if (!moves_.empty())
    //     {
    //         auto iter = moves_.begin();
    //         selectedMove = iter->move;
    //         moves_.erase(iter);
    //         break;
    //     }
    //     else
    //     {
    //         phase_ = Phase::CAPTURE;
    //         moves_ = GenerateCaptureMove();
    //         // continue to the next case, so there is not break here
    //     }
    // }
    // case Phase::CAPTURE:
    // {
    //     if (!moves_.empty())
    //     {
    //         auto bestMoveIter = moves_.begin();
    //         auto iter = moves_.begin();
    //         ++iter;
    //         for (; iter != moves_.end(); ++iter)
    //         {
    //             if (iter->score > bestMoveIter->score)
    //             {
    //                 bestMoveIter = iter;
    //             }
    //         }
    //         selectedMove = bestMoveIter->move;
    //         moves_.erase(bestMoveIter);
    //         break;
    //     }
    //     else
    //     {
    //         phase_ = Phase::KILLER;
    //         // continue to the next case, so there is not break here
    //     }
    // }
    // case Phase::KILLER:
    // {
    //     phase_ = Phase::NON_CAPTURE;
    //     moves_ = GenerateNonCaptureMove();
    //     if (killer_move_ != 0)
    //     {
    //         selectedMove = killer_move_;
    //         break;
    //     }
    //     else
    //     {
    //         // continue to the next case, so there is not break here
    //     }
    // }
    // case Phase::NON_CAPTURE:
    // {
    //     if (!moves_.empty())
    //     {
    //         auto bestMoveIter = moves_.begin();
    //         auto iter = moves_.begin();
    //         ++iter;
    //         for (; iter != moves_.end(); ++iter)
    //         {
    //             if (iter->score > bestMoveIter->score)
    //             {
    //                 bestMoveIter = iter;
    //             }
    //         }
    //         selectedMove = bestMoveIter->move;
    //         moves_.erase(bestMoveIter);
    //         break;
    //     }
    //     else
    //     {
    //         phase_ = Phase::END;
    //         // continue to the next case, so there is not break here
    //     }
    // }
    // default:
    //     break;
    // }
    // return selectedMove;
}

bool MovePicker::NoLegalMove()
{
    return move_generator_.check_moves().empty() 
        && move_generator_.capture_moves().empty() 
        && move_generator_.non_capture_moves().empty();
}