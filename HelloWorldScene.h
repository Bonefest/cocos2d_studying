/*

С целью документации будет представлен набор различных тестов.
Каждый тест содержит как можно больше деталей, которые помогут быстрее вспомнить
в будущем,а так же изучить RakNet в действии.

На данной сцене игрокам необходимо выбрать роль: клиент,сервер или пир.Ввести
желаемый ник,а так же ввести адрес сервера при необходимости.После чего
необходимо выбрать желаемый тест.

На данный момент предоставляется 6 тестов:
*Базовая репликация - демонстрируется репликация персонажа: позиции,текстуры,здоровья.
*Репликация на основе комманд - демонстрируется репликация игроков с помощью передачи
команд серверу.В данном подходе все решения принимает сервер.
*Чат - игроки получают возможность подключаться к чату и свободно в нём общаться.
Данная демонстрация не включает репликацию как таковую.
*peer-to-peer - показывется репликация с применением указаной топологии
*Поиск серверов в локальной сети - демонстрируется вывод всех доступных для подключения
серверов


*/


#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Game.h"
#include "ui/UIButton.h"
#include "ui/UITextField.h"
#include "ui/UIScrollView.h"

#include "RakPeerInterface.h"

class HelloWorld : public cocos2d::Scene
{
public:
    virtual bool init();

    static Scene* createScene();
    void menuCloseCallback(cocos2d::Ref* pSender);

    CREATE_FUNC(HelloWorld);

    void update(float delta);
private:
    void initUI();
    void updateUserTypeButton();
    void onUserTypeButtonActioned(cocos2d::Ref* ref,cocos2d::ui::Widget::TouchEventType type);
    void onConnectButtonActioned(cocos2d::Ref* ref,cocos2d::ui::Widget::TouchEventType type);
    void attemptConnectAsClient();
    void attemptConnectAsServer();
    void parseAddress(const std::string& address);
    void changeToGameScene();

    cocos2d::Size visibleSize;

    cocos2d::ui::TextField* nicknameField;
    cocos2d::ui::TextField* addressField;
    cocos2d::ui::Button* userTypeButton;
    cocos2d::ui::Button* connectButton;
    USER_TYPE userType;

    RakNet::RakPeerInterface* peer;
    std::string ip;
    unsigned int port;

    bool sceneChanging;
};

#endif // __HELLOWORLD_SCENE_H__
