#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include "cocos2d.h"
#include "Entity.h"

class Command {
public:
    virtual void execute()=0;
};

class NullCommand: public Command {
public:
    void execute() { }
};

class NewSpeedCommand: public Command {
public:
    NewSpeedCommand(Player* unit,cocos2d::Vec2 newSpeed);

    void execute();

private:
    Player* _unit;
    cocos2d::Vec2 _speed;
};


#endif // COMMAND_H_INCLUDED
