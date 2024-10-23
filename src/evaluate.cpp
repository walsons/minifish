#include "evaluate.h"

int Evaluate::Eval(const Position& position)
{
    int score = 0;
    for (Square s = SQ_A0; s < SQ_NUM; s += SQ_EAST)
    {
        switch (position.piece_from_square(s))
        {
        case 'R':
            score += RookValue;
            break;
        case 'r':
            score += -RookValue;
            break;
        case 'N':
            score += KnightValue;
            break;
        case 'n':
            score += -KnightValue;
            break;
        case 'B':
            score += BishopValue;
            break;
        case 'b':
            score += -BishopValue;
            break;
        case 'A':
            score += AdvisorValue;
            break;
        case 'a':
            score += -AdvisorValue;
            break;
        case 'K':
            score += KingValue;
            break;
        case 'k':
            score += -KingValue;
            break;
        case 'C':
            score += CannonValue;
            break;
        case 'c':
            score += -CannonValue;
            break;
        case 'P':
            score += PawnValue;
            break;
        case 'p':
            score += -PawnValue;
            break;
        }
        if (s >= SQ_A5 && IsColor<'w'>(position.piece_from_square(s)))
            score += 10;
        if (s < SQ_A5 && IsColor<'b'>(position.piece_from_square(s)))
            score += 10;
    }

    return position.side_to_move() == 'w' ? score : -score;
}