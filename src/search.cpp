#include <iostream>
#include <algorithm>

#include "search.h"
#include "move_generator.h"
#include "evaluate.h"
#include "transposition_table.h"
#include "history.h"
#include "move_picker.h"

extern TranspositionTable TT;
extern History HISTORY;

void Search::IterativeDeepeningLoop(int maxDepth)
{
    TT.NewSearch();
    HISTORY.Clear();

    root_moves_.clear();
    MoveGenerator moveGenerator(root_position_);
    auto checkMoves = moveGenerator.check_moves();
    root_moves_.insert(root_moves_.end(), std::make_move_iterator(checkMoves.begin()), std::make_move_iterator(checkMoves.end()));
    auto captureMoves = moveGenerator.capture_moves();
    root_moves_.insert(root_moves_.end(), std::make_move_iterator(captureMoves.begin()), std::make_move_iterator(captureMoves.end()));
    auto nonCaptureMoves = moveGenerator.non_capture_moves();
    root_moves_.insert(root_moves_.end(), std::make_move_iterator(nonCaptureMoves.begin()), std::make_move_iterator(nonCaptureMoves.end()));

    for (int depth = 1; depth <= maxDepth; ++depth)
    {
        SearchStack ss[100];
        root_search(depth, ss);
        
        if (ss[0].pv.size() > 0)
        {
            root_moves_.remove(ss[0].pv[0]);
            root_moves_.push_front(ss[0].pv[0]);
        }

        // for (auto move : root_moves_)
        // {
        //     std::cout << move << "; ";
        // }
        // std::cout << std::endl;

        // std::cout << "depth: " << depth << " pv: ";
        // for (auto move: ss[0].pv)
        // {
        //     std::cout << move << "; ";
        // }
        // std::cout << std::endl;
    }
}

void Search::root_search(int depth, SearchStack ss[])
{
    best_move_ = 0;
    best_score_ = -Infinite;
    prohibited_move_ = 0;
    if (depth <= 0) { return; }

    if (root_moves_.empty()) 
    {
        best_score_ = -MateValue;
        return;
    }

    int alpha = -Infinite, beta = Infinite;
    int ply = 0;
    for (auto move: root_moves_)
    {
        UndoInfo undoInfo;
        root_position_.MakeMove(move, undoInfo);
        int score = -search(root_position_, depth - 1, -beta, -alpha, ss, ply + 1);
        root_position_.UndoMove(undoInfo);
        if (score > alpha)
        {
            best_move_ = move;
            best_score_ = score;
            alpha = score;
            ss[ply].current_move = move;
            ss[ply].pv.clear();
            ss[ply].pv.push_back(ss[ply].current_move);
            ss[ply].pv.insert(ss[ply].pv.end(), ss[ply + 1].pv.begin(), ss[ply + 1].pv.end());
        }
    }
}

int Search::search(Position& position, int depth, int alpha, int beta, SearchStack ss[], int ply)
{
    TTEntry* ttEntry = TT[position.key()];
    if (ttEntry != nullptr && ttEntry->depth == depth)
    {
        return ttEntry->value;
    }

    MovePicker movePicker(position, ttEntry != nullptr ? ttEntry->move : Move());
    if (movePicker.NoLegalMove()) 
    {
        auto score = -MateValue + ply;
        TT.Store(position.key(), score, depth, 0);
        // We like choose fastest checkmate in search
        return score;
    }

    if (depth == 0)
    {
        // auto score = Evaluate::Eval(position);
        auto score = qsearch(position, alpha, beta, ss, ply);
        TT.Store(position.key(), score, depth, 0);
        return score;
    }

    Move move;
    while ((move = movePicker.NextMove()))
    {
        UndoInfo undoInfo;
        position.MakeMove(move, undoInfo);
        auto score = -search(position, depth - 1, -beta, -alpha, ss, ply + 1);
        position.UndoMove(undoInfo);
        if (score >= beta)
        {
            TT.Store(position.key(), beta, depth, move);
            HISTORY.Success(position, move, depth);
            return beta;
        }
        else if (score > alpha)
        {
            alpha = score;
            ss[ply].current_move = move;
            ss[ply].pv.clear();
            ss[ply].pv.push_back(ss[ply].current_move);
            ss[ply].pv.insert(ss[ply].pv.end(), ss[ply + 1].pv.begin(), ss[ply + 1].pv.end());
            TT.Store(position.key(), score, depth, move);
        }
    }
    return alpha;
}

int Search::qsearch(Position& position, int alpha, int beta, SearchStack ss[], int ply)
{
    auto score = Evaluate::Eval(position);
    if (score >= beta)
    {
        return beta;
    }
    else if (score > alpha)
    {
        alpha = score;
    }
    MoveGenerator moveGenerator(position);
    auto checkMoves = moveGenerator.check_moves();
    auto captureMoves = moveGenerator.capture_moves();
    auto nonCaptureMoves = moveGenerator.non_capture_moves();
    if (checkMoves.empty() && captureMoves.empty() && nonCaptureMoves.empty())
    {
        return -MateValue + ply;
    }
    for (auto move: captureMoves)
    {
        UndoInfo undoInfo;
        position.MakeMove(move, undoInfo);
        score = -qsearch(position, -beta, -alpha, ss, ply + 1);
        position.UndoMove(undoInfo);
        if (score >= beta)
        {
            return beta;
        }
        else if (score > alpha)
        {
            alpha = score;
        }
    }
    return alpha;
}

void Search::sort_moves(const Position& position, std::list<Move>& moves, Move ttMove)
{
    // allocate too many memroy, will refactor this later
    static int scores[1 << sizeof(ttMove.NumberFormat()) * 8];

    for (auto move: moves)
    {
        scores[move] = HISTORY.HistoryValue(position, move);
        if (move == ttMove)
        {
            scores[move] += History::kHistoryMax;
        }
    }
    moves.sort([](const Move& a, const Move& b) {
        return scores[a] > scores[b];
    });
}