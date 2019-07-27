#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include "cocos2d.h"
#include "Entity.h"
#include "MenuScene.h"

class MenuScene;

class Command {
public:
    virtual void execute()=0;
};

class NullCommand: public Command {
public:
    void execute() { }
};

class MoveByCommand: public Command {
public:
    //AddSpeedCommand(Player* unit,CollisionManager* manager,cocos2d::Vec2 newPos);
    MoveByCommand(cocos2d::Sprite* unit,MenuScene* scene,const cocos2d::Vec2 byPosition);
    void execute();

private:
    cocos2d::Sprite* _unit;
    MenuScene* _scene;
    cocos2d::Vec2 newPos;
};

class PlaceBomb: public Command {
public:
};


#endif // COMMAND_H_INCLUDED
