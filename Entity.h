#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "cocos2d.h"

struct KeyConfig {
    cocos2d::EventKeyboard::KeyCode keyLeft;
    cocos2d::EventKeyboard::KeyCode keyUp;
    cocos2d::EventKeyboard::KeyCode keyRight;
    cocos2d::EventKeyboard::KeyCode keyDown;
    cocos2d::EventKeyboard::KeyCode keyPlace;
    cocos2d::EventKeyboard::KeyCode keyUse;
};



class Player: public cocos2d::Sprite {
public:
    static Player* createPlayer(const std::string& path);

    void update(float delta);

    void setSpeed(float speed) { _speed = speed; }
    float getSpeed() const { return _speed; }
    KeyConfig getPlayerKeys() const { return playerKeys; }
    void setPlayerKeys(KeyConfig keys) { playerKeys = keys; }

    void setBombsRange(int range) { bombsRange = range; }

    int getAvailableBombsCount() const { return bombsAvailable; }
    int getBombsRange() const { return bombsRange; }
    int getBombsMaxCount() const { return bombsMaxCount; }

private:
    KeyConfig playerKeys;
    float _speed;

    int bombsAvailable;
    int bombsRange;
    int bombsMaxCount;

    void initPlayer();
};


class Wall: public cocos2d::Sprite {
public:
    static Wall* createWall(const std::string& path);

};

class Bomb: public cocos2d::Sprite {
public:
    static Bomb* createBomb(const std::string& path,int range);
    void setRange(int explosionRange) { range=explosionRange; }
    int getRange() const { return range; }
    float getExplosionTime() const { return explositionTime; }
private:
    int range;
    float explositionTime;
};

class Explosion: public cocos2d::Sprite {
public:
    static Explosion* createExplosion(const std::string& path);
    void finish();
};

enum BONUS_TYPE { BONUS_RANGE=0,BONUS_SPEED,BONUS_BOMB,BONUS_MAX };

class BonusBox: public cocos2d::Sprite {
public:
    static BonusBox* createBonusBox(BONUS_TYPE type);

    BONUS_TYPE getBonusType() const { return _type; }
private:

    BONUS_TYPE _type;
};

class Bonus: public cocos2d::Sprite {
public:
    static Bonus* createBonus(BONUS_TYPE type);

    void improvePlayer(Player* player);
private:
    BONUS_TYPE _type;
};


std::string getImagePath(BONUS_TYPE type,bool isBox);


#endif // ENTITY_H_INCLUDED
