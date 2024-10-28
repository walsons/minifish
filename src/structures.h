#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <iostream>
#include "types.h"

class Move
{
public:
    operator short&() { return move_; }
    operator short() const { return move_; }
    Move() : move_(0) {}
    Move(const short move) : move_(move) {}

    Square MoveFrom() const;
    Square MoveTo() const;
    std::string StringFormat() const;
    short NumberFormat() const { return move_; }

    friend std::ostream& operator<<(std::ostream& os, const Move& move)
    {
        return os << move.move_ << "(" << move.StringFormat() << ")";
    }

private:
    short move_;
};

struct UndoInfo
{
    Move move;
    Piece piece;
};

#endif
