#ifndef REVERSIOPPONENT_H
#define REVERSIOPPONENT_H


#include <boost/asio.hpp>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

using boost::asio::ip::tcp;

/**
 * @brief The ReversiOpponent class
 *              This class represents an opponent that plays reversi. In practice,
 *              it represents the other reverspi player AI (desdemona?). This is a
 *              synchronous class. It could have been an asynchronous one where
 *              for example a callback is called upon receiving a move, but I'll
 *              maybe do that later.
 */
class ReversiOpponent
{
private:
    static boost::asio::io_service ioService;
    const char *m_adversaryIp;
    const char *m_adversaryPort;
    const char *m_myPort;
    std::mutex m_acceptMutex;
    std::condition_variable m_acceptCV;
    tcp::socket m_readSocket;
    tcp::acceptor m_acceptor;
    bool m_connectionAccepted;
    bool m_messageReceived;
    std::thread m_acceptorThread;
    std::atomic<bool> m_programBeingTerminated;

    size_t transmit_msg(const void *msg, size_t size);
    size_t receive_msg(void *msg, size_t size);
public:
    /**
     * @brief ReversiOppoent
     * @param adversaryIp
     *          IP or hostname of the adversary to connect to.
     * @param adversaryPort
     *          Port of the server to adversary to.
     * @param myPort
     *          Port number for listening to - for incoming connections.
     */
    ReversiOpponent(const char *adversaryIp, const char *adversaryPort, const char *myPort);

    /**
      * @brief
      *         Destructor for the ReversiOpponent. Shuts down any remaining
      *         connections associated with the object being destructed.
      */
    ~ReversiOpponent();


    /**
     * @brief decideColor
     *          Play paper-scissor-rock with the other party
     *          and return the result. Please note this is a blocking function.
     * @return
     *          0   => Error.
     *          1   => Local party is white (remote party is to do the first move)
     *          2   => Local party is black (local party is to do the first move)
     */
    int decideColor(void);

    /**
     * @brief sendMove
     *          Send a move across the network to the other player. If the other player
     *          recognizes the move as a valid one and acknowledges this, return true, and
     *          return false otherwise.
     * @param row
     *          Row at which to place a new disk.
     * @param col
     *          Column at which to place a new disk.
     * @return
     *          True on success, false otherwise.
     */
    bool sendMove(int row, int col);

    /**
     * @brief receiveMove
     *          Block until the opponent sends a valid move, and fill row & col with the coordinates
     *          of the move.
     * @param [out] row
     *          row of the move that was sent, if return value is true. Otherwise, the value shall be untouched.
     * @param [out] col
     *          col of the move that was sent, if return value is true. Otherwise, the value shall be untouched.
     * @return
     *          True on success, false on failure.
     */
    bool receiveMove(int &row, int &col);

    /**
     * @brief pass
     *          Send a pass request to the other party.
     * @return
     *          True if pass successful, false otherwise.
     */
    bool pass(void);
};

#endif // REVERSIOPPONENT_H
