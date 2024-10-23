#include <iostream>
#include <sstream>
#include "position.h"
#include "search.h"

void Engine()
{
    std::cout << "***** minifish *****" << std::endl;

    double duration;
    std::string line;
    while (true)
    {
        bool flag = true;
        std::getline(std::cin, line);
        std::stringstream ss(line);
        std::string token;
        while (ss >> token)
        {
            Position position;
            if (token == "fen")
            {
                // TODO: startpos and append moves
                std::string fen;
                ss >> fen;
                auto pos = line.find(fen);
                fen = line.substr(pos);

                position.SetPosition(fen);

                flag = false;
            }
            else if (token == "setoption" || token == "ucinewgame")
            {
                flag = false;
            }
            else if (token == "isready")
            {
                std::cout << "readyok" << std::endl;
                flag = false;
            }
            else if (token == "depth")
            {
                std::string depth;
                ss >> depth;
                depth = "4"; // currently we fix depth to 4

                Search search(position);

                clock_t beg, end;
                beg = clock();
                search.IterativeDeepeningLoop(std::stoi(depth));
                end = clock();
                duration = (double)(end - beg) / CLOCKS_PER_SEC;

                std::cout << "time cost:" << duration << std::endl;
                std::cout << "bestmove:" << Move2String(search.best_move()) << ", score:" << search.best_score() << std::endl;
            }
        }
        if (flag)
            std::cout << "**********" << std::endl;
    }
}

int main()
{
    Engine();
}

/*
 * TODO:
 * 1. transposition table
 * 2. counter move and history 
*/ 
