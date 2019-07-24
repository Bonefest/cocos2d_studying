#include "MenuScene.h"

USING_NS_CC;
#include <iostream>
#include <cstdlib>

#include <BitStream.h>
#include <RakNetTypes.h>

bool MenuScene::init() {
    if(!Scene::init())
        return false;

    return true;
}
void MenuScene::menuCloseCallback(cocos2d::Ref* psender) {

    RakNet::RakPeerInterface::DestroyInstance(peer);
    cocos2d::Director::getInstance()->end();
}
