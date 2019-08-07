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
   PlaceBomb(const cocos2d::Vec2& position,MenuScene* scene,int range):_position(position),_scene(scene),_range(range) { }

   void execute();

private:
    cocos2d::Vec2 _position;
    MenuScene* _scene;
    int _range;

};

enum Direction {
    LEFT,UP,RIGHT,DOWN
};

class ShootRay: public Command {
public:
    ShootRay(const cocos2d::Vec2& startPosition,Direction direction,int range,MenuScene* scene);

    void execute();

private:
    cocos2d::Vec2 _startPosition;
    Direction _direction;
    int _range;
    MenuScene* _scene;
};


#endif // COMMAND_H_INCLUDED
