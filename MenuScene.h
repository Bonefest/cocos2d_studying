#ifndef MENUSCENE_H_INCLUDED
#define MENUSCENE_H_INCLUDED

#include "cocos2d.h"

class MenuScene : cocos2d::Scene {
public:
    virtual bool init();
    static Scene* createScene();

    CREATE_FUNC(MenuScene);
    void menuCloseCallback(cocos2d::Ref* psender);

    void update(float delta);


};

#endif // MENUSCENE_H_INCLUDED
