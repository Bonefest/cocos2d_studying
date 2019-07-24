#include "KeyManager.h"
#include <algorithm>

void KeyManager::onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event) {
    if(!isPressed(key)) {
        addNewKey(key);
    }
}
void KeyManager::onKeyReleased(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event) {
    removeKey(key);
}

bool KeyManager::isPressed(cocos2d::EventKeyboard::KeyCode key) const {
    return (std::find(pressedKeys.begin(),pressedKeys.end(),key) !=  pressedKeys.end());
}

void KeyManager::addNewKey(cocos2d::EventKeyboard::KeyCode key) {
    pressedKeys.push_back(key);
}


void KeyManager::removeKey(cocos2d::EventKeyboard::KeyCode key) {
    pressedKeys.remove(key);
}
