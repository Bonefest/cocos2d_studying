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
#include <string>

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

void HelloWorld::changeLabelStatus() {
    char text[127];
    sprintf(text,"Score:%d",size);
    info->setString(text);
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Scene::init() ) {
        return false;
    }
    timer = 0;
    size = 1;
    currentDir = UP;
    tail = new SnakePart(cocos2d::Sprite::create("HelloWorld.png"),true);
    tail->sprite->setContentSize(cocos2d::Size(32,32));
    tail->sprite->setPosition(16,16);
    if(tail->sprite == nullptr) {
        std::cout << "Error with creating a sprite!\n";
        return false;
    }
    tail->isHead = true;
    tail->next = nullptr;
    this->addChild(tail->sprite);

    info = cocos2d::Label::createWithTTF("Pos:(0,0)","fonts/arial.ttf",24);
    Size size = cocos2d::Director::getInstance()->getWinSize();

    if(info == nullptr) {
        std::cout << "Error with creating a label!\n";
        return false;
    }
    info->setPosition(cocos2d::Vec2(size.width/2,size.height - size.height/4));
    this->addChild(info,1);

    auto keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::snakeEvent,this);

    this->scheduleUpdate();

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener,this);
    return true;
}

void HelloWorld::snakeEvent(cocos2d::EventKeyboard::KeyCode keyCode,Event* event) {
    cocos2d::EventKeyboard* kevent = static_cast<EventKeyboard*>(event);
    if(cocos2d::EventKeyboard::KeyCode::KEY_A == keyCode && currentDir != RIGHT)
        currentDir = LEFT;
    else if(cocos2d::EventKeyboard::KeyCode::KEY_W == keyCode && currentDir != DOWN)
        currentDir = UP;
    else if(cocos2d::EventKeyboard::KeyCode::KEY_D == keyCode && currentDir != LEFT)
        currentDir = RIGHT;
    else if(cocos2d::EventKeyboard::KeyCode::KEY_S == keyCode && currentDir != UP)
        currentDir = DOWN;
    else if(cocos2d::EventKeyboard::KeyCode::KEY_R == keyCode)
        addPart();
}

void HelloWorld::addPart() {
    SnakePart* newPart = new SnakePart(Sprite::create("HelloWorld.png"),false);
    newPart->sprite->setContentSize(cocos2d::Size(32,32));
    newPart->sprite->setPosition(tail->sprite->getPosition());
    newPart->next = tail;
    tail = newPart;
    this->addChild(newPart->sprite);
    size++;
    changeLabelStatus();
}

void HelloWorld::moveSnake() {
    SnakePart* head;
    for(SnakePart* part = tail;part != nullptr;part = part->next) {
        if(part->next == nullptr) {
            auto position = part->sprite->getPosition();
            std::cerr << position.x << std::endl;
            switch(currentDir) {
            case UP:
                position.add(Vec2(0,32));
                part->sprite->setPosition(position);
                break;
            case RIGHT:
                position.add(Vec2(32,0));
                part->sprite->setPosition(position);
                break;
            case DOWN:
                position.add(Vec2(0,-32));
                part->sprite->setPosition(position);
                break;
            case LEFT:
                position.add(Vec2(-32,0));
                part->sprite->setPosition(position);
                break;
            }
            head = part;
        }
        else {
            auto nextPosition = part->next->sprite->getPosition();
            part->sprite->setPosition(nextPosition);
        }
    }
    auto headPos = head->sprite->getPosition();
    for(SnakePart* part=tail;part->next != nullptr;part=part->next) {
        auto partPos = part->sprite->getPosition();
        if(partPos == headPos) {
            menuCloseCallback(nullptr);
        }
    }
}

void HelloWorld::update(float fDelta) {
    timer += fDelta;
    if(timer >= 0.4) {
        timer = 0;

        moveSnake();
    }
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    SnakePart* temp;
    while(tail != nullptr) {
        temp = tail->next;
        delete tail;
        tail = temp;
    }
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
