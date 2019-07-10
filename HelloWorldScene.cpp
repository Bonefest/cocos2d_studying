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
#include "MenuScene.h"
#include <RakPeerInterface.h>

#include <string>

USING_NS_CC;


Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

Block* Block::createBlock(double speed,cocos2d::Color3B color) {
    Block* block = new Block;
    if(block->init()) {
        block->autorelease();
        block->speed = speed;
        block->setColor(color);
        block->finished = false;
        block->hiding = false;

        return block;
    }

    CC_SAFE_DELETE(block);
    return nullptr;
}

bool Block::isClicked(cocos2d::Point point) const {
    return this->getBoundingBox().containsPoint(point);
}

void Block::updateBlock(float delta) {
    auto pos = this->getPosition();
    pos.add(Vec2(0,-speed*delta));
    setPosition(pos);
}

bool Block::isOut() const {
    auto pos = Block::getPosition();
    return pos.y < -BLOCK_SIZE*2;
}

void HelloWorld::spawnBlock() {
    cocos2d::Size screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    float shift = screenSize.width/TRACKS;
    int track = cocos2d::random(0,TRACKS-1);
    std::cout << "Track: " << track << std::endl;

    Block* newBlock = Block::createBlock(currentSpeed,cocos2d::Color3B(random(0,254),random(0,254),random(0,254)));
    newBlock->setPosition(Vec2(shift*track + shift/2 - BLOCK_SIZE/2,screenSize.height));
    newBlock->setContentSize(cocos2d::Size(BLOCK_SIZE,BLOCK_SIZE));
    newBlock->setOpacity(254);
    this->addChild(newBlock);

    activatedBlocks.push_back(newBlock);
}

void HelloWorld::levelChanger() {
    if(spawnTime > 0.4)
        spawnTime -= 0.1;
    speedColor.r = random(0.0f,1.0f) - clearColor.r;
    speedColor.g = random(0.0f,1.0f) - clearColor.g;
    speedColor.b = random(0.0f,1.0f) - clearColor.b;
    speedColor.a = 0.0f;

    currentSpeed += 20;
    changeColor = true;
    spawnNew = true;

    this->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(1.0f),cocos2d::CallFunc::create(CC_CALLBACK_0(HelloWorld::colorChanger,this)),nullptr));
    showBigMessage(0.4f,std::to_string(++level));
}

void HelloWorld::colorChanger() {
    changeColor = false;
}

void HelloWorld::showBigMessage(double time,const std::string& msg,bool fade) {
    message->setOpacity(255);
    message->setScale(0.1f);
    message->setString(msg);

    if(fade)
        message->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(time,2.0f),cocos2d::FadeOut::create(0.5f),nullptr));
    else
        message->runAction(cocos2d::Sequence::create(cocos2d::ScaleTo::create(time,2.0f),nullptr));
}


void HelloWorld::prepareToPlay() {
    showBigMessage(0.4f,std::to_string(++status));
    if(status >= COUNTERS) {
        gameActive = true;
    } else {
        this->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(1.0f),cocos2d::CallFunc::create(CC_CALLBACK_0(HelloWorld::prepareToPlay,this)),nullptr));
    }
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Scene::init() ) {
        return false;
    }

    spawnTime = SPAWN_TIME;
    currentSpawnTime = 0;
    currentLevelTime = 0;
    currentSpeed = 100;
    level = 1;
    status = 0;

    spawnNew = true;
    changeColor = false;
    gameActive = false;

    auto scrSize = cocos2d::Director::getInstance()->getWinSize();

    scoreLabel = cocos2d::Label::createWithTTF("Score: 0","fonts/slkscr.ttf",26);
    scoreLabel->setPosition(Vec2(scrSize.width/2,scrSize.height - scoreLabel->getContentSize().height));
    this->addChild(scoreLabel,1);

    message = cocos2d::Label::createWithTTF("Test","fonts/slkscr.ttf",48);
    message->setPosition(Vec2(scrSize.width/2,scrSize.height/2));
    this->addChild(message,2);

    drawer = cocos2d::DrawNode::create();
    this->addChild(drawer);

    auto listener = cocos2d::EventListenerMouse::create();
    listener->onMouseDown = CC_CALLBACK_1(HelloWorld::onClickBegin,this);

    auto toucher = cocos2d::EventListenerTouchAllAtOnce::create();
    toucher->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchBegin,this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(toucher,this);

    prepareToPlay();

    spawnBlock();

    this->scheduleUpdate();

    return true;
}

