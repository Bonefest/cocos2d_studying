#ifndef KEYMANAGER_H_INCLUDED
#define KEYMANAGER_H_INCLUDED

#include "cocos2d.h"
#include <list>

class KeyManager {
public:
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);

    bool isPressed(cocos2d::EventKeyboard::KeyCode key) const;
private:
    std::list<cocos2d::EventKeyboard::KeyCode> pressedKeys;

    void addNewKey(cocos2d::EventKeyboard::KeyCode);
    void removeKey(cocos2d::EventKeyboard::KeyCode);
};


#endif // KEYMANAGER_H_INCLUDED
