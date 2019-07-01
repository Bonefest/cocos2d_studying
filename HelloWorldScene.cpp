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

void DragSprite::update(float delta) {
    if(!dragged) {
        auto pos = getPosition();
        pos += speed*delta*3;
        setPosition(pos);
    }
}

void DragSprite::onClick(cocos2d::Event* event) {
    cocos2d::EventMouse* mevent = static_cast<cocos2d::EventMouse*>(event);
    auto mousePos = cocos2d::Vec2(mevent->getCursorX(),mevent->getCursorY());
    if(isClicked(mousePos)) {
        dragged = true;
        auto spritePos = this->getPosition();

        offset = mousePos-spritePos;
        speed = Vec2::ZERO;
    }
}

void DragSprite::onDrop(cocos2d::Event* event) {
    dragged = false;
}

void DragSprite::onMove(cocos2d::Event* event) {
    if(dragged) {
        cocos2d::EventMouse* mevent = static_cast<cocos2d::EventMouse*>(event);
        auto mousePos = cocos2d::Vec2(mevent->getCursorX(),mevent->getCursorY());
        speed = mousePos - lastCurPos;
        lastCurPos = mousePos;

        mousePos -= offset;
        this->setPosition(mousePos);
    }
}

bool DragSprite::isClicked(cocos2d::Vec2 mousePos) {
    return this->getBoundingBox().containsPoint(mousePos);
}

DragSprite* DragSprite::createWithFile(const std::string& fileName) {
    DragSprite* dragSprite = new DragSprite;
    if(dragSprite->initWithFile(fileName)) {
        dragSprite->autorelease();
        dragSprite->dragged = false;
        dragSprite->offset = cocos2d::Vec2::ZERO;
        dragSprite->speed = cocos2d::Vec2::ZERO;
        dragSprite->lastCurPos = cocos2d::Vec2::ZERO;

        auto mouseListener = cocos2d::EventListenerMouse::create();
        mouseListener->onMouseDown = CC_CALLBACK_1(DragSprite::onClick,dragSprite);
        mouseListener->onMouseUp = CC_CALLBACK_1(DragSprite::onDrop,dragSprite);
        mouseListener->onMouseMove = CC_CALLBACK_1(DragSprite::onMove,dragSprite);

        dragSprite->_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener,dragSprite);
        return dragSprite;
    }
    CC_SAFE_DELETE(dragSprite);
    return nullptr;
}

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Scene::init() ) {
        return false;
    }

    sprite = DragSprite::createWithFile("CloseNormal.png");
    sprite->setPosition(Vec2(200,200));

    this->addChild(sprite);

    this->scheduleUpdate();

    return true;
}

void HelloWorld::update(float delta) {
    sprite->update(delta);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
