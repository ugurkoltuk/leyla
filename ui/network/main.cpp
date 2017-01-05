#include <iostream>
#include <cctype>
#include <termcap.h>
#include <unistd.h>
#include <ctime>
#include <cstring>
#include "reversiopponent.h"

#include "game/gameplay.h"
#include "ai/leyla.h"

using namespace std;


void printUsage(void)
{
    cerr << "Usage: reversi-cpp <myport> <adversaryport> <adversaryip> <search depth>" << endl;
}


int main(int argc, char **argv)
{
    if (argc < 5) {
        printUsage();
        exit(EXIT_FAILURE);
    }

    const char *myPort = argv[1];
    const char *adversaryPort = argv[2];
    const char *adversaryIp = argv[3];
    int searchDepth= atoi(argv[4]);

    Gameplay game;

    ReversiOpponent desdemona(adversaryIp, adversaryPort, myPort);
    Gameplay::Player colorOfLeyla = Gameplay::Player(desdemona.decideColor());
    cout << ((colorOfLeyla == 1) ? "LOST THE RPS GAME! :(" : "WON THE GAME!!! I start!") << endl;
    Leyla leyla(searchDepth, colorOfLeyla, 76, 40, 87, 20);

    Board::Coordinates leylaCoordinates;

    cout << game;

    if (colorOfLeyla == Gameplay::Player_Black)
    {
        cout << "leyla is thinking." << endl;
        game.play(leylaCoordinates = leyla.play(game));
        cout << "Leyla played (" << leylaCoordinates.first + 1 << ", " << static_cast<char>(leylaCoordinates.second + 'A') << ")" << endl;
        desdemona.sendMove(leylaCoordinates.first, leylaCoordinates.second);
        cout << game;
    }

    while (!game.hasEnded())
    {
        int desdemonaRow, desdemonaCol;
        bool desdemonaMoved;

        cout << "Will now wait for desdemona to play..." << endl;

        desdemonaMoved = desdemona.receiveMove(desdemonaRow, desdemonaCol);
        if (desdemonaMoved)
        {
            game.play(Board::Coordinates(desdemonaRow, desdemonaCol));
            cout << game;
        }
        else if (game.skip())
        {
            cout << "Desdemona has no valid moves!" << endl;
        }
        else
        {
            cerr << "Desdemona failed to move for some reason!!!" << endl;
            exit(EXIT_FAILURE);
        }

        if (game.hasEnded())
        {
            break;
        }


        if (game.hasValidMoves())
        {
            game.play(leylaCoordinates = leyla.play(game));
            cout << "Leyla plays (" << leylaCoordinates.first + 1 << ", " << static_cast<char>(leylaCoordinates.second + 'A') << ")" << endl;
            cout << game;
            cout << "Will send my move now." << endl;
            desdemona.sendMove(leylaCoordinates.first, leylaCoordinates.second);
        }
        else
        {
            //if we dn't have valid moves, let's send "pass"
            cout << "I dont have valid moves, i'm gonna pass." << endl;
            game.skip();
            desdemona.pass();
        }
    }

    if (game.winner() == Gameplay::Player_White)
    {
        cerr << "White player " << ((colorOfLeyla == Gameplay::Player_White) ? "(which happens to be me) " : "") << "wins!" << endl;
        return 1;
    }
    else if (game.winner() == Gameplay::Player_Black)
    {
        cerr << "Black player " << ((colorOfLeyla == Gameplay::Player_Black) ? "(which happens to be me) " : "") << "wins!" << endl;
        return 2;
    }
    else
    {
        cerr << "DRAW!!!" << endl;
        return 0;
    }


    return 0;
}
