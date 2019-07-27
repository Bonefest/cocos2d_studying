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

    initPlayers();
    initMap();

    this->scheduleUpdate();
    return true;
}

void MenuScene::initPlayers() {
    for(int i = 0;i<NUM_PLAYERS;++i) {
        players[i] = Player::createPlayer(GameConfigManager::getInstance()->getSpritePath("Player"));
        players[i]->setAnchorPoint(Vec2(0,0));
        this->addChild(players[i]);
    }

    players[0]->setPlayerKeys({cocos2d::EventKeyboard::KeyCode::KEY_A,
                               cocos2d::EventKeyboard::KeyCode::KEY_W,
                               cocos2d::EventKeyboard::KeyCode::KEY_D,
                               cocos2d::EventKeyboard::KeyCode::KEY_S,
                               cocos2d::EventKeyboard::KeyCode::KEY_SPACE,
                               cocos2d::EventKeyboard::KeyCode::KEY_E});

    players[0]->setPosition(Vec2(32,32));

}

void MenuScene::initMap() {
    Wall* newWall;
    for(size_t x = 0;x<MAP_WIDTH;++x) {
        newWall = createStandardWallAndAdd();
        newWall->setPosition(x*BLOCK_SIZE,0);

        newWall = createStandardWallAndAdd();
        newWall->setPosition(x*BLOCK_SIZE,(MAP_HEIGHT-1)*BLOCK_SIZE);
    }

    for(size_t y = 1;y<MAP_HEIGHT-1;++y) {
        newWall = createStandardWallAndAdd();
        newWall->setPosition(0,y*BLOCK_SIZE);

        newWall = createStandardWallAndAdd();
        newWall->setPosition( (MAP_WIDTH-1)*BLOCK_SIZE, y*BLOCK_SIZE);
    }

    for(size_t y = 2;y<MAP_HEIGHT-1;y+=2) {
        for(size_t x = 2;x<MAP_WIDTH-1;x+=2) {
            newWall = createStandardWallAndAdd();
            newWall->setPosition(x*BLOCK_SIZE,y*BLOCK_SIZE);
        }
    }
}

Wall* MenuScene::createStandardWallAndAdd() {
   auto newWall = Wall::createWall(GameConfigManager::getInstance()->getSpritePath("Wall"));
   newWall->setAnchorPoint(Vec2(0,0));
   this->addChild(newWall);
   walls.push_back(newWall);

   return newWall;
}

void MenuScene::update(float delta) {

    std::vector<Command*> commands(handleInput(delta));
    for(auto commandIter = commands.begin();commandIter != commands.end();commandIter++) {
        (*commandIter)->execute();
        delete *commandIter;
    }

    calculateCollisions();

}

std::vector<Command*> MenuScene::handleInput(float delta) {
    KeyConfig keys;
    cocos2d::Vec2 resultPlayerSpeed;
    bool keyPressed = false;
    std::vector<Command*> inputtedCommands;

    for(size_t i = 0;i<NUM_PLAYERS;++i) {
        keys=players[i]->getPlayerKeys();
        auto playerPosition = players[i]->getPosition();
        if(keyManager.isPressed(keys.keyLeft)) {
            inputtedCommands.push_back(new MoveByCommand(players[i],this,Vec2(-PLAYER_SPEED*delta,0)));
        }

        if(keyManager.isPressed(keys.keyRight)) {
            inputtedCommands.push_back(new MoveByCommand(players[i],this,Vec2(PLAYER_SPEED*delta,0)));
        }

        if(keyManager.isPressed(keys.keyUp)) {
            inputtedCommands.push_back(new MoveByCommand(players[i],this,Vec2(0,PLAYER_SPEED*delta)));
        }

        if(keyManager.isPressed(keys.keyDown)) {
            inputtedCommands.push_back(new MoveByCommand(players[i],this,Vec2(0,-PLAYER_SPEED*delta)));
        }


    }

    return inputtedCommands;
}

//Проблемка в том, что при коллизии скорость становится ноль и мы больше не сможем двигаться
void MenuScene::calculateCollisions() {
    for(size_t i = 0;i<NUM_PLAYERS;++i) {
        auto playerSpeed = players[i]->getSpeed();
        auto playerRect = players[i]->getBoundingBox();
        cocos2d::Rect wallRect;
        for(auto wallIter = walls.begin();wallIter != walls.end();wallIter++) {
            if(isCollide(players[i],*wallIter)) {
                wallRect = (*wallIter)->getBoundingBox();
                if(playerRect.getMaxX()-2 > wallRect.getMaxX() || playerRect.getMinX()+2 < wallRect.getMinX())
                    playerSpeed.x = 0;

                if(playerRect.getMaxY()-2 > wallRect.getMaxY() || playerRect.getMinY()+2 < wallRect.getMinY())
                    playerSpeed.y = 0;

            }
        }

        players[i]->setSpeed(playerSpeed);

    }
}

bool MenuScene::isCollide(cocos2d::Node* objectA,cocos2d::Node* objectB) {
    return objectA->getBoundingBox().intersectsRect(objectB->getBoundingBox());
}

bool MenuScene::isPositionAvailable(const cocos2d::Rect& rect) {
    for(auto wallIter = walls.begin();wallIter != walls.end();wallIter++) {
        if((*wallIter)->getBoundingBox().intersectsRect(rect))
            return false;
    }

    return true;
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
