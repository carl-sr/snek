#include <iostream>
#include <vector>
#include <algorithm>
#include <ncurses.h>

struct Coordinate
{
    int x;
    int y;
};

Coordinate operator+(const Coordinate a, const Coordinate b)
{
    return {a.x + b.x, a.y + b.y};
}

bool operator==(const Coordinate a, const Coordinate b)
{
    return a.x == b.x && a.y == b.y;
}

void print(const Coordinate c, char ch = 'x')
{
    mvaddch(c.y, c.x, ch);
}

enum class Direction
{
    UP = 0,
    LEFT = 1,
    DOWN = 2,
    RIGHT = 3,
};

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
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(75);
    refresh();
}

Scoped_Screen::~Scoped_Screen()
{
    nocbreak();
    endwin();
}

bool opposite(Direction a, Direction b)
{
    switch (a)
    {
    case (Direction::UP):
        return b == Direction::DOWN;
    case (Direction::LEFT):
        return b == Direction::RIGHT;
    case (Direction::DOWN):
        return b == Direction::UP;
    case (Direction::RIGHT):
        return b == Direction::LEFT;
    default:
        return false;
    }
}

int main(int argc, char *argv[])
{
    Scoped_Screen s;

    int height;
    int width;
    getmaxyx(stdscr, height, width);

    std::vector<Coordinate> snek(1, {1, 1});
    Direction snek_direction = Direction::RIGHT;

    // adjust based on given direction
    Coordinate adjust[4] = {
        {0, -1}, // UP
        {-1, 0}, // LEFT
        {0, 1},  // DOWN
        {1, 0}   // RIGHT
    };
    Coordinate ball{1 + rand() % (width - 1), 1 + rand() % (height - 1)};
    int score{1};

    int c{'\0'};
    do
    {
        // print stuff
        clear();
        print(ball, 'O');
        std::for_each(snek.begin(), snek.end(), [](auto c) { print(c); });
        mvprintw(0, 0, "SNEK - score: %i", score);
        // display some information about terminal height/width and snake position
        // mvprintw(0, 30, "max height: %i, max width: %i current: (%i, %i)", height, width, snek.front().x, snek.front().y);

        refresh();

        // get new snake direction based on keyboard input
        Direction new_direction = snek_direction;
        c = getch();
        switch (c)
        {
        case KEY_UP:
            new_direction = Direction::UP;
            break;
        case KEY_LEFT:
            new_direction = Direction::LEFT;
            break;
        case KEY_DOWN:
            new_direction = Direction::DOWN;
            break;
        case KEY_RIGHT:
            new_direction = Direction::RIGHT;
            break;
        }
        // new snek direction should not be backwards into itself
        if (!opposite(snek_direction, new_direction) || snek.size() == 1)
            snek_direction = new_direction;

        // move the snek
        snek.back() = snek.front() + adjust[(int)snek_direction];
        std::rotate(snek.rbegin(), snek.rbegin() + 1, snek.rend());

        // check snek boundaries
        std::for_each(snek.begin(), snek.end(), [&](const Coordinate &i) {
            if (i.x >= width || i.x < 0)
                c = 'q';
            if (i.y >= height || i.y < 1)
                c = 'q';
        });
        // check snek self collisions
        if (std::find(snek.begin() + 1, snek.end(), snek.front()) != snek.end())
            c = 'q';

        // check ball collision
        if (std::find(snek.begin(), snek.end(), ball) != snek.end())
        {
            // increment score, generate new ball, add section to snek
            ++score;
            do
            {
                ball = {rand() % width, rand() % height};
            } while (std::find(snek.begin(), snek.end(), ball) != snek.end());
            snek.push_back(snek.back());
        }

    } while (c != 'q');

    return 0;
}