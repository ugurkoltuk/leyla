#include "reversiopponent.h"
#include <iostream>
#include <random>
#include <chrono>


using boost::asio::ip::tcp;
using namespace std;

boost::asio::io_service ReversiOpponent::ioService;

ReversiOpponent::ReversiOpponent(const char *adversaryIp, const char *adversaryPort, const char *myPort)
       :
        m_adversaryIp(adversaryIp),
        m_adversaryPort(adversaryPort),
        m_myPort(myPort),
        m_acceptor(ioService, tcp::endpoint(tcp::v4(), atoi(m_myPort))),
        m_readSocket(ioService),
        m_connectionAccepted(false),
        m_messageReceived(false),
        m_programBeingTerminated(false),
        m_acceptorThread
                    (
                        [this]
                        {
                            for (;;)
                            {
                                {
                                    std::lock_guard<std::mutex> lg(m_acceptMutex);
                                    m_acceptor.accept(m_readSocket);
                                    m_connectionAccepted = true;
                                    m_messageReceived = false;
                                }

                                if (m_programBeingTerminated)
                                {
                                    break;
                                }

                                m_acceptCV.notify_one();

                                std::unique_lock<std::mutex> lk(m_acceptMutex);
                                m_acceptCV.wait(lk, [this]{return m_messageReceived;});
                                lk.unlock();
                            }
                        }
                    )
{

}

ReversiOpponent::~ReversiOpponent()
{
    //stimulate the acceptor thread.
    m_programBeingTerminated = true;

    try
    {
        tcp::resolver resolver(ioService);
        tcp::resolver::query query("localhost", m_myPort, tcp::resolver::query::canonical_name);
        tcp::socket writeSocket(ioService);
        boost::asio::connect(writeSocket, resolver.resolve((query)));
        writeSocket.close();
    }
    catch (boost::system::system_error &e)
    {
        cerr << "Fail while self-connecting to stop accept loop" << endl;
    }


    m_acceptorThread.join();
}

size_t ReversiOpponent::transmit_msg(const void *msg, size_t size)
{
    tcp::resolver resolver(ioService);
    tcp::resolver::query query(m_adversaryIp, m_adversaryPort, tcp::resolver::query::canonical_name);
    tcp::socket writetSocket(ioService);
    bool connected = false;
    while (!connected)
    {
        try
        {
            boost::asio::connect(writetSocket, resolver.resolve(query));
        }
        catch (boost::system::system_error &e)
        {
            continue;
        }
        connected = true;
    }

    return boost::asio::write(writetSocket, boost::asio::buffer(msg, size));
}

size_t ReversiOpponent::receive_msg(void *msg, size_t size)
{
    std::unique_lock<std::mutex> lock(m_acceptMutex);
    m_acceptCV.wait(lock, [this]{return this->m_connectionAccepted;});

    size_t bytesRead = m_readSocket.read_some(boost::asio::buffer(msg, size));
    m_connectionAccepted = false;
    m_messageReceived = true;
    m_readSocket.close();

    lock.unlock();
    m_acceptCV.notify_one();

    return bytesRead;
}

bool ReversiOpponent::pass()
{
    return transmit_msg("PASS", strlen("PASS")) == strlen("PASS");
}

bool ReversiOpponent::sendMove(int row, int col)
{
    char moveBuffer[2] = {static_cast<char>(row + '0'), static_cast<char>(col + '0')};

    return transmit_msg(moveBuffer, sizeof(moveBuffer)) == sizeof(moveBuffer);
}

bool ReversiOpponent::receiveMove(int &row, int &col)
{
    char opponentMoveBuffer[5] = {0, }; //it may also be PASS
    size_t readBytes = receive_msg(opponentMoveBuffer, sizeof(opponentMoveBuffer));

    if (readBytes == 2)
    {
        row = opponentMoveBuffer[0] - '0';
        col = opponentMoveBuffer[1] - '0';

        cout << "Received (" << row+1 << ", " << static_cast<char>(col+'A') << ")" << endl;
    }
    else if (readBytes == 4 && string(opponentMoveBuffer) == "PASS")
    {
        cout << "Received " << opponentMoveBuffer << endl;
    }
    else
    {
        cout << "An error in transmission. Received: " << opponentMoveBuffer << endl;
    }

    return readBytes == 2;
}

int ReversiOpponent::decideColor(void)
{
    bool decided = false;
    char mySelection, opponentSelection;

    while (!decided)
    {
        default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());
        uniform_int_distribution<int> distribution(0, 2);

        mySelection = '0' + distribution(generator);
        cout << "My selection is " << static_cast<char>(mySelection) << " i will now communicate it through. " << endl;

        transmit_msg(&mySelection, sizeof(mySelection));
        receive_msg(&opponentSelection, sizeof(opponentSelection));

        cout << "Opponents selection is " << static_cast<char>(opponentSelection) << endl;

        mySelection -= '0';
        opponentSelection -= '0';

        decided = mySelection != opponentSelection;
        if (!decided)
        {
            cout << "Draw in RPS, RETRY!" << endl;
        }
    }

    if (opponentSelection < 0 || opponentSelection > 2)
    {
        return 0;
    }

    int rpsResult[][3] = {
                            {0, 1, 2, },
                            {2, 0, 1, },
                            {1, 2, 0, },
                         };

    return rpsResult[mySelection][opponentSelection];
}
