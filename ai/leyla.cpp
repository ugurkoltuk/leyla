#include <cstdlib>
#include <ctime>
#include "leyla.h"

Leyla::Leyla(size_t depth)
    :m_depth(depth)
{
    srand((unsigned)time(NULL));
}

Board::Coordinates Leyla::play(const Gameplay &game)
{
    Board::Coordinates randomCoordinates;
    Gameplay localGame(game);

    do {
        randomCoordinates.first = rand() % game.size();
        randomCoordinates.second = rand() % game.size();
    } while (!localGame.play(randomCoordinates));

    return randomCoordinates;
}

