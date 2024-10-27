#ifndef POSITION_H
#define POSITION_H

#include <cstring>

#include <string>
#include <stack>

#include "types.h"
#include "function.h"
#include "zobrist.h"

const std::string kStartPos = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";

class Board
{
public:
    Board() : b(new Piece[90]) {}
    ~Board() { delete[] b; }
    Board(const Board& board) : b(new Piece[90]) { std::memcpy(b, board.b, 90 * sizeof(Piece)); }
    Board& operator=(const Board& board)
    {
        // self assignment
        if (board.b == b)
            return *this;
        std::memcpy(b, board.b, 90 * sizeof(Piece));
        return *this;
    }
    Piece& operator[](Square index) {
        return b[index];
    }
    const Piece& operator[](Square index) const {
        return b[index];
    }
private:
    Piece* b;  // board representation
};

class Position
{
public:
    Position(const std::string& fen = kStartPos) { SetPosition(fen); }

    void SetPosition(const std::string& fen);
    void ResetPosition() { SetPosition(kStartPos); }

    std::string GenerateFen();

    void MakeMove(Move move, UndoInfo& undoInfo);
    void UndoMove(const UndoInfo& undoInfo);

    // These two functions is used to for IsChecked()
    void SimpleMakeMove(Move move, UndoInfo& undoInfo);
    void SimpleUndoMove(const UndoInfo& undoInfo);

    void DisplayBoard();

    Square KingSquare(Color c);
    bool IsEnemyChecked();
    bool IsSelfChecked();

    Piece piece_from_square(Square s) const { return board_[s]; }
    Color color_from_square(Square s) const { return IsColor<Color::RED>(board_[s]) ? Color::RED : Color::BLACK; }
    U64 key() const { return key_; }
    Color side_to_move() const { return side_to_move_; }

private:
    bool IsChecked(Color c);
    Piece PieceFromChar(char c) const;
    char CharFromPiece(Piece piece) const;

private:
    Board board_;
    U64 key_;
    Color side_to_move_;
};

#endif