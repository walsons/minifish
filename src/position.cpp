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
        board_[s] = boardStr[s];
    }
    for (Square s = SQ_A0; s < SQ_NUM; s += SQ_EAST)
    {
        key_ ^= Zobrist::PieceSquareZobrist(PieceTypeMap[board_[s]], s);
    }

    std::string fenSideToMove;
    ss >> fenSideToMove;
    side_to_move_ = (fenSideToMove == "w" ? 'w' : 'b');

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
        if (board_[s] != '0')
            seg.push_back(board_[s]);
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
    fen.push_back(side_to_move_);
    fen += " - - 0 1";
    return fen;
}

void Position::MakeMove(Move move, UndoInfo& undoInfo)
{
    auto from = MoveFrom(move);
    auto to = MoveTo(move);

    key_ ^= Zobrist::PieceSquareZobrist(PieceTypeMap[board_[from]], from);
    key_ ^= Zobrist::PieceSquareZobrist(NO_PIECE, from);
    key_ ^= Zobrist::PieceSquareZobrist(PieceTypeMap[board_[to]], to);
    key_ ^= Zobrist::PieceSquareZobrist(PieceTypeMap[board_[from]], to);

    undoInfo = { move, board_[to] };
    board_[to] = board_[from];
    board_[from] = '0';

    side_to_move_ = (side_to_move_ == 'b' ? 'w' : 'b');
    if (side_to_move_ == 'b')
        key_ ^= Zobrist::BlackToMoveZobrist();
}

void Position::UndoMove(const UndoInfo& undoInfo)
{
    auto from = MoveFrom(undoInfo.move);
    auto to = MoveTo(undoInfo.move);

    board_[from] = board_[to];
    board_[to] = undoInfo.piece;

    key_ ^= Zobrist::PieceSquareZobrist(PieceTypeMap[board_[from]], from);
    key_ ^= Zobrist::PieceSquareZobrist(NO_PIECE, from);
    key_ ^= Zobrist::PieceSquareZobrist(PieceTypeMap[board_[to]], to);
    key_ ^= Zobrist::PieceSquareZobrist(PieceTypeMap[board_[from]], to);

    if (side_to_move_ == 'b')
        key_ ^= Zobrist::BlackToMoveZobrist();
    side_to_move_ = (side_to_move_ == 'b' ? 'w' : 'b');
}

void Position::SimpleMakeMove(Move move, UndoInfo& undoInfo)
{
    auto from = MoveFrom(move);
    auto to = MoveTo(move);

    undoInfo = { move, board_[to] };
    board_[to] = board_[from];
    board_[from] = '0';
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
        segStr.push_back(board_[s]);
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
    
    if (c == 'w')
    {
        // Short-circuit when found
        f(SQ_D0, SQ_F0, 'K') || f(SQ_D1, SQ_F1, 'K') || f(SQ_D2, SQ_F2, 'K');
    }
    else
    {
        f(SQ_D7, SQ_F7, 'k') || f(SQ_D8, SQ_F8, 'k') || f(SQ_D9, SQ_F9, 'k');
    }
    
    return kPos;
}

bool Position::IsEnemyChecked()
{
    return IsChecked(side_to_move_ == 'w' ? 'b' : 'w');
}

bool Position::IsSelfChecked()
{
    return IsChecked(side_to_move_);
}

bool Position::IsChecked(Color c)
{
    Square kPos = KingSquare(c);

    /***** Pawn *****/
    if (c == 'w')
    {
        if (board_[kPos + SQ_NORTH] == 'p' || board_[kPos + SQ_EAST] == 'p' || board_[kPos + SQ_WEST] == 'p')
            return true;
    }
    else
    {
        if (board_[kPos + SQ_SOUTH] == 'P' || board_[kPos + SQ_EAST] == 'P' || board_[kPos + SQ_WEST] == 'P')
            return true;
    }

    /***** Rook, Cannon, King *****/
    auto f1 = [&](Direction d, decltype(IsNotNorthEnd) f) {
        int flag = 0;  // How many pieces in the middle
        for (Square pos = kPos + d; f(pos, kPos); pos += d)
        {
            if (board_[pos] != '0')
            {
                if (flag == 0)
                {
                    // Rook
                    if ((c == 'w' && board_[pos] == 'r') || (c == 'b' && board_[pos] == 'R'))
                        return true;
                    // King
                    if ((c == 'w' && board_[pos] == 'k') || (c == 'b' && board_[pos] == 'K'))
                        return true;
                }
                else if (flag == 1)
                {
                    // Cannon
                    if ((c == 'w' && board_[pos] == 'c') || (c == 'b' && board_[pos] == 'C'))
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
        if (SQ_A0 <= barrier && barrier < SQ_NUM && Distance(kPos, barrier) == 1 && board_[barrier] == '0')
        {
            if (SQ_A0 <= destination1 && destination1 < SQ_NUM && Distance(kPos, destination1) == 2 && 
                ((side_to_move() == 'w' && board_[destination1] == 'n') || (side_to_move() == 'b' && board_[destination1] == 'N')))
            {
                return true;
            }
            if (SQ_A0 <= destination2 && destination2 < SQ_NUM && Distance(kPos, destination2) == 2 && 
                ((side_to_move() == 'w' && board_[destination2] == 'n') || (side_to_move() == 'b' && board_[destination2] == 'N')))
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

