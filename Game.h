#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "RakString.h"

const int MAX_CONNECTIONS = 2;
const int MAX_APPLES = 2;
const float SNAKE_SPEED = 32.0f;
const float APPLE_TIMER = 2.0f;
const float MOVE_TIMER = 0.5f;
const unsigned int SIZE = 32;

const short WIDTH = 32*25;
const short HEIGHT = 32*24;

enum USER_TYPE {
    CLIENT,
    SERVER
};

enum TeamColor {
    RED,GREEN,BLUE,YELLOW
};

enum Direction {
    LEFT,UP,RIGHT,DOWN
};

struct Player {
    TeamColor color;
    RakNet::RakString playerName;
};

#endif // GAME_H_INCLUDED
