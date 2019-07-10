#include "MenuScene.h"

USING_NS_CC;

Scene* MenuScene::createScene() {
    return MenuScene::create();
}

bool MenuScene::init() {
    if(!Scene::init())
        return false;

    auto screenSize = cocos2d::Director::getInstance()->getWinSize();

    scoreLabel = cocos2d::Label::createWithTTF("Record " + std::to_string(cocos2d::UserDefault::getInstance()->getIntegerForKey("Score")),"fonts/slkscr.ttf",36);
    scoreLabel->setPosition(screenSize.width/2,screenSize.height-scoreLabel->getContentSize().height);
    this->addChild(scoreLabel);

    startButton = cocos2d::ui::Button::create("CloseNormal.png","CloseSelected.png");
    startButton->setPosition(Vec2(screenSize.width/2,screenSize.height/2));
    startButton->setScale(2.0f);

    this->addChild(startButton);

    startButton->addTouchEventListener(CC_CALLBACK_0(MenuScene::startGame,this));

    return true;
}

void MenuScene::startGame() {
    cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(1.0f,HelloWorld::createScene()));
}

void MenuScene::menuCloseCallback(cocos2d::Ref* psender) {
    cocos2d::Director::getInstance()->end();
}
