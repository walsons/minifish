#include "move_generator.h"

void MoveGenerator::GeneratePseudoLegalMoves()
{
    Color c = position_.side_to_move();
    for (Square s = SQ_A0; s < SQ_NUM; s += SQ_EAST)
    {
        if (position_.color_from_square(s) != c)
            continue;

        switch (position_.piece_from_square(s))
        {
        case 'R':
            RookMove<'w'>(s);
            break;
        case 'r':
            RookMove<'b'>(s);
            break;
        case 'N':
            KnightMove<'w'>(s);
            break;
        case 'n':
            KnightMove<'b'>(s);
            break;
        case 'B':
            BishopMove<'w'>(s);
            break;
        case 'b':
            BishopMove<'b'>(s);
            break;
        case 'A':
            AdvisorMove<'w'>(s);
            break;
        case 'a':
            AdvisorMove<'b'>(s);
            break;
        case 'K':
            KingMove<'w'>(s);
            break;
        case 'k':
            KingMove<'b'>(s);
            break;
        case 'C':
            CannonMove<'w'>(s);
            break;
        case 'c':
            CannonMove<'b'>(s);
            break;
        case 'P':
            PawnMove<'w'>(s);
            break;
        case 'p':
            PawnMove<'b'>(s);
            break;
        default:
            break;
        }
    }
}

void MoveGenerator::GenerateLegalMoves()
{
    Color c = position_.side_to_move();
    auto kPos = position_.KingSquare(c);
    int kRow = kPos / 9;
    int kCol = kPos % 9;

    GeneratePseudoLegalMoves();

    auto movesFilter = [&](std::vector<Move>& pseudoLegalMoves, std::vector<Move>& legalMoves) {
        if (!position_.IsSelfChecked())
        {
            for (auto pMove : pseudoLegalMoves)
            {
                auto from = MoveFrom(pMove);
                auto to = MoveTo(pMove);
                if (from != kPos)
                {
                    if (from / 9 != kRow && from % 9 != kCol && to / 9 != kRow && to % 9 != kCol && Distance(from, kPos) > 1)
                    {
                        // TODO: optimize this funciton, due to this case will probably has check move
                        legalMoves.push_back(pMove);
                    }
                    else
                    {
                        UndoInfo undoInfo;
                        position_.SimpleMakeMove(pMove, undoInfo);
                        if (!position_.IsSelfChecked())
                        {
                            if (position_.IsEnemyChecked())
                                check_moves_.push_back(pMove);
                            else
                                legalMoves.push_back(pMove);
                        }
                        position_.SimpleUndoMove(undoInfo);
                    }
                }
                else
                {
                    UndoInfo undoInfo;
                    position_.SimpleMakeMove(pMove, undoInfo);
                    if (!position_.IsSelfChecked())
                    {
                        if (position_.IsEnemyChecked())
                            check_moves_.push_back(pMove);
                        else
                            legalMoves.push_back(pMove);
                    }
                    position_.SimpleUndoMove(undoInfo);
                }
            }
        }
        else
        {
            for (auto pMove : pseudoLegalMoves)
            {
                auto from = MoveFrom(pMove);
                auto to = MoveTo(pMove);
                if (from != kPos)
                {
                    if (from / 9 != kRow && from % 9 != kCol && to / 9 != kRow && to % 9 != kCol && Distance(to, kPos) > 2)
                    {
                        continue;
                    }
                    else
                    {
                        UndoInfo undoInfo;
                        position_.SimpleMakeMove(pMove, undoInfo);
                        if (!position_.IsSelfChecked())
                        {
                            if (position_.IsEnemyChecked())
                                check_moves_.push_back(pMove);
                            else
                                legalMoves.push_back(pMove);
                        }
                        position_.SimpleUndoMove(undoInfo);
                    }
                }
                else
                {
                    UndoInfo undoInfo;
                    position_.SimpleMakeMove(pMove, undoInfo);
                    if (!position_.IsSelfChecked())
                    {
                        if (position_.IsEnemyChecked())
                            check_moves_.push_back(pMove);
                        else
                            legalMoves.push_back(pMove);
                    }
                    position_.SimpleUndoMove(undoInfo);
                }
            }
        }
    };
    
    movesFilter(pseudo_legal_capture_moves_, capture_moves_);
    movesFilter(pseudo_legal_non_capture_moves_, non_capture_moves_);
}