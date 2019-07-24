#include "MenuScene.h"
#include "GameConfigManager.h"

USING_NS_CC;
#include <iostream>

Scene* MenuScene::createScene() {
   return MenuScene::create();
}

bool MenuScene::init() {
    if(!Scene::init())
        return false;

    auto keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(MenuScene::onKeyPressed,this);
    keyListener->onKeyReleased = CC_CALLBACK_2(MenuScene::onKeyReleased,this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener,this);


    this->scheduleUpdate();
    return true;
}

void MenuScene::initPlayers() {
    for(int i = 0;i<NUM_PLAYERS;++i) {
        Player* player = Player::createPlayer(GameConfigManager::getInstance()->getSpritePath("Player"));
        this->addChild(player);
    }

}

void MenuScene::update(float delta) {
    Command* command = handleInput(delta);
    command->execute();
    delete command;

}

Command* MenuScene::handleInput(float delta) {
    KeyConfig keys;
    cocos2d::Vec2 resultPlayerSpeed;
    bool keyPressed = false;

    for(size_t i = 0;i<NUM_PLAYERS;++i) {
        keys=players[i]->getPlayerKeys();
        resultPlayerSpeed = players[0]->getSpeed();

        if(keyManager.isPressed(keys.keyLeft)) {
            resultPlayerSpeed.x = -PLAYER_SPEED;
            keyPressed = true;
        } else if(keyManager.isPressed(keys.keyRight)) {
            resultPlayerSpeed.x = PLAYER_SPEED;
            keyPressed = true;
        } else if(keyManager.isPressed(keys.keyUp)) {
            resultPlayerSpeed.y = PLAYER_SPEED;
            keyPressed = true;
        } else if(keyManager.isPressed(keys.keyDown)) {
            resultPlayerSpeed.y = -PLAYER_SPEED;
            keyPressed = true;
        }

        if(keyPressed) {
            return new NewSpeedCommand(players[i],resultPlayerSpeed);
        }
    }

    return new NullCommand;
}

void MenuScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event) {
    keyManager.onKeyPressed(key,event);

}
void MenuScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event) {
    keyManager.onKeyReleased(key,event);

}


void MenuScene::menuCloseCallback(cocos2d::Ref* psender) {

    cocos2d::Director::getInstance()->end();
}
