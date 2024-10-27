#include "structures.h"

Square Move::MoveFrom() const
{
    return static_cast<Square>((move_ & 0xFF00) >> 8);
}

Square Move::MoveTo() const
{
    return static_cast<Square>(move_ & 0x00FF);
}

std::string Move::StringFormat() const
{
    std::string str;
    Square from = MoveFrom();
    Square to = MoveTo();
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