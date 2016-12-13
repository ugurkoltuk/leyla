#ifndef BOARD_H
#define BOARD_H

#include <bitset>
#include <utility>
#include <iosfwd>

class Board
{
private:
    std::bitset<64> m_board_white_discs;
    std::bitset<64> m_board_black_discs;
public:
    typedef std::pair<size_t, size_t> Coordinates;

    enum Disc
    {
        Disc_None = 0,
        Disc_White,
        Disc_Black,
    };

    explicit Board(size_t dim);
    Board(const Board &rhs);

    size_t size(void)const;
    bool isInBoard(const Coordinates &coordinates)const;

    class setter_proxy {
        int m_row, m_col;
        Board &m_board;
    public:
        setter_proxy(Board &board, int row, int col) :m_row(row), m_col(col), m_board(board){}
        const Disc &operator=(const Disc &rval);
        operator Disc()const;
    };

    setter_proxy at(const Coordinates &coordinates);
    Disc at(const Coordinates &cooridnates)const;

    void flip(const Coordinates &coordinates);
};

std::ostream& operator<<(std::ostream& os, const Board &b);



#endif // BOARD_H
