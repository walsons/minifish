#ifndef TYPES_H
#define TYPES_H

enum Square : char
{
    SQ_A0, SQ_B0, SQ_C0, SQ_D0, SQ_E0, SQ_F0, SQ_G0, SQ_H0, SQ_I0,
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1, SQ_I1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2, SQ_I2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3, SQ_I3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4, SQ_I4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5, SQ_I5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6, SQ_I6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7, SQ_I7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8, SQ_I8,
    SQ_A9, SQ_B9, SQ_C9, SQ_D9, SQ_E9, SQ_F9, SQ_G9, SQ_H9, SQ_I9,
    SQ_NUM
};

enum Direction : char
{
    SQ_NORTH = 9,
    SQ_SOUTH = -SQ_NORTH,
    SQ_EAST = 1,
    SQ_WEST = -SQ_EAST
};

enum class Piece : char
{
    W_PAWN,
    W_ADVISOR,
    W_BISHOP,
    W_KNIGHT,
    W_CANNON,
    W_ROOK,
    W_KING,

    B_PAWN,
    B_ADVISOR,
    B_BISHOP,
    B_KNIGHT,
    B_CANNON,
    B_ROOK,
    B_KING,

    PIECE_NUM,

    NO_PIECE,

    START = W_PAWN,
    END = B_KING,
    W_START = W_PAWN,
    W_END = W_KING,
    B_START = B_PAWN,
    B_END = B_KING
};

enum class Color : char
{
    RED,
    BLACK
};

// using Piece = char;
// using Move = short;
// using Color = char;
using U64 = unsigned long long;

#endif