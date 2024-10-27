#include "position.h"

#include <iostream>
#include <sstream>

void Position::SetPosition(const std::string& fen)
{
    std::istringstream ss(fen);

    std::string fenPos;
    ss >> fenPos;
    std::string boardStr;
    std::string segStr;
    for (auto c : fenPos)
    {
        if (c == ' ')
            break;
        else if (c > '0' && c <= '9')
            segStr += std::string((c - '0'), '0');
        else if (c == '/')
        {
            boardStr = segStr + boardStr;
            segStr.clear();
            continue;
        }
        else
            segStr += c;
    }
    boardStr = segStr + boardStr;
    for (Square s = SQ_A0; s < SQ_NUM; s += SQ_EAST)
    {
        board_[s] = PieceFromChar(boardStr[s]);
    }
    for (Square s = SQ_A0; s < SQ_NUM; s += SQ_EAST)
    {
        key_ ^= Zobrist::PieceSquareZobrist(board_[s], s);
    }

    std::string fenSideToMove;
    ss >> fenSideToMove;
    side_to_move_ = (fenSideToMove == "w" ? Color::RED : Color::BLACK);
    if (side_to_move_ == Color::BLACK)
        key_ ^= Zobrist::BlackToMoveZobrist();

    // - - 0 1
    std::string temp;
    for (int i = 0; i < 4; ++i)
    {
        ss >> temp;
    }

    // moves xxxx xxxx ...
    std::string move;
    if (ss >> move)
    {
        while (ss >> move)
        {
            UndoInfo undoInfo;
            MakeMove(String2Move(move), undoInfo);
        }
    }
};

std::string Position::GenerateFen()
{
    std::string fen;
    std::string seg;
    for (Square s = SQ_A0; s < SQ_NUM; s += SQ_EAST)
    {
        if (s != SQ_A0 && s % 9 == 0)
        {
            fen = ("/" + seg) + fen;
            seg.clear();
        }
        if (board_[s] != Piece::NO_PIECE)
            seg.push_back(CharFromPiece(board_[s]));
        else
        {
            if (seg.back() > '0' && seg.back() <= '9')
                seg.back() += 1;
            else
                seg.push_back('1');
        }
    }
    fen = seg + fen;
    fen.push_back(' ');
    fen.push_back(side_to_move_ == Color::RED ? 'w' : 'b');
    fen += " - - 0 1";
    return fen;
}

void Position::MakeMove(Move move, UndoInfo& undoInfo)
{
    auto from = MoveFrom(move);
    auto to = MoveTo(move);

    key_ ^= Zobrist::PieceSquareZobrist(board_[from], from);
    key_ ^= Zobrist::PieceSquareZobrist(Piece::NO_PIECE, from);
    key_ ^= Zobrist::PieceSquareZobrist(board_[to], to);
    key_ ^= Zobrist::PieceSquareZobrist(board_[from], to);

    undoInfo = { move, board_[to] };
    board_[to] = board_[from];
    board_[from] = Piece::NO_PIECE;

    side_to_move_ = (side_to_move_ == Color::BLACK ? Color::RED : Color::BLACK);
    key_ ^= Zobrist::BlackToMoveZobrist();
}

void Position::UndoMove(const UndoInfo& undoInfo)
{
    auto from = MoveFrom(undoInfo.move);
    auto to = MoveTo(undoInfo.move);

    board_[from] = board_[to];
    board_[to] = undoInfo.piece;

    key_ ^= Zobrist::PieceSquareZobrist(board_[from], from);
    key_ ^= Zobrist::PieceSquareZobrist(Piece::NO_PIECE, from);
    key_ ^= Zobrist::PieceSquareZobrist(board_[to], to);
    key_ ^= Zobrist::PieceSquareZobrist(board_[from], to);

    key_ ^= Zobrist::BlackToMoveZobrist();
    side_to_move_ = (side_to_move_ == Color::BLACK ? Color::RED : Color::BLACK);
}

void Position::SimpleMakeMove(Move move, UndoInfo& undoInfo)
{
    auto from = MoveFrom(move);
    auto to = MoveTo(move);

    undoInfo = { move, board_[to] };
    board_[to] = board_[from];
    board_[from] = Piece::NO_PIECE;
}