void HelloWorld::onClickBegin(cocos2d::Event* event) {
    cocos2d::EventMouse* mevent = static_cast<EventMouse*>(event);
    float mouseX = mevent->getCursorX();
    float mouseY = mevent->getCursorY();

    for(auto iter = activatedBlocks.begin();iter!=activatedBlocks.end();iter++)  {


        if( gameActive && (*iter)->isClicked(cocos2d::Point(mouseX,mouseY)) && !(*iter)->isHiding()) {
            auto seq = cocos2d::Sequence::create(cocos2d::FadeOut::create(0.5f),cocos2d::CallFunc::create(CC_CALLBACK_0(Block::onFadeFinish,*iter)),nullptr);
            (*iter)->runAction(seq);
            score++;
            (*iter)->setHiding(true);

            scoreLabel->setString("Score: " + std::to_string(score));
        }
    }
}


void HelloWorld::onTouchBegin(const std::vector<cocos2d::Touch*>& touches,cocos2d::Event* event) {
    for(auto touchIter = touches.begin();touchIter != touches.end();touchIter++) {

        auto touchPos = (*touchIter)->getLocation();

        for(auto iter = activatedBlocks.begin();iter!=activatedBlocks.end();iter++)  {


            if( gameActive && (*iter)->isClicked(cocos2d::Point(touchPos.x,touchPos.y)) && !(*iter)->isHiding()) {
                auto seq = cocos2d::Sequence::create(cocos2d::FadeOut::create(0.5f),cocos2d::CallFunc::create(CC_CALLBACK_0(Block::onFadeFinish,*iter)),nullptr);
                (*iter)->runAction(seq);
                score++;
                (*iter)->setHiding(true);

                scoreLabel->setString("Score: " + std::to_string(score));
            }
        }
    }
}

void HelloWorld::gameOver() {
    int lastScore = cocos2d::UserDefault::getInstance()->getIntegerForKey("Score");
    if(score > lastScore) cocos2d::UserDefault::getInstance()->setIntegerForKey("Score",score);
    cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(1.0f,MenuScene::createScene()));
}

void HelloWorld::update(float fDelta) {
    if(gameActive) {
        currentSpawnTime += fDelta;
        currentLevelTime += fDelta;

        cocos2d::Director::getInstance()->setClearColor(clearColor);

        if(changeColor) {
            clearColor.r += speedColor.r*fDelta;
            clearColor.g += speedColor.g*fDelta;
            clearColor.b += speedColor.b*fDelta;
        }

        if(currentSpawnTime > spawnTime && spawnNew) {
            spawnBlock();
            currentSpawnTime = 0;
        }
        if(currentLevelTime > LEVEL_TIME + LEVEL_WAIT) {
            levelChanger();
            currentLevelTime = 0;
        } else if(currentLevelTime > LEVEL_TIME) {
            spawnNew = false;
        }


        for(auto iter = activatedBlocks.begin();iter!=activatedBlocks.end();) {
            //std::cout << (*iter)->getPosition().x << std::endl;
            if((*iter)->isOut() && !(*iter)->isHiding()) {
                gameActive = false;
                spawnNew = false;
                showBigMessage(0.5,"You\nlose!",false);
                this->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(1.0f),cocos2d::CallFunc::create(CC_CALLBACK_0(HelloWorld::gameOver,this)),nullptr));
            }

            if((*iter)->isFinished()) {
                this->removeChild(*iter);
                //(*iter)->release();
                iter = activatedBlocks.erase(iter);
            }
            else {
                (*iter)->updateBlock(fDelta);
                iter++;
            }

        }
    }
}

void HelloWorld::draw(cocos2d::Renderer* render,const cocos2d::Mat4& mat,uint32_t t) {
    drawer->clear();

    for(auto iter = activatedBlocks.begin();iter!=activatedBlocks.end();iter++) {
        auto pos = (*iter)->getPosition();
        auto des = pos;
        des.add(Vec2(BLOCK_SIZE,BLOCK_SIZE));
        drawer->drawSolidRect(pos,des,cocos2d::Color4F((*iter)->getColor(),(*iter)->getOpacity()/255.0f));
    }
    Scene::draw(render,mat,t);

}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
