#ifndef MENUSCENE_H_INCLUDED
#define MENUSCENE_H_INCLUDED

#include "cocos2d.h"
#include "KeyManager.h"
#include "Entity.h"
#include "Command.h"

const int NUM_PLAYERS = 4;
const float PLAYER_SPEED = 20;

const int MAP_WIDTH = 13;
const int MAP_HEIGHT = 15;

class MenuScene : cocos2d::Scene {
public:
    virtual bool init();
    static Scene* createScene();

    CREATE_FUNC(MenuScene);
    void menuCloseCallback(cocos2d::Ref* psender);

    void update(float delta);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);

    void initPlayers();
    void initMap();

    Command* handleInput(float delta);

    void calculateCollisions();

private:
    KeyManager keyManager;
    Player* players[NUM_PLAYERS];

};

#endif // MENUSCENE_H_INCLUDED
