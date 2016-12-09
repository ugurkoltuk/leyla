#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <utility>
#include <iosfwd>

class Board
{
public:
    enum Disc
    {
        Disc_White,
        Disc_Black,
        Disc_None,
    };
private:
    std::vector<std::vector<Disc>> m_board;
    void allocateBoardVector(size_t dimension);
public:
    typedef std::pair<size_t, size_t> Coordinates;

    explicit Board(size_t dim);
    Board(const Board &rhs);

    size_t size(void)const;
    bool isInBoard(const Coordinates &coordinates)const;

    Disc &at(const Coordinates &coordinates);
    const Disc &at(const Coordinates &cooridnates)const;

    void flip(const Coordinates &coordinates);

};

std::ostream& operator<<(std::ostream& os, const Board &b);



#endif // BOARD_H
