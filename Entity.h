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

    void setSpeed(const cocos2d::Vec2& newSpeed);
    const cocos2d::Vec2& getSpeed() const { return speed; }

    KeyConfig getPlayerKeys() const { return playerKeys; }
    void setPlayerKeys(KeyConfig keys) { playerKeys = keys; }
private:
    KeyConfig playerKeys;
    cocos2d::Vec2 speed;
};



#endif // ENTITY_H_INCLUDED
