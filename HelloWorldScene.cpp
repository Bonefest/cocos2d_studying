/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#include <cstdlib>
#include <iostream>
#include "MessageIdentifiers.h"
#include "GameScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene() {
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init() {
    if ( !Scene::init() ) {
        return false;
    }
    visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    peer = RakNet::RakPeerInterface::GetInstance();

    userType = CLIENT;
    initUI();

    sceneChanging = false;
    scheduleUpdate();
    return true;
}
#include "RakSleep.h"

void HelloWorld::update(float delta) {
    if(userType == CLIENT && peer->GetConnectionState(RakNet::SystemAddress(ip.c_str(),port)) == RakNet::ConnectionState::IS_CONNECTED && !sceneChanging)
        changeToGameScene();
}

void HelloWorld::initUI() {
    nicknameField = cocos2d::ui::TextField::create("Nickname","fonts/arial.ttf",32);
    nicknameField->setPosition(Vec2(visibleSize.width/2,5*visibleSize.height/8));
    this->addChild(nicknameField);

    addressField = cocos2d::ui::TextField::create("127.0.0.1:9999","fonts/arial.ttf",32);
    addressField->setPosition(Vec2(visibleSize.width/2,4*visibleSize.height/8));
    this->addChild(addressField);

    userTypeButton = cocos2d::ui::Button::create();
    userTypeButton->setPosition(Vec2(visibleSize.width/2,3*visibleSize.height/8));
    userTypeButton->addTouchEventListener(CC_CALLBACK_2(HelloWorld::onUserTypeButtonActioned,this));
    userTypeButton->setTitleFontSize(32);
    this->addChild(userTypeButton);

    connectButton = cocos2d::ui::Button::create();
    connectButton->setPosition(Vec2(visibleSize.width/2,2*visibleSize.height/8));
    connectButton->setTitleText("Connect");
    connectButton->addTouchEventListener(CC_CALLBACK_2(HelloWorld::onConnectButtonActioned,this));
    connectButton->setTitleFontSize(32);
    this->addChild(connectButton);

    updateUserTypeButton();
}


void HelloWorld::onUserTypeButtonActioned(cocos2d::Ref* ref,cocos2d::ui::Widget::TouchEventType type) {
    if(type == cocos2d::ui::Widget::TouchEventType::BEGAN) {
        userType = USER_TYPE((int(userType) + 1)%2);
        updateUserTypeButton();
    }
}

void HelloWorld::onConnectButtonActioned(cocos2d::Ref* ref,cocos2d::ui::Widget::TouchEventType type) {
    if(type == cocos2d::ui::Widget::TouchEventType::BEGAN) {
        if(nicknameField->getString() != "") {
            parseAddress(addressField->getString());
            if(userType == CLIENT) attemptConnectAsClient();
            else if(userType == SERVER) attemptConnectAsServer();
        }
    }
}

void HelloWorld::parseAddress(const std::string& address) {
    size_t pos = address.find(':');
    if(pos == std::string::npos) {
        ip = "127.0.0.1";
        port = 9999;
        return;
    }

    ip = address.substr(0,pos);
    port = atoi(address.substr(pos+1).c_str());
}

void HelloWorld::attemptConnectAsClient() {
    RakNet::SocketDescriptor sdesc;
    peer->Startup(1,&sdesc,1);
    peer->Connect(ip.c_str(),port,0,0);
    peer->SetMaximumIncomingConnections(1);
}

void HelloWorld::attemptConnectAsServer() {
    RakNet::SocketDescriptor sdesc(port,0);
    peer->SetMaximumIncomingConnections(MAX_CONNECTIONS);
    if(peer->Startup(MAX_CONNECTIONS,&sdesc,1) == RakNet::StartupResult::RAKNET_STARTED)
        changeToGameScene();
}

void HelloWorld::changeToGameScene() {
    std::cout << "Change !\n";
    Scene* scene = GameScene::create(peer,userType,nicknameField->getString());
    cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(1.0f,scene));
    sceneChanging = true;
}


void HelloWorld::updateUserTypeButton() {
    const char* text;
    cocos2d::Color3B color;

    switch(userType) {
    case SERVER:text="Server";color=cocos2d::Color3B::RED;break;
    case CLIENT:text="Client";color=cocos2d::Color3B::GREEN;break;
    }

    userTypeButton->setTitleColor(color);
    userTypeButton->setTitleText(text);
}

void HelloWorld::menuCloseCallback(Ref* pSender) {
    Director::getInstance()->end();
}
