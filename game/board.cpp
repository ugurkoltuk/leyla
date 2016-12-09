#include "board.h"

#include <iostream>
#include <stdexcept>

using namespace std;

Board::Disc m_invalid;

void Board::allocateBoardVector(size_t dimension)
{
    for (int i = 0; i < dimension; ++i)
    {
        m_board.push_back(vector<Board::Disc>(dimension, Board::Disc::Disc_None));
    }
}

Board::Board(size_t dimension)
{
    allocateBoardVector(dimension);

    at(Coordinates(3, 3)) = Disc_White;
    at(Coordinates(4, 4)) = Disc_White;

    at(Coordinates(3, 4)) = Disc_Black;
    at(Coordinates(4, 3)) = Disc_Black;
}

Board::Board(const Board &rhs)
{
    allocateBoardVector(rhs.size());

    for (int row = 0; row < rhs.size(); ++row)
    {
        for (int col = 0; col < rhs.size(); ++ col)
        {
            at(Coordinates(row, col)) = rhs.at(Board::Coordinates(row, col));
        }
    }

}

bool Board::isInBoard(const Coordinates &coordinates)const
{
    if (coordinates.first >= size())
    {
        return false;
    }
    if (coordinates.second >= size())
    {
        return false;
    }

    return true;
}


Board::Disc &Board::at(const Coordinates &coordinates)
{
    if (isInBoard(coordinates))
    {
        return m_board[coordinates.first][coordinates.second];
    }

    throw out_of_range("Cannot access element.");
}

const Board::Disc &Board::at(const Coordinates &cooridnates) const
{
    if (isInBoard(cooridnates))
    {
        return m_board[cooridnates.first][cooridnates.second];
    }

    throw out_of_range("Cannot access element.");
}

void Board::flip(const Coordinates &coordinates)
{
    if (isInBoard(coordinates))
    {
        if (at(coordinates) == Disc_White)
        {
            at(coordinates) = Disc_Black;
        }
        else if (at(coordinates) == Disc_Black)
        {
            at(coordinates) = Disc_White;
        }
        else
        {
            throw logic_error("Trying to flip a disc that doesn't exist...");
        }
    }
}

size_t Board::size(void)const
{
    return m_board.size();
}

ostream& operator<<(ostream& os, const Board &gameBoard)
{
    size_t row = 0;
    size_t col = 0;

    for (row = 0; row < gameBoard.size(); ++row)
    {
        if (row == 0)
        {
            os << "     ";
            for (col = 0; col < gameBoard.size(); ++col)
            {
                os << char('A' + col) << "   ";
            }
            os << endl;
        }
        os << row << "  |";


        for (col = 0; col < gameBoard.size(); ++col)
        {
            switch (gameBoard.at(Board::Coordinates(row,col)))
            {
            case Board::Disc::Disc_None:
                os << "   ";
                break;
            case Board::Disc::Disc_Black:
                os << " ● ";
                break;
            case Board::Disc::Disc_White:
                os << " ○ ";
                break;
            default:
                os << "????";
                break;
            }
            os << "|";
        }

        os << endl << endl;
    }
    return os;
}