void Position::SimpleUndoMove(const UndoInfo& undoInfo)
{
    auto from = MoveFrom(undoInfo.move);
    auto to = MoveTo(undoInfo.move);

    board_[from] = board_[to];
    board_[to] = undoInfo.piece;
}

void Position::DisplayBoard()
{
    std::string boardStr;
    std::string segStr;

    for (Square s = SQ_A0; s < SQ_NUM; s += SQ_EAST)
    {
        if (s % 9 == 0)
        {
            boardStr = segStr + boardStr;
            segStr.clear();
        }
        segStr.push_back(CharFromPiece(board_[s]));
    }
    boardStr = segStr + boardStr;

    std::cout << "  +---+---+---+---+---+---+---+---+---+" << std::endl;
    for (int i = 0; i < 10; ++i)
    {
        std::cout << (9 - i) << " |";
        for (int j = 0; j < 9; ++j)
        {
            std::cout << " " << (boardStr[i * 9 + j] == '0' ? ' ' : boardStr[i * 9 + j]) << " " << "|";
        }
        std::cout << std::endl;
        std::cout << "  +---+---+---+---+---+---+---+---+---+" << std::endl;
    }
    std::cout << "  ";
    for (int j = 0; j < 9; ++j)
    {
        std::cout << "  " << static_cast<char>('a' + j) << " ";
    }
    std::cout << std::endl;
}

Square Position::KingSquare(Color c)
{
    Square kPos = SQ_NUM;
    auto f = [&](Square beg, Square end, Piece k) {
        for (Square s = beg; s <= end; s += SQ_EAST)
        {
            if (board_[s] == k)
            {
                kPos = s;
                return true;
            }
        }
        return false;
    };
    
    if (c == Color::RED)
    {
        // Short-circuit when found
        f(SQ_D0, SQ_F0, Piece::W_KING) || f(SQ_D1, SQ_F1, Piece::W_KING) || f(SQ_D2, SQ_F2, Piece::W_KING);
    }
    else
    {
        f(SQ_D7, SQ_F7, Piece::B_KING) || f(SQ_D8, SQ_F8, Piece::B_KING) || f(SQ_D9, SQ_F9, Piece::B_KING);
    }
    
    return kPos;
}

bool Position::IsEnemyChecked()
{
    return IsChecked(side_to_move_ == Color::RED ? Color::BLACK : Color::RED);
}

bool Position::IsSelfChecked()
{
    return IsChecked(side_to_move_);
}

