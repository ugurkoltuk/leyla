#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <vector>

#include "board.h"

class Gameplay
{
public:
    enum Player
    {
        Player_White,
        Player_Black,
        Player_None,
    };

    /**
     * @brief Gameplay
     *          Starts a new Reversi/Othello game.
     */
    Gameplay(void);

    /**
     * @brief Gameplay
     *          Copy constructor.
     * @param rhs
     *          The Gameplay to copy from.
     */
    Gameplay(const Gameplay &rhs);

    /**
     * @brief play
     *          This function plays a turn for the current player, flips necessary
     *          pieces and advances the game to the next turn.
     * @param coordinates
     *          Coordinates to place the piece at.
     * @return
     *          true on success, false on failure (such as wrong position).
     */
    bool play(const Board::Coordinates &cooridnates);

    /**
     * @brief advance
     *          Skip the current player's turn.
     * @return
     *          true on success, otherwise on failure.
     */
     bool skip(void);

    /**
     * @brief currentPlayer
     *          This function returns whoever is the current player.
     * @return disc color of the current player. (Black or White)
     */
    Player currentPlayer(void) const;

    /**
     * @brief currentDiscs
     *          returns the number of discs that are owned by a player at the moment.
     * @param whom
     *          The player whose disc number is requested.
     * @return
     *          Current disc number of the player.
     */
    size_t currentDiscCount(Player whom) const;

    /**
     * @brief size
     *          Gets the size of underlying board.
     * @return
     *          size of the underlying board.
     */
    size_t size(void) const;

    /**
     * @brief isValidMove
     *          Checks if given move is a valid move.
     * @param move
     *          move to check
     * @return
     *        true if a valid move, false otherwise.
     */
    bool isValidMove(Board::Coordinates move) const;

    /**
     * @brief allValidMoves
     *          Finds all valid moves for a given state of a gameplay.
     * @return
     *          A vector of all valid moves, if any.
     */
    std::vector<Board::Coordinates> allValidMoves() const;

    /**
     * @brief hasValidMoves
     *          Function to check if a given player has
     *          any valid moves.
     * @return
     */
    bool hasValidMoves() const;

    /**
     * @brief hasEnded
     *          checks if the game has ended.
     * @return
     */
    bool hasEnded(void) const;

    /**
     * @brief winner
     *          returns the winner of the game, if any. Returns Player_None
     *          if the game hasn't ended yet, or if it is a draw.
     * @return
     *          the winner of the game, if any. Returns Player_None
     *          if the game hasn't ended yet, or if it is a draw.
     */
    Player winner(void) const;


    /**
     * @brief operator <<
     *          inserter to print the current game status into a stream
     * @param os
     *          stream to print current game status into.
     * @param g
     *          gameplay to print status of.
     * @return
     *          os
     */
    friend std::ostream& operator<<(std::ostream& os, const Gameplay &g);
private:
    Board m_board;
    Player m_currentPlayer;

    enum Direction
    {
        Direction_NorthWest = 0,
        Direction_North,
        Direction_NorthEast,
        Direction_East,
        Direction_SouthEast,
        Direction_South,
        Direction_SouthWest,
        Direction_West,
        Direction_Max,
    };

    void advance(void);
    static void getIncrement(Direction dir, int &row_increment, int &col_increment);
};

#endif // GAMEPLAY_H
