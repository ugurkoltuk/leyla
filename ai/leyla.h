#ifndef LEYLA_H
#define LEYLA_H

#include "game/gameplay.h"

class Leyla
{
private:
    size_t m_depth;
public:
    Leyla(size_t depth);
    Board::Coordinates play(const Gameplay &game);
};

#endif // LEYLA_H