bool Position::IsChecked(Color c)
{
    Square kPos = KingSquare(c);

    /***** Pawn *****/
    if (c == Color::RED)
    {
        if (board_[kPos + SQ_NORTH] == Piece::B_PAWN || board_[kPos + SQ_EAST] == Piece::B_PAWN || board_[kPos + SQ_WEST] == Piece::B_PAWN)
            return true;
    }
    else
    {
        if (board_[kPos + SQ_SOUTH] == Piece::W_PAWN || board_[kPos + SQ_EAST] == Piece::W_PAWN || board_[kPos + SQ_WEST] == Piece::W_PAWN)
            return true;
    }

    /***** Rook, Cannon, King *****/
    auto f1 = [&](Direction d, decltype(IsNotNorthEnd) f) {
        int flag = 0;  // How many pieces in the middle
        for (Square pos = kPos + d; f(pos, kPos); pos += d)
        {
            if (board_[pos] != Piece::NO_PIECE)
            {
                if (flag == 0)
                {
                    // Rook
                    if ((c == Color::RED && board_[pos] == Piece::B_ROOK) || (c == Color::BLACK && board_[pos] == Piece::W_ROOK))
                        return true;
                    // King
                    if ((c == Color::RED && board_[pos] == Piece::B_KING) || (c == Color::BLACK && board_[pos] == Piece::W_KING))
                        return true;
                }
                else if (flag == 1)
                {
                    // Cannon
                    if ((c == Color::RED && board_[pos] == Piece::B_CANNON) || (c == Color::BLACK && board_[pos] == Piece::W_CANNON))
                        return true;
                }
                ++flag;
            }
        }
        return false;
    };
    if (f1(SQ_NORTH, IsNotNorthEnd)) { return true; }
    if (f1(SQ_SOUTH, IsNotSouthEnd)) { return true; }
    if (f1(SQ_EAST, IsNotEastEnd)) { return true; }
    if (f1(SQ_WEST, IsNotWestEnd)) { return true; }

    /***** Knight *****/
    auto f2 = [&](Square barrier, Square destination1, Square destination2) {
        if (SQ_A0 <= barrier && barrier < SQ_NUM && Distance(kPos, barrier) == 1 && board_[barrier] == Piece::NO_PIECE)
        {
            if (SQ_A0 <= destination1 && destination1 < SQ_NUM && Distance(kPos, destination1) == 2 && 
                ((side_to_move() == Color::RED && board_[destination1] == Piece::B_KNIGHT) || (side_to_move() == Color::BLACK && board_[destination1] == Piece::W_KNIGHT)))
            {
                return true;
            }
            if (SQ_A0 <= destination2 && destination2 < SQ_NUM && Distance(kPos, destination2) == 2 && 
                ((side_to_move() == Color::RED && board_[destination2] == Piece::B_KNIGHT) || (side_to_move() == Color::BLACK && board_[destination2] == Piece::W_KNIGHT)))
            {
                return true;
            }
        }
        return false;
    };
    // North East
    Square barrier = kPos + SQ_NORTH + SQ_EAST;
    Square destination1 = kPos + static_cast<Square>(2) * SQ_NORTH + SQ_EAST;
    Square destination2 = kPos + static_cast<Square>(2) * SQ_EAST + SQ_NORTH;
    if (f2(barrier, destination1, destination2)) { return true; }
    // South East
    barrier = kPos + SQ_SOUTH + SQ_EAST;
    destination1 = kPos + static_cast<Square>(2) * SQ_SOUTH + SQ_EAST;
    destination2 = kPos + static_cast<Square>(2) * SQ_EAST + SQ_SOUTH;
    if (f2(barrier, destination1, destination2)) { return true; }
    // North West
    barrier = kPos + SQ_NORTH + SQ_WEST;
    destination1 = kPos + static_cast<Square>(2) * SQ_NORTH + SQ_WEST;
    destination2 = kPos + static_cast<Square>(2) * SQ_WEST + SQ_NORTH;
    if (f2(barrier, destination1, destination2)) { return true; }
    // South West
    barrier = kPos + SQ_SOUTH + SQ_WEST;
    destination1 = kPos + static_cast<Square>(2) * SQ_SOUTH + SQ_WEST;
    destination2 = kPos + static_cast<Square>(2) * SQ_WEST + SQ_SOUTH;
    if (f2(barrier, destination1, destination2)) { return true; }

    return false;
}

Piece Position::PieceFromChar(char c) const
{
    switch (c)
    {
    case 'R':
        return Piece::W_ROOK;
    case 'r':
        return Piece::B_ROOK;
    case 'N':
        return Piece::W_KNIGHT;
    case 'n':
        return Piece::B_KNIGHT;
    case 'B':
        return Piece::W_BISHOP;
    case 'b':
        return Piece::B_BISHOP;
    case 'A':
        return Piece::W_ADVISOR;
    case 'a':
        return Piece::B_ADVISOR;
    case 'K':
        return Piece::W_KING;
    case 'k':
        return Piece::B_KING;
    case 'C':
        return Piece::W_CANNON;
    case 'c':
        return Piece::B_CANNON;
    case 'P':
        return Piece::W_PAWN;
    case 'p':
        return Piece::B_PAWN;
    }
    return Piece::NO_PIECE;
}

char Position::CharFromPiece(Piece piece) const
{
    switch (piece)
    {
    case Piece::W_ROOK:
        return 'R';
    case Piece::B_ROOK:
        return 'r';
    case Piece::W_KNIGHT:
        return 'N';
    case Piece::B_KNIGHT:
        return 'n';
    case Piece::W_BISHOP:
        return 'B';
    case Piece::B_BISHOP:
        return 'b';
    case Piece::W_ADVISOR:
        return 'A';
    case Piece::B_ADVISOR:
        return 'a';
    case Piece::W_KING:
        return 'K';
    case Piece::B_KING:
        return 'k';
    case Piece::W_CANNON:
        return 'C';
    case Piece::B_CANNON:
        return 'c';
    case Piece::W_PAWN:
        return 'P';
    case Piece::B_PAWN:
        return 'p';
    }
    return '0';
}