#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "RakString.h"

const int MAX_CONNECTIONS = 4;

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
