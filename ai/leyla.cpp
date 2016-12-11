#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "leyla.h"

using namespace std;

#define OPPONENT_OF(aPlayer) ((aPlayer) == Gameplay::Player_White ? Gameplay::Player_Black : Gameplay::Player_White)

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
        if (state.currentPlayer() == m_aiPlayer)
        {
            return *max_element(allOutcomes.begin(), allOutcomes.end());
        }
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

    return state.currentDiscCount(m_aiPlayer) - state.currentDiscCount(OPPONENT_OF(m_aiPlayer));
}
