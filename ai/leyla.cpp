#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "leyla.h"

using namespace std;

#define         OPPONENT_OF(aPlayer) ((aPlayer) == Gameplay::Player_White ? Gameplay::Player_Black : Gameplay::Player_White)
#define         DISC_OF(aPlayer)     ((aPlayer) == Gameplay::Player_White ? Board::Disc_White : Board::Disc_Black)

Leyla::Leyla(size_t depth, Gameplay::Player aiPlayer)
    :m_depth(depth),
     m_aiPlayer(aiPlayer)
{
    srand((unsigned)time(NULL));
}

Board::Coordinates Leyla::play(const Gameplay &state) const
{
    vector<Board::Coordinates> allMoves = state.allValidMoves();
    vector<int> allOutcomes;

    int i = 0;

    for (auto move : allMoves)
    {
        Gameplay nextState(state);
        nextState.play(move);

        allOutcomes.push_back(evaluate(nextState, m_depth));
    }

    auto best_outcome = max_element(allOutcomes.begin(), allOutcomes.end());
    if (*best_outcome == numeric_limits<int>::max())
    {
        cerr <<"You are defeated in a few moves. " <<endl;
    }
    else if (*best_outcome == numeric_limits<int>::min())
    {
        cerr << "I am defeated in a few moves. " << endl;
    }
    else
    {
        cerr << "Evaluation of this move is: " << *best_outcome << endl;
    }

    return allMoves[distance(allOutcomes.begin(), best_outcome)];
}

int Leyla::evaluate(const Gameplay &state, size_t depth)const
{
    if (depth == 0)
    {
        return valueOf(state);
    }
    else
    {
        if (!state.hasValidMoves())
        {
            return valueOf(state);
        }

        vector<Board::Coordinates> allMoves = state.allValidMoves();
        vector<int> allOutcomes;

#pragma omp parallel for
        for (int i = 0; i < allMoves.size(); ++i)
        {
            Gameplay nextState(state);
            nextState.play(allMoves[i]);

            int evaluation = evaluate(nextState, depth - 1);
#pragma omp critical
            allOutcomes.push_back(evaluation);
        }

        //if it is currently AI turn, pick the next move with largest outcome.
        if (state.currentPlayer() == m_aiPlayer)
        {
            return *max_element(allOutcomes.begin(), allOutcomes.end());
        }

        //if it is currently opponents turn, pick the next move with smallest outcome.
        return *min_element(allOutcomes.begin(), allOutcomes.end());
    }
}

int Leyla::valueOf(const Gameplay &state) const
{
    if (state.winner() == m_aiPlayer)
    {
        return numeric_limits<int>::max();
    }

    if (state.winner() == OPPONENT_OF(m_aiPlayer))
    {
        return numeric_limits<int>::min();
    }

    int p = parity(state);
    int m = mobility(state);
    int c = cornersCaptured(state);
    int s = stability(state);

    int parityWeight = 10;
    int mobilityWeight = 78;
    int cornersWeight = 120;
    int stabilityWeight = 60;

    if ((state.board().size() * state.board().size()) - (state.currentDiscCount(Gameplay::Player_White) + state.currentDiscCount(Gameplay::Player_Black)) < 10)
    {
        //override these at the end of the game:
        parityWeight = 120;
        cornersWeight = 10;
    }

    return (parityWeight * p) + (cornersWeight * c) + (mobilityWeight* m) + (stabilityWeight * s);
}


//Heuristics inspired by https://courses.cs.washington.edu/courses/cse573/04au/Project/mini1/RUSSIA/Final_Paper.pdf

int Leyla::parity(const Gameplay &state)const
{
    int aiDiscCount = state.currentDiscCount(m_aiPlayer);
    int humanDiscCount = state.currentDiscCount(OPPONENT_OF(m_aiPlayer));
    return 100 * (aiDiscCount - humanDiscCount) / (aiDiscCount + humanDiscCount);
}

int Leyla::mobility(const Gameplay &state)const
{
    int aiMoves = state.validMovesCount(m_aiPlayer);
    int humanMoves = state.validMovesCount(OPPONENT_OF(m_aiPlayer));

    if (aiMoves + humanMoves == 0)
    {
        return 0;
    }

    return 100 * (aiMoves - humanMoves) / (aiMoves + humanMoves);
}

int Leyla::cornersCaptured(const Gameplay &state)const
{
    const Board &gameBoard = state.board();

    int aiCorners = 0;
    int humanCorners = 0;

    aiCorners += gameBoard.at(Board::Coordinates(0,0)) == DISC_OF(m_aiPlayer);
    aiCorners += gameBoard.at(Board::Coordinates(0,7)) == DISC_OF(m_aiPlayer);
    aiCorners += gameBoard.at(Board::Coordinates(7,0)) == DISC_OF(m_aiPlayer);
    aiCorners += gameBoard.at(Board::Coordinates(7,7)) == DISC_OF(m_aiPlayer);

    humanCorners += gameBoard.at(Board::Coordinates(0,0)) == DISC_OF(OPPONENT_OF(m_aiPlayer));
    humanCorners += gameBoard.at(Board::Coordinates(0,7)) == DISC_OF(OPPONENT_OF(m_aiPlayer));
    humanCorners += gameBoard.at(Board::Coordinates(7,0)) == DISC_OF(OPPONENT_OF(m_aiPlayer));
    humanCorners += gameBoard.at(Board::Coordinates(7,7)) == DISC_OF(OPPONENT_OF(m_aiPlayer));


    if (aiCorners + humanCorners == 0)
    {
        return 0;
    }

    return 100 * (aiCorners - humanCorners) / (aiCorners + humanCorners);
}

int Leyla::stability(const Gameplay &state)const
{
    int aiStability = 0;
    int humanStability = 0;

    int stabilityValues[8][8] = {
        {4,  -3,  2,  2,  2,  2, -3,  4, },
        {-3, -4, -1, -1, -1, -1, -4, -3, },
        { 2, -1,  1,  0,  0,  0,  1,  2, },
        { 2, -1,  0,  1,  1,  0, -1,  2, },
        { 2, -1,  0,  1,  1,  0, -1,  2, },
        { 2, -1,  1,  0,  0,  0,  1,  2, },
        {-3, -4, -1, -1, -1, -1, -4, -3, },
        { 4, -3,  2,  2,  2,  2, -3,  4, },
    };

    for (int row = 0; row < state.size(); ++row)
    {
        for (int col = 0; col < state.size(); ++col)
        {
            if (state.board().at(Board::Coordinates(row, col)) == DISC_OF(m_aiPlayer))
            {
                aiStability += stabilityValues[row][col];
            }
            else if (state.board().at(Board::Coordinates(row, col)) == DISC_OF(OPPONENT_OF(m_aiPlayer)))
            {
                humanStability += stabilityValues[row][col];
            }
        }
    }

    if (aiStability + humanStability == 0)
    {
        return 0;
    }

    return 100 * (aiStability - humanStability) / (aiStability + humanStability);
}

