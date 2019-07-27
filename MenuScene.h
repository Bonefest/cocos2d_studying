#ifndef MENUSCENE_H_INCLUDED
#define MENUSCENE_H_INCLUDED

#include "cocos2d.h"
#include "KeyManager.h"
#include "Entity.h"
#include "Command.h"

class Command;

#include <list>

const int NUM_PLAYERS = 4;
const float PLAYER_SPEED = 200;

const int MAP_WIDTH = 13;
const int MAP_HEIGHT = 15;

const int BLOCK_SIZE = 32;

class MenuScene : cocos2d::Scene {
public:
    virtual bool init();

    CREATE_FUNC(MenuScene);

    static Scene* createScene();
    void menuCloseCallback(cocos2d::Ref* psender);

    void update(float delta);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);

    std::vector<Command*> handleInput(float delta);

    bool isCollide(cocos2d::Node* objectA,cocos2d::Node* objectB);

    bool isPositionAvailable(const cocos2d::Rect&);
private:
    void initPlayers();
    void initMap();

    void calculateCollisions();
    Wall* createStandardWallAndAdd();

    KeyManager keyManager;
    Player* players[NUM_PLAYERS];
    std::list<Wall*> walls;

};

#endif // MENUSCENE_H_INCLUDED
