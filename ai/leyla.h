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

    //heuristics:
    int parity(const Gameplay &state)const;
    int mobility(const Gameplay &state)const;
    int cornersCaptured(const Gameplay &state)const;
public:
    Leyla(size_t depth, Gameplay::Player aiPlayer);
    Board::Coordinates play(const Gameplay &state)const;
};

#endif // LEYLA_H
