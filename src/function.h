#ifndef FUNCTION_H
#define FUNCTION_H

#include "structures.h"

inline Square operator+(Square sq, Direction d)
{
    return static_cast<Square>(static_cast<char>(sq) + static_cast<char>(d));
}

inline Square& operator+=(Square& sq, Direction d)
{
    sq = static_cast<Square>(static_cast<char>(sq) + static_cast<char>(d));
    return sq;
}


inline Square NorthEnd(Square s) { return SQ_NUM; }
inline Square SouthEnd(Square s) { return static_cast<Square>(SQ_A0 - 1); }
inline Square EastEnd(Square s) { return static_cast<Square>(static_cast<char>(s) / 9 * 9 + 9); }
inline Square WestEnd(Square s) { return static_cast<Square>(static_cast<char>(s) / 9 * 9 - 1); }

inline bool IsNotNorthEnd(Square current, Square start) { return current < NorthEnd(start); }
inline bool IsNotSouthEnd(Square current, Square start) { return current > SouthEnd(start); }
inline bool IsNotEastEnd(Square current, Square start) { return current < EastEnd(start); }
inline bool IsNotWestEnd(Square current, Square start) { return current > WestEnd(start); }

// Used to detemine relative position for Knight and Bishop
inline char Distance(Square s1, Square s2) 
{
    int v = std::abs(s1 / 9 - s2 / 9);
    int h = std::abs(s1 % 9 - s2 % 9);
    return std::max(v, h);
}

inline Move ConstructMove(Square from, Square to)
{
    return ((0x00FF & from) << 8) | to;
}

inline Direction operator*(int x, Direction d)
{
    return static_cast<Direction>(x * static_cast<char>(d));
}

inline Square MoveFrom(Move move)
{
    return static_cast<Square>((move & 0xFF00) >> 8);
}

inline Square MoveTo(Move move)
{
    return static_cast<Square>(move & 0x00FF);
}

inline constexpr Color operator!(Color c)
{
    return c == Color::RED ? Color::BLACK : Color::RED;
}

inline bool IsEmpty(Piece piece) { return piece == '0'; }

template <Color c>
bool IsColor(Piece piece);
template <>
inline bool IsColor<Color::RED>(Piece piece) { return piece >= 'A' && piece <= 'Z'; }
template <>
inline bool IsColor<Color::BLACK>(Piece piece) { return piece >= 'a' && piece <= 'z'; }

template <Color c>
bool IsInPalace(Square s);
template <>
inline bool IsInPalace<Color::RED>(Square s)
{
    return (SQ_D0 <= s && s <= SQ_F0) || (SQ_D1 <= s && s <= SQ_F1) || (SQ_D2 <= s && s <= SQ_F2);
}
template <>
inline bool IsInPalace<Color::BLACK>(Square s)
{
    return (SQ_D9 <= s && s <= SQ_F9) || (SQ_D8 <= s && s <= SQ_F8) || (SQ_D7 <= s && s <= SQ_F7);
}

inline std::string Move2String(Move move)
{
    std::string str;
    Square from = MoveFrom(move);
    Square to = MoveTo(move);
    char s1 = static_cast<char>(from % 9 + 'a');
    int i1 = from / 9;
    char s2 = static_cast<char>(to % 9 + 'a');
    int i2 = to / 9;
    str += std::string(1, s1);
    str += std::to_string(i1);
    str += std::string(1, s2);
    str += std::to_string(i2);
    return str;
}

inline Move String2Move(std::string str)
{
    Square from = static_cast<Square>((str[0] - 'a') + (str[1] - '0') * 9);
    Square to = static_cast<Square>((str[2] - 'a') + (str[3] - '0') * 9);
    return ConstructMove(from, to);
}

#endif