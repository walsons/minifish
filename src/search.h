#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include <list>

#include "position.h"

struct SearchStack
{
    std::vector<Move> pv;
    Move current_move;
};

class Search
{
public:
    Search(const Position& position) : root_position_(position) {}
    void SetPosition(const Position& position) { root_position_ = position; }
    void IterativeDeepeningLoop(int maxDepth);

    Move best_move() { return best_move_; }
    int best_score() { return best_score_; }

private:
    void root_search(int depth, SearchStack ss[]);
    int search(Position& position, int depth, int alpha, int beta, SearchStack ss[], int ply);

private:
    Position root_position_;
    Move best_move_;
    int best_score_;
    std::list<Move> root_moves_;
    // Prohibit loop move
    Move prohibited_move_;
};

#endif