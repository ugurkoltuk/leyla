#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "leyla.h"

using namespace std;

#define         OPPONENT_OF(aPlayer) ((aPlayer) == Gameplay::Player_White ? Gameplay::Player_Black : Gameplay::Player_White)
#define         DISC_OF(aPlayer)     ((aPlayer) == Gameplay::Player_White ? Board::Disc_White : Board::Disc_Black)
#define         NELEMS(A) (sizeof(A) / sizeof A[0])

Leyla::Leyla(size_t depth, Gameplay::Player aiPlayer, int pw, int mw, int cw, int sw)
    :m_depth(depth),
     m_aiPlayer(aiPlayer),
     m_parityWeight(pw),
     m_mobilityWeight(mw),
     m_cornersWeight(cw),
     m_stabilityWeight(sw)
{
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
        cerr << "[" << (int)m_aiPlayer << "] You are defeated in a few moves. " <<endl;
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
        Gameplay localState(state);
        if (!localState.hasValidMoves())
        {
            localState.skip();
            if (!localState.hasValidMoves())
            {
                //now that's an endgame.
                return valueOf(localState);
            }
        }

        vector<Board::Coordinates> allMoves = localState.allValidMoves();
        vector<int> allOutcomes;

#pragma omp parallel for
        for (int i = 0; i < allMoves.size(); ++i)
        {
            Gameplay nextState(localState);
            nextState.play(allMoves[i]);

            int evaluation = evaluate(nextState, depth - 1);
#pragma omp critical
            allOutcomes.push_back(evaluation);
        }

        //if it is currently AI turn, pick the next move with largest outcome.
        if (localState.currentPlayer() == m_aiPlayer)
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

    int parityWeight = m_parityWeight;
    int mobilityWeight = m_mobilityWeight;
    int cornersWeight = m_cornersWeight;
    int stabilityWeight = m_stabilityWeight;

    if (state.size() * state.size() - state.currentDiscCount(Gameplay::Player_White) - state.currentDiscCount(Gameplay::Player_Black) < 8)
    {
        //override these at the end of the game:
        parityWeight = 400;
        cornersWeight = 10;
        mobilityWeight = 4;
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

    aiCorners += (gameBoard.at(Board::Coordinates(0,0)) == DISC_OF(m_aiPlayer));
    aiCorners += (gameBoard.at(Board::Coordinates(0,7)) == DISC_OF(m_aiPlayer));
    aiCorners += (gameBoard.at(Board::Coordinates(7,0)) == DISC_OF(m_aiPlayer));
    aiCorners += (gameBoard.at(Board::Coordinates(7,7)) == DISC_OF(m_aiPlayer));

    humanCorners += (gameBoard.at(Board::Coordinates(0,0)) == DISC_OF(OPPONENT_OF(m_aiPlayer)));
    humanCorners += (gameBoard.at(Board::Coordinates(0,7)) == DISC_OF(OPPONENT_OF(m_aiPlayer)));
    humanCorners += (gameBoard.at(Board::Coordinates(7,0)) == DISC_OF(OPPONENT_OF(m_aiPlayer)));
    humanCorners += (gameBoard.at(Board::Coordinates(7,7)) == DISC_OF(OPPONENT_OF(m_aiPlayer)));


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

    size_t row, col;

    for (row = 0; row < state.size(); ++row)
    {
        for (col = 0; col < state.size(); ++col)
        {
            Board::Disc currentDisc = state.board().at(Board::Coordinates(row, col));

            if (currentDisc == DISC_OF(m_aiPlayer))
            {
                aiStability += coinStability(state.board(), row, col);
            }
            else if (currentDisc == DISC_OF(OPPONENT_OF(m_aiPlayer)))
            {
                humanStability += coinStability(state.board(), row, col);
            }
        }
    }

    if (aiStability + humanStability == 0)
    {
        return 0;
    }

    return 100 * (aiStability - humanStability) / (aiStability + humanStability);
}


/*************************************************************************
 *                                                                       *
 *                    {    +1,     if unflippable                        *
 *                    {                                                  *
 *  stability(coin) = {     0,     if flippable at some time in future   *
 *                    {                                                  *
 *                    {    -1,     if immediately flippable.             *
 *                                                                       *
 *************************************************************************/

/**
 * @brief Leyla::coinStability
 * @param board
 *          board on which stability of a coin shall be calculated
 * @param row
 *          row at which coin in question resides
 * @param col
 *          column at which coin in question resides
 * @return
 */
int Leyla::coinStability(const Board &board, size_t row, size_t col)
{
    Board::Disc currentColor = board.at(Board::Coordinates(row, col));
    int rowAndColIncrements[4][2] = {
                                        { 0, -1},
                                        {-1, -1},
                                        {-1,  0},
                                        {-1, +1},
                                    };
    int totallyStableAxisCount = 0;

    for (int inc = 0; inc < NELEMS(rowAndColIncrements); ++inc)
    {
        int r, c;

        bool firstSideOpen = false;
        bool firstSideOpponent = false;
        bool secondSideOpen = false;
        bool secondSideOpponent = false;

        // please note that *sideOpen & *sideOpponent are mutually exclusive.
        // that is, they can't be true at the same time.

        r = row;
        c = col;
        //scan in positive direction of current axis
        while (board.isInBoard(Board::Coordinates(r, c)))
        {
            if (board.at(Board::Coordinates(r, c)) == Board::Disc_None) {
                firstSideOpen = true;
                break;
            }
            if (board.at(Board::Coordinates(r, c)) != currentColor) {
                firstSideOpponent = true;
                break;
            }
            r += rowAndColIncrements[inc][0];
            c += rowAndColIncrements[inc][1];
        }

        r = row;
        c = col;
        //scan in negative direction of current axis
        while (board.isInBoard(Board::Coordinates(r, c)))
        {
            if (board.at(Board::Coordinates(r, c)) == Board::Disc_None) {
                secondSideOpen = true;
                break;
            }
            if (board.at(Board::Coordinates(r, c)) != currentColor) {
                secondSideOpponent = true;
                break;
            }
            r += -rowAndColIncrements[inc][0];
            c += -rowAndColIncrements[inc][1];
        }

        //if both sides are open, it can be flipped at some time in the future.
        if (firstSideOpen && secondSideOpen)
        {
            return 0;
        }

        //if one side's open and other belongs to opponent, it can be flipped now.
        if ((firstSideOpen && secondSideOpponent) || (firstSideOpponent && secondSideOpen))
        {
            return -1; //can be flipped now.
        }

        if ((!firstSideOpen && !firstSideOpponent) || (!secondSideOpen && !secondSideOpponent))
        {
            //if neither sides are open
            totallyStableAxisCount++;
        }

        //all the other cases: it is stable in this direction.
    }

    if (totallyStableAxisCount == NELEMS(rowAndColIncrements))
    {
        return 1;
    }

    return 0;
}
