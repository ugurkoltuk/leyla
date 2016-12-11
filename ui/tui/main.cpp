#include <iostream>
#include <cctype>
#include <termcap.h>
#include <unistd.h>
#include <ctime>

#include "game/gameplay.h"
#include "ai/leyla.h"

using namespace std;

void clear_screen()
{
    char buf[1024];
    char *str;

    tgetent(buf, getenv("TERM"));
    str = tgetstr("cl", NULL);
    fputs(str, stdout);
}

int main(int argc, char **argv)
{
    Gameplay game;

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <search depth>" << endl;
        exit(EXIT_FAILURE);
    }

    Leyla leyla(atoi(argv[1]), Gameplay::Player_White);
    bool humanPlayed = false;
    bool leylaPlayed = false;

    clear_screen();
    Board::Coordinates leylaCoordinates;
    cout << game;

    struct timespec tm, tm2;
    clock_gettime(CLOCK_MONOTONIC, &tm);
    while (!game.hasEnded())
    {
#if 0
        size_t row, col;
        char col_char;

        do {
            clear_screen();
            if (!game.hasValidMoves())
            {
                cout <<"Human has no valid moves!" << endl;
                sleep(3);
                game.skip();
                break;
            }

            if (humanPlayed)
            {
                cout << "Invalid move (" << row << ", " << col_char << ")! Try again." << endl;
            }
            humanPlayed = true;

            if (leylaPlayed)
            {
                cout << "Leyla played: (" << leylaCoordinates.first << ", " << char(leylaCoordinates.second + 'A') << ")." << endl;
            }
            cout << game;
            cout << "Enter your move:" << endl;
            cin >> row >> col_char;
            col = toupper(col_char) - 'A';
        }
        while (!game.play(Board::Coordinates(row, col)));
#endif

        Leyla leyla2(atoi(argv[1]), Gameplay::Player_Black);
        clear_screen();
        cout << game;

        if (!game.hasValidMoves())
        {
            cout << "Leyla2 has no valid moves." << endl;
            //sleep(3);
            game.skip();
            continue;
        }
        cout << "Leyla2 is thinking ... " << endl;
        game.play(leylaCoordinates = leyla2.play(game));
        clear_screen();
        cout << game;

        if (!game.hasValidMoves())
        {
            cout << "Leyla has no valid moves." << endl;
            //sleep(3);
            game.skip();
            continue;
        }
        cout << "Leyla is thinking ... " << endl;
        game.play(leylaCoordinates = leyla.play(game));
        clear_screen();
        cout << game;


        leylaPlayed = true;
        humanPlayed = false;
    }
    clock_gettime(CLOCK_MONOTONIC, &tm2);


    clear_screen();
    cout << game;

    if (game.winner() == Gameplay::Player_Black)
    {
        cout << "Black player wins!" << endl;
    }
    else if (game.winner() == Gameplay::Player_White)
    {
        cout << "White wins!" << endl;
    }
    else
    {
        cout << "DRAW!!!" << endl;
    }
    cout << "elapsed: " << (tm2.tv_nsec - tm.tv_nsec) / 1000000 + ((tm2.tv_sec - tm.tv_sec) * 1000) << " milliseconds." << endl;

    return 0;
}
