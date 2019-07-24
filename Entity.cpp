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
}

void Player::setSpeed(const cocos2d::Vec2& newSpeed) {
    speed=newSpeed;
}
