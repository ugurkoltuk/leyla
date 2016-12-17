#ifndef LEYLA_H
#define LEYLA_H

#include "game/gameplay.h"

class Leyla
{
private:
    size_t m_depth;
    Gameplay::Player m_aiPlayer;
    int evaluate(const Gameplay &state, size_t depth)const;
    int valueOf(const Gameplay &state)const;

    //heuristics & heuristic helpers:
    int parity(const Gameplay &state)const;
    int mobility(const Gameplay &state)const;
    int cornersCaptured(const Gameplay &state)const;
    int stability(const Gameplay &state)const;

    int m_parityWeight;
    int m_mobilityWeight;
    int m_cornersWeight;
    int m_stabilityWeight;

public:
    Leyla(size_t depth, Gameplay::Player aiPlayer, int pw, int mw, int cw, int sw);
    Board::Coordinates play(const Gameplay &state)const;
    static int coinStability(const Board &board, size_t row, size_t col);
};

#endif // LEYLA_H
