#include "board.h"

#include <iostream>
#include <stdexcept>
#include "../ai/leyla.h"

using namespace std;

Board::Board(size_t dimension)
{
    at(Coordinates(3, 3)) = Disc_White;
    at(Coordinates(4, 4)) = Disc_White;

    at(Coordinates(3, 4)) = Disc_Black;
    at(Coordinates(4, 3)) = Disc_Black;
}

Board::Board(const Board &rhs)
{
    m_board_white_discs = rhs.m_board_white_discs;
    m_board_black_discs = rhs.m_board_black_discs;
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

const Board::Disc &Board::setter_proxy::operator=(const Board::Disc &rval)
{
    if (rval == Disc_White)
    {
        m_board.m_board_white_discs[m_row * 8 + m_col] = 1;
        m_board.m_board_black_discs[m_row * 8 + m_col] = 0;
    }
    else if (rval == Disc_Black)
    {
        m_board.m_board_white_discs[m_row * 8 + m_col] = 0;
        m_board.m_board_black_discs[m_row * 8 + m_col] = 1;
    }
    else
    {
        m_board.m_board_white_discs[m_row * 8 + m_col] = 0;
        m_board.m_board_black_discs[m_row * 8 + m_col] = 0;
    }

    return rval;
}

Board::setter_proxy::operator Disc()const
{
    if (m_board.m_board_black_discs[m_row * 8 + m_col] && !m_board.m_board_white_discs[m_row * 8 + m_col])
    {
        return Disc::Disc_Black;
    }

    if (!m_board.m_board_black_discs[m_row * 8 + m_col] && m_board.m_board_white_discs[m_row * 8 + m_col])
    {
        return Disc::Disc_White;
    }

    if (!m_board.m_board_black_discs[m_row * 8 + m_col] && !m_board.m_board_white_discs[m_row * 8 + m_col])
    {
        return Disc::Disc_None;
    }

}


Board::setter_proxy Board::at(const Coordinates &coordinates)
{
    if (!isInBoard(coordinates))
    {
        throw out_of_range("Element out of board range.");
    }

    return setter_proxy(*this, coordinates.first, coordinates.second);
}

Board::Disc Board::at(const Coordinates &cooridnates) const
{
    if (!isInBoard(cooridnates))
    {
        throw out_of_range("Element out of board range.");
    }

    return Board::Disc(Disc_None + (m_board_white_discs[cooridnates.first * 8 + cooridnates.second] + 2 * m_board_black_discs[cooridnates.first * 8 + cooridnates.second]));
}

void Board::flip(const Coordinates &coordinates)
{
    if (isInBoard(coordinates))
    {
        m_board_white_discs.flip(coordinates.first * 8 + coordinates.second);
        m_board_black_discs.flip(coordinates.first * 8 + coordinates.second);
    }
}

size_t Board::size(void)const
{
    return 8;
}

ostream& operator<<(ostream& os, const Board &gameBoard)
{
    size_t row = 0;
    size_t col = 0;

    for (row = 0; row < gameBoard.size(); ++row)
    {
        if (row == 0)
        {
            os << "   ";
            for (col = 0; col < gameBoard.size(); ++col)
            {
                os << char('A' + col) << " ";
            }
            os << endl;
        }
        os << row + 1 << "  ";


        for (col = 0; col < gameBoard.size(); ++col)
        {
            switch (gameBoard.at(Board::Coordinates(row,col)))
            {
            case Board::Disc::Disc_None:
                os << ". ";
                break;
            case Board::Disc::Disc_Black:
                if (Leyla::coinStability(gameBoard, row, col) == 1)
                os << "○'";
                else
                os << "○ ";
                break;
            case Board::Disc::Disc_White:
                if (Leyla::coinStability(gameBoard, row, col) == 1)
                os << "●'";
                else
                os << "● ";
                break;
            default:
                os << "??";
                break;
            }
        }

        os << endl;
    }
    return os;
}
