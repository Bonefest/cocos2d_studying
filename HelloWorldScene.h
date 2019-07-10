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

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <iostream>
#include <list>
#include <RakPeerInterface.h>

const int TRACKS = 3;
const double SPAWN_TIME = 1.5;
const double BLOCK_SIZE = 64;
const double LEVEL_WAIT = 2.0f;
const double LEVEL_TIME = 10.0f;
const int COUNTERS = 3;

class Block: public cocos2d::Node {
public:
    static Block* createBlock(double speed,cocos2d::Color3B color = cocos2d::Color3B::WHITE);

    void updateBlock(float delta);

    bool isOut() const;
    bool isClicked(cocos2d::Point point) const;

    double getSpeed() const { return speed; }

    bool isFinished() const { return finished; }
    bool isHiding() const { return hiding; }
    void setHiding(bool t) { hiding = t; }

    void onFadeFinish() { finished = true; }
private:
    bool finished;
    bool hiding;
    double speed;



};

class HelloWorld : public cocos2d::Scene
{
public:
    virtual bool init();

    void draw(cocos2d::Renderer* render,const cocos2d::Mat4& mat,uint32_t t);

    static Scene* createScene();
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    void update(float delta);

    void onClickBegin(cocos2d::Event* event);
    void onTouchBegin(const std::vector<cocos2d::Touch*>& touches,cocos2d::Event* event);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
    RakNet::RakPeerInterface* peer;

    cocos2d::Color4F clearColor;
    cocos2d::Color4F speedColor;

    int status;

    double currentSpeed;

    double currentSpawnTime;
    double spawnTime;

    double currentLevelTime;
    double currentColorTime;

    int level;
    int score;

    cocos2d::Label* scoreLabel;
    cocos2d::Label* message;

    std::list<Block*> activatedBlocks;

    void showBigMessage(double time,const std::string& message,bool fade=true);

    void spawnBlock();
    void removeBlock(Block* blockPtr);
    void levelChanger();
    void colorChanger();
    void gameOver();

    void prepareToPlay();

    bool changeColor;
    bool spawnNew;
    bool gameActive;

    cocos2d::DrawNode* drawer;
};

#endif // __HELLOWORLD_SCENE_H__
