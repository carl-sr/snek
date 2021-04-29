#include <iostream>
#include <ncurses.h>

// ncurses initialization / destruction functions
class Scoped_Screen
{
public:
    Scoped_Screen();
    ~Scoped_Screen();
};

Scoped_Screen::Scoped_Screen()
{
    initscr();
    cbreak();
    noecho();
    clear();
    refresh();
}

Scoped_Screen::~Scoped_Screen()
{
    endwin();
}

int main(int argc, char *argv[])
{
    Scoped_Screen s;

    getch();

    return 0;
}