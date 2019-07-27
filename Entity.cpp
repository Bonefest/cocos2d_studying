#include "Entity.h"

Player* Player::createPlayer(const std::string& path) {
    Player* newPlayer = new Player;
    if(newPlayer->initWithFile(path)) {
        newPlayer->autorelease();

        return newPlayer;
    }

    CC_SAFE_DELETE(newPlayer);
    return nullptr;
}


void Player::update(float delta) {
    auto position = getPosition();
    auto speed = getSpeed();
    position.add(delta*speed);
    setPosition(position);
}

void Player::setSpeed(const cocos2d::Vec2& newSpeed) {
    speed=newSpeed;
}


Wall* Wall::createWall(const std::string& path) {
    Wall* result = new Wall;
    if(result->initWithFile(path)) {
        result->autorelease();
        return result;
    }
    CC_SAFE_DELETE(result);
    return nullptr;
}
