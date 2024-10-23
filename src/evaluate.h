#ifndef EVALUATE_H
#define EVALUATE_H

#include "position.h"

constexpr int Infinite = 60000;
constexpr int MateValue = 50000;
constexpr int RookValue = 1500;
constexpr int KnightValue = 750;
constexpr int BishopValue = 150;
constexpr int AdvisorValue = 150;
constexpr int KingValue = MateValue;
constexpr int CannonValue = 710;
constexpr int PawnValue = 100;

// pikafish piece value
//constexpr int RookValue = 1213;
//constexpr int KnightValue = 964;
//constexpr int BishopValue = 191;
//constexpr int AdvisorValue = 216;
//constexpr int KingValue = MAXS;
//constexpr int CannonValue = 746;
//constexpr int PawnValue = 140;

class Evaluate
{
public:
    static int Eval(const Position& position);
};

#endif