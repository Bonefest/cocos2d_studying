#ifndef MENUSCENE_H_INCLUDED
#define MENUSCENE_H_INCLUDED

#include "cocos2d.h"
#include "ui/UIButton.h"
#include "HelloWorldScene.h"


class MenuScene : cocos2d::Scene {
public:
    virtual bool init();
    static Scene* createScene();

    CREATE_FUNC(MenuScene);

private:
    cocos2d::Label* scoreLabel;
    cocos2d::ui::Button* startButton;

    void menuCloseCallback(cocos2d::Ref* psender);
    void startGame();

};

#endif // MENUSCENE_H_INCLUDED
