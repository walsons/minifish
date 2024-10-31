#include "evaluate.h"

int Evaluate::Eval(const Position& position)
{
    int score = 0;
    for (Square s = SQ_A0; s < SQ_NUM; s += SQ_EAST)
    {
        switch (position.piece_from_square(s))
        {
        case Piece::W_ROOK:
            score += RookValue;
            break;
        case Piece::B_ROOK:
            score += -RookValue;
            break;
        case Piece::W_KNIGHT:
            score += KnightValue;
            break;
        case Piece::B_KNIGHT:
            score += -KnightValue;
            break;
        case Piece::W_BISHOP:
            score += BishopValue;
            break;
        case Piece::B_BISHOP:
            score += -BishopValue;
            break;
        case Piece::W_ADVISOR:
            score += AdvisorValue;
            break;
        case Piece::B_ADVISOR:
            score += -AdvisorValue;
            break;
        case Piece::W_KING:
            score += KingValue;
            break;
        case Piece::B_KING:
            score += -KingValue;
            break;
        case Piece::W_CANNON:
            score += CannonValue;
            break;
        case Piece::B_CANNON:
            score += -CannonValue;
            break;
        case Piece::W_PAWN:
            score += PawnValue;
            break;
        case Piece::B_PAWN:
            score += -PawnValue;
            break;
        default:
            break;
        }
        if (s >= SQ_A5 && IsColor<Color::RED>(position.piece_from_square(s)))
            score += 10;
        if (s < SQ_A5 && IsColor<Color::BLACK>(position.piece_from_square(s)))
            score += 10;
    }

    return position.side_to_move() == Color::RED ? score : -score;
}