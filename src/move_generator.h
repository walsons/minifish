#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "position.h"
#include "function.h"

class MoveGenerator
{
public:
    MoveGenerator(Position& position) : position_(position)
    {
        // rook:17 cannon:17 knight:8 bishop:4 advisor:4 king:4 pawn:3
        // 17 + 17 + 8 + 4 + 4 + 4 + 3 = 57
        pseudo_legal_capture_moves_.reserve(57);
        pseudo_legal_non_capture_moves_.reserve(57);

        capture_moves_.reserve(57);
        non_capture_moves_.reserve(57);
        check_moves_.reserve(57);

        GenerateLegalMoves();
    }
    
    std::vector<Move> capture_moves() { return capture_moves_; }
    std::vector<Move> non_capture_moves() { return non_capture_moves_; }
    std::vector<Move> check_moves() { return check_moves_; }

private:
    template <Color c>
    void RookMove(Square s)
    {
        for (Square pos = s + SQ_NORTH; pos < NorthEnd(s); pos += SQ_NORTH)
        {
            if (IsColor<c>(position_.piece_from_square(pos)))
                break;
            pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, pos));
            if (!IsEmpty(position_.piece_from_square(pos)))
            {
                pseudo_legal_non_capture_moves_.pop_back();
                pseudo_legal_capture_moves_.push_back(ConstructMove(s, pos));
                break;
            }
        }
        for (Square pos = s + SQ_SOUTH; pos > SouthEnd(s); pos += SQ_SOUTH)
        {
            if (IsColor<c>(position_.piece_from_square(pos)))
                break;
            pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, pos));
            if (!IsEmpty(position_.piece_from_square(pos)))
            {
                pseudo_legal_non_capture_moves_.pop_back();
                pseudo_legal_capture_moves_.push_back(ConstructMove(s, pos));
                break;
            }
        }
        for (Square pos = s + SQ_EAST; pos < EastEnd(s); pos += SQ_EAST)
        {
            if (IsColor<c>(position_.piece_from_square(pos)))
                break;
            pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, pos));
            if (!IsEmpty(position_.piece_from_square(pos)))
            {
                pseudo_legal_non_capture_moves_.pop_back();
                pseudo_legal_capture_moves_.push_back(ConstructMove(s, pos));
                break;
            }
        }
        for (Square pos = s + SQ_WEST; pos > WestEnd(s); pos += SQ_WEST)
        {
            if (IsColor<c>(position_.piece_from_square(pos)))
                break;
            pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, pos));
            if (!IsEmpty(position_.piece_from_square(pos)))
            {
                pseudo_legal_non_capture_moves_.pop_back();
                pseudo_legal_capture_moves_.push_back(ConstructMove(s, pos));
                break;
            }
        }
    }

    template <Color c>
    void CannonMove(Square s)
    {
        auto f = [&](Direction d, decltype(IsNotNorthEnd) f) {
            bool flag = false;
            for (Square pos = s + d; f(pos, s); pos += d)
            {
                if (position_.piece_from_square(pos) != Piece::NO_PIECE)
                {
                    if (flag)
                    {
                        if (IsColor<!c>(position_.piece_from_square(pos)))
                            pseudo_legal_capture_moves_.push_back(ConstructMove(s, pos));
                        break;
                    }
                    else
                    {
                        flag = true;
                        continue;
                    }
                }
                else
                {
                    if (flag)
                    {
                        continue;
                    }
                    else
                    {
                        pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, pos));
                    }
                }
            }
        };
        f(SQ_NORTH, IsNotNorthEnd);
        f(SQ_SOUTH, IsNotSouthEnd);
        f(SQ_EAST, IsNotEastEnd);
        f(SQ_WEST, IsNotWestEnd);
    }

    template <Color c>
    void KnightMove(Square s)
    {
        auto f = [&](Square barrier, Square destination1, Square destination2) {
            if (SQ_A0 <= barrier && barrier < SQ_NUM && Distance(s, barrier) == 1 && position_.piece_from_square(barrier) == Piece::NO_PIECE)
            {
                if (SQ_A0 <= destination1 && destination1 < SQ_NUM && Distance(s, destination1) == 2)
                {
                    if (IsEmpty(position_.piece_from_square(destination1)))
                        pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, destination1));
                    else if (IsColor<!c>(position_.piece_from_square(destination1)))
                        pseudo_legal_capture_moves_.push_back(ConstructMove(s, destination1));
                }
                if (SQ_A0 <= destination2 && destination2 < SQ_NUM && Distance(s, destination2) == 2)
                {
                    if (IsEmpty(position_.piece_from_square(destination1)))
                        pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, destination2));
                    else if (IsColor<!c>(position_.piece_from_square(destination1)))
                        pseudo_legal_capture_moves_.push_back(ConstructMove(s, destination2));
                }
            }
        };
        // North
        static constexpr Direction N2E1 = static_cast<Direction>(SQ_NORTH + SQ_NORTH + SQ_EAST);
        static constexpr Direction N2W1 = static_cast<Direction>(SQ_NORTH + SQ_NORTH + SQ_WEST);
        f(s + SQ_NORTH, s + N2E1, s + N2W1);
        // South
        static constexpr Direction S2E1 = static_cast<Direction>(SQ_SOUTH + SQ_SOUTH + SQ_EAST);
        static constexpr Direction S2W1 = static_cast<Direction>(SQ_SOUTH + SQ_SOUTH + SQ_WEST);
        f(s + SQ_SOUTH, s + S2E1, s + S2W1);
        // East
        static constexpr Direction E2N1 = static_cast<Direction>(SQ_EAST + SQ_EAST + SQ_NORTH);
        static constexpr Direction E2S1 = static_cast<Direction>(SQ_EAST + SQ_EAST + SQ_SOUTH);
        f(s + SQ_EAST, s + E2N1, s + E2S1);
        // West
        static constexpr Direction W2N1 = static_cast<Direction>(SQ_WEST + SQ_WEST + SQ_NORTH);
        static constexpr Direction W2S1 = static_cast<Direction>(SQ_WEST + SQ_WEST + SQ_SOUTH);
        f(s + SQ_WEST, s + W2N1, s + W2S1);
    }

    template <Color c>
    void BishopMove(Square s)
    {
        auto f = [&](Square barrier, Square destination) {
            static constexpr Square SQ_BEG = (c == Color::RED ? SQ_A0 : SQ_A5);
            static constexpr Square SQ_END = (c == Color::RED ? SQ_A5 : SQ_NUM);
            if (SQ_BEG <= barrier && barrier < SQ_END && Distance(s, barrier) == 1 && position_.piece_from_square(barrier) == Piece::NO_PIECE)
            {
                if (SQ_BEG <= destination && destination < SQ_END && Distance(s, destination) == 2)
                {
                    if (IsEmpty(position_.piece_from_square(destination)))
                        pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, destination));
                    else if (IsColor<!c>(position_.piece_from_square(destination)))
                        pseudo_legal_capture_moves_.push_back(ConstructMove(s, destination));
                }
            }
        };
        // North East
        static constexpr Direction N1E1 = static_cast<Direction>(SQ_NORTH + SQ_EAST);
        static constexpr Direction N2E2 = static_cast<Direction>(SQ_NORTH + SQ_NORTH + SQ_EAST + SQ_EAST);
        f(s + N1E1, s + N2E2);
        // North West
        static constexpr Direction N1W1 = static_cast<Direction>(SQ_NORTH + SQ_WEST);
        static constexpr Direction N2W2 = static_cast<Direction>(SQ_NORTH + SQ_NORTH + SQ_WEST + SQ_WEST);
        f(s + N1W1, s + N2W2);
        // South East
        static constexpr Direction S1E1 = static_cast<Direction>(SQ_SOUTH + SQ_EAST);
        static constexpr Direction S2E2 = static_cast<Direction>(SQ_SOUTH + SQ_SOUTH + SQ_EAST + SQ_EAST);
        f(s + S1E1, s + S2E2);
        // South West
        static constexpr Direction S1W1 = static_cast<Direction>(SQ_SOUTH + SQ_WEST);
        static constexpr Direction S2W2 = static_cast<Direction>(SQ_SOUTH + SQ_SOUTH + SQ_WEST + SQ_WEST);
        f(s + S1W1, s + S2W2);
    }

    template <Color c>
    void AdvisorMove(Square s)
    {
        auto f = [&](Square destination) {
            if (IsEmpty(position_.piece_from_square(destination)))
                pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, destination));
            else if (IsColor<!c>(position_.piece_from_square(destination)))
                pseudo_legal_capture_moves_.push_back(ConstructMove(s, destination));
        };

        switch (s)
        {
        // White
        case SQ_D0:
        case SQ_F0:
        case SQ_D2:
        case SQ_F2:
        {
            f(SQ_E1);
        }
        break;
        case SQ_E1:
        {
            f(SQ_D0);
            f(SQ_F0);
            f(SQ_D2);
            f(SQ_F2);
        }
        break;
        // Black
        case SQ_D9:
        case SQ_F9:
        case SQ_D7:
        case SQ_F7:
        {
            f(SQ_E8);
        }
        break;
        case SQ_E8:
        {
            f(SQ_D9);
            f(SQ_F9);
            f(SQ_D7);
            f(SQ_F7);
        }
        break;
        default:
            break;
        }
    }

    template <Color c>
    void KingMove(Square s)
    {
        auto f = [&](Square destination) {
            if (IsInPalace<c>(destination))
            {
                if (IsEmpty(position_.piece_from_square(destination)))
                    pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, destination));
                else if (IsColor<!c>(position_.piece_from_square(destination)))
                    pseudo_legal_capture_moves_.push_back(ConstructMove(s, destination));
            }
        };
        // North
        f(s + SQ_NORTH);
        // South
        f(s + SQ_SOUTH);
        // East
        f(s + SQ_EAST);
        // West
        f(s + SQ_WEST);
        // Face enemy King
        static constexpr Direction d = (c == Color::RED ? SQ_NORTH : SQ_SOUTH);
        static constexpr auto e = (c == Color::RED ? &NorthEnd : &SouthEnd);
        static constexpr Piece k = (c == Color::RED ? Piece::B_KING : Piece::W_KING);
        for (Square pos = s + d; s < e(pos); s += d)
        {
            if (position_.piece_from_square(pos) == Piece::NO_PIECE)
                continue;
            else if (position_.piece_from_square(pos) == k)
                pseudo_legal_capture_moves_.push_back(ConstructMove(s, pos));
            else
                break;
        }
    }

    template <Color c>
    void PawnMove(Square s)
    {
        // The range across the river
        static constexpr Square SQ_BEG = (c == Color::RED ? SQ_A5 : SQ_A0);
        static constexpr Square SQ_END = (c == Color::RED ? SQ_NUM : SQ_A5);
        // The forward direction
        static constexpr Direction SQ_FORWARD = (c == Color::RED ? SQ_NORTH : SQ_SOUTH);

        if (IsEmpty(position_.piece_from_square(s + SQ_FORWARD)))
            pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, s + SQ_FORWARD));
        else if (IsColor<!c>(position_.piece_from_square(s + SQ_FORWARD)))
            pseudo_legal_capture_moves_.push_back(ConstructMove(s, s + SQ_FORWARD));

        if (SQ_BEG <= s && s < SQ_END)
        {
            if (IsEmpty(position_.piece_from_square(s + SQ_EAST)))
                pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, s + SQ_EAST));
            else if (IsColor<!c>(position_.piece_from_square(s + SQ_EAST)))
                pseudo_legal_capture_moves_.push_back(ConstructMove(s, s + SQ_EAST));

            if (IsEmpty(position_.piece_from_square(s + SQ_WEST)))
                pseudo_legal_non_capture_moves_.push_back(ConstructMove(s, s + SQ_WEST));
            else if (IsColor<!c>(position_.piece_from_square(s + SQ_WEST)))
                pseudo_legal_capture_moves_.push_back(ConstructMove(s, s + SQ_WEST));
        }
    }

    // Moves that don't consider being checked
    void GeneratePseudoLegalMoves();
    // Available moves
    void GenerateLegalMoves();

private:
    Position position_;
    
    std::vector<Move> pseudo_legal_capture_moves_;
    std::vector<Move> pseudo_legal_non_capture_moves_;

    std::vector<Move> capture_moves_;
    std::vector<Move> non_capture_moves_;
    // Check moves are from both capture moves and non_capture moves
    std::vector<Move> check_moves_;
};

#endif