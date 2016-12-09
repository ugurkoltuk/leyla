#include "gameplay.h"

#include <utility>
#include <vector>
#include <iostream>

using namespace std;

Gameplay::Gameplay():
    m_board(8),
    m_currentPlayer(Player_Black)
{

}

Gameplay::Gameplay(const Gameplay &rhs)
    :m_board(rhs.m_board),
      m_currentPlayer(rhs.m_currentPlayer)

{

}

#define     CURRENT_PLAYERS_DISC()      (currentPlayer() == Player_White ? Board::Disc_White : Board::Disc_Black)
#define     CURRENT_OPPONENTS_DISC()    (currentPlayer() == Player_White ? Board::Disc_Black : Board::Disc_White)


void Gameplay::getIncrement(Direction dir, int &row_increment, int &col_increment)
{
    row_increment = 0;
    col_increment = 0;

    if (dir == Direction_NorthWest || dir == Direction_North || dir == Direction_NorthEast)
    {
        row_increment = -1;
    }
    else if (dir == Direction_SouthWest || dir == Direction_South || dir == Direction_SouthEast)
    {
        row_increment = +1;
    }

    if (dir == Direction_NorthWest || dir == Direction_West || dir == Direction_SouthWest)
    {
        col_increment = -1;
    }
    else if (dir == Direction_NorthEast || dir == Direction_East || dir == Direction_SouthEast)
    {
        col_increment = +1;
    }
}

void Gameplay::advance(void)
{
    m_currentPlayer = m_currentPlayer == Player_White ? Player_Black : Player_White;
}

bool Gameplay::play(const Board::Coordinates &coordinates)
{
    if (!m_board.isInBoard(coordinates))
    {
        return false;
    }

    if (m_board.at(coordinates) != Board::Disc_None)
    {
        return false;
    }

    bool didAnything = false;
    vector<Board::Coordinates> viableRays[Direction_Max];
    for (Direction dir = Direction_NorthWest; dir < Direction_Max; dir = (Direction)((int)dir + 1))// WTF cast.
    {
        int row_increment, col_increment;
        int candidate_row, candidate_col;

        getIncrement(dir, row_increment, col_increment);
        candidate_row = coordinates.first + row_increment;
        candidate_col = coordinates.second + col_increment;

        while (1)
        {
            Board::Coordinates candidate_coordinates(candidate_row, candidate_col);

            if (!m_board.isInBoard(candidate_coordinates))
            {
                viableRays[dir].clear();
                break;
            }

            if (m_board.at(candidate_coordinates) == Board::Disc_None)
            {
                viableRays[dir].clear();
                break;
            }
            else if (m_board.at(candidate_coordinates) == CURRENT_OPPONENTS_DISC())
            {
                viableRays[dir].push_back(candidate_coordinates);
            }
            else if (m_board.at(candidate_coordinates) == CURRENT_PLAYERS_DISC())
            {
                break;
            }

            candidate_row += row_increment;
            candidate_col += col_increment;
        }
        for (Board::Coordinates flipCoordinates : viableRays[dir])
        {
            m_board.flip(flipCoordinates);
            didAnything = true;
        }
    }

    if (didAnything)
    {
        m_board.at(coordinates) = CURRENT_PLAYERS_DISC();
        advance();
    }

    return didAnything;

}

Gameplay::Player Gameplay::currentPlayer(void) const
{
    return m_currentPlayer;
}

size_t Gameplay::currentDiscCount(Player whom) const
{
    size_t row, col;
    size_t discCount = 0;

    Board::Disc discInQuestion = (whom == Player_White ? Board::Disc_White : Board::Disc_Black);

    for (row = 0; row < size(); ++row)
    {
        for (col = 0; col < size(); ++col)
        {
            if (m_board.at(Board::Coordinates(row, col)) == discInQuestion)
            {
                discCount++;
            }
        }
    }

    return discCount;
}

size_t Gameplay::size(void) const
{
    return m_board.size();
}

bool Gameplay::hasValidMoves(Player player) const
{
    for (size_t row = 0; row < size(); ++row)
    {
        for (size_t col = 0; col < size(); ++col)
        {
            Gameplay copy(*this);
            if (player != currentPlayer())
            {
                copy.advance();
            }
            if (copy.play(Board::Coordinates(row,col)) == true)
            {
                return true;
            }
        }
    }
    return false;
}

bool Gameplay::hasEnded(void) const
{
    return !hasValidMoves(Player_Black) && !hasValidMoves(Player_White);
}

Gameplay::Player Gameplay::winner(void) const
{
    if (hasEnded())
    {
        if (currentDiscCount(Player_White) > currentDiscCount(Player_Black))
        {
            return Player_White;
        }
        if (currentDiscCount(Player_White) < currentDiscCount(Player_Black))
        {
            return Player_Black;
        }
    }
    return Player_None;
}
std::ostream& operator<<(std::ostream& os, const Gameplay &g)
{
    os << g.m_board;
    os << "White: " << g.currentDiscCount(Gameplay::Player_White) << " Black: " << g.currentDiscCount(Gameplay::Player_Black);
    os << ", Current player: " << (g.currentPlayer() == Gameplay::Player_White ?  "White" : "Black") << endl;

    return os;
}

