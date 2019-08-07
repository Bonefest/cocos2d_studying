#include "Entity.h"
#include "GameConfigManager.h"

Player* Player::createPlayer(const std::string& path) {
    Player* newPlayer = new Player;
    if(newPlayer->initWithFile(path)) {
        newPlayer->initPlayer();
        return newPlayer;
    }

    CC_SAFE_DELETE(newPlayer);
    return nullptr;
}

void Player::initPlayer() {
    bombsAvailable = bombsMaxCount = 3;
    bombsRange = 4;
}

void Player::update(float delta) {

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


Bomb* Bomb::createBomb(const std::string& path,int range) {
    Bomb* bomb = new Bomb;
    if(bomb->initWithFile(path)) {
        bomb->autorelease();
        bomb->setRange(range);
        bomb->explositionTime = 1.0f;
        return bomb;
    }
    CC_SAFE_DELETE(bomb);
    return nullptr;
}

Explosion* Explosion::createExplosion(const std::string& path) {
    Explosion* explosion = new Explosion;
    if(explosion->initWithFile(path)) {
        return explosion;
    }

    CC_SAFE_DELETE(explosion);
    return nullptr;
}

void Explosion::finish() {
    this->removeFromParent();
}

#include <iostream>
BonusBox* BonusBox::createBonusBox(BONUS_TYPE type) {
    BonusBox* bonusBox = new BonusBox;
    if(bonusBox->initWithFile(getImagePath(type,true))) {
        bonusBox->autorelease();
        bonusBox->_type = type;
        return bonusBox;
    }
    CC_SAFE_DELETE(bonusBox);
    return nullptr;
}

Bonus* Bonus::createBonus(BONUS_TYPE type) {
    Bonus* bonus = new Bonus;
    if(bonus->initWithFile(getImagePath(type,false))) {
        bonus->autorelease();
        bonus->_type = type;
        return bonus;
    }
    CC_SAFE_DELETE(bonus);
    return nullptr;
}


void Bonus::improvePlayer(Player* player) {
    switch(_type) {
    case BONUS_RANGE:
        player->setBombsRange(player->getBombsRange()+1);
        break;
    case BONUS_SPEED:
        player->setSpeed(player->getSpeed()+20);
        break;
    }
}

std::string getImagePath(BONUS_TYPE type,bool isBox) {
    auto configInstance = GameConfigManager::getInstance();
    switch(type) {
    case BONUS_RANGE: return (isBox)?configInstance->getSpritePath("BonusBoxRange"):configInstance->getSpritePath("BonusRange");
    case BONUS_SPEED: return (isBox)?configInstance->getSpritePath("BonusBoxSpeed"):configInstance->getSpritePath("BonusSpeed");
    case BONUS_BOMB : return (isBox)?configInstance->getSpritePath("BonusBoxBomb"):configInstance->getSpritePath("BonusBomb");
    }
    return configInstance->getSpritePath("None");
}
