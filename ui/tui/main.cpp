#include <iostream>
#include <cctype>
#include <termcap.h>

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

int main()
{
    Gameplay game;
    Leyla leyla(5);
    bool humanPlayed = false;
    bool leylaPlayed = false;

   Board::Coordinates leylaCoordinates;

    while (!game.hasEnded())
    {
        size_t row, col;
        char col_char;

        do {
            clear_screen();
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

         game.play(leylaCoordinates = leyla.play(game));
         leylaPlayed = true;
         humanPlayed = false;
    }

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

    return 0;
}
