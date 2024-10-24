#include <iostream>

#include "search.h"
#include "move_generator.h"
#include "evaluate.h"

void Search::IterativeDeepeningLoop(int maxDepth)
{
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

        // std::cout << "depth: " << depth << " pv: ";
        // for (auto move: ss[0].pv)
        // {
        //     std::cout << Move2String(move) << "; ";
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
        if (score <= alpha)
        {
            continue;
        }
        else
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
    std::list<Move> legalMoves;
    int score = 0;
    MoveGenerator moveGenerator(position);
    auto checkMoves = moveGenerator.check_moves();
    legalMoves.insert(legalMoves.end(), std::make_move_iterator(checkMoves.begin()), std::make_move_iterator(checkMoves.end()));
    auto captureMoves = moveGenerator.capture_moves();
    legalMoves.insert(legalMoves.end(), std::make_move_iterator(captureMoves.begin()), std::make_move_iterator(captureMoves.end()));
    auto nonCaptureMoves = moveGenerator.non_capture_moves();
    legalMoves.insert(legalMoves.end(), std::make_move_iterator(nonCaptureMoves.begin()), std::make_move_iterator(nonCaptureMoves.end()));
    if (legalMoves.empty()) 
    {
        // We like choose fastest checkmate in search
        return -MateValue - depth;
    }

    if (depth == 0)
    {
        return Evaluate::Eval(position);
    }

    for (auto move: legalMoves)
    {
        UndoInfo undoInfo;
        position.MakeMove(move, undoInfo);
        score = -search(position, depth - 1, -beta, -alpha, ss, ply + 1);
        position.UndoMove(undoInfo);
        if (score >= beta)
        {
            // TODO: Update history and counter move
            return score;
        }
        else if (score <= alpha)
        {
            continue;
        }
        else
        {
            alpha = score;
            ss[ply].current_move = move;
            ss[ply].pv.clear();
            ss[ply].pv.push_back(ss[ply].current_move);
            ss[ply].pv.insert(ss[ply].pv.end(), ss[ply + 1].pv.begin(), ss[ply + 1].pv.end());
        }
    }
    return alpha;
}