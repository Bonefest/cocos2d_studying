#ifndef MENUSCENE_H_INCLUDED
#define MENUSCENE_H_INCLUDED

#include "cocos2d.h"
#include "ui/UIButton.h"
#include "ui/UITextField.h"
#include "RakPeerInterface.h"
#include <MessageIdentifiers.h>

#include <deque>

const int MAX_CONNECTIONS = 10;

enum USER_MESSAGES {
    TEXT_MESSAGE=ID_USER_PACKET_ENUM+1
};

class MessageQueue : public cocos2d::Label {
public:
    static MessageQueue* createBoxTTF(const std::string& path,int fontSize,int maxMessages);
    void addNewMessage(const std::string& message);


private:
    int maxMessages;

    std::deque<std::string> messagesQueue;

    void reorganize();                      //Преобразовывает строку добавляя новый текст в начало и отрезая конец
};

class MenuScene : cocos2d::Scene {
public:
    virtual bool init();
    static Scene* createScene();

    CREATE_FUNC(MenuScene);
    void menuCloseCallback(cocos2d::Ref* psender);

    void update(float delta);

private:
    RakNet::RakPeerInterface*   peer;
    RakNet::Packet*             packet;
    RakNet::SystemAddress       serverAddress;

    bool isServer;
    bool connected,connecting;

    cocos2d::ui::TextField* addressField;
    cocos2d::ui::TextField* portField;
    cocos2d::ui::TextField* messageField;
    cocos2d::ui::Button*    connectButton;
    cocos2d::Label*         messageLabel;
    cocos2d::Label*         ipLabel;
    cocos2d::Label*         portLabel;
    MessageQueue*           messageQueue;

    std::string message;

    void startConnection(cocos2d::Ref* pSender,cocos2d::ui::Widget::TouchEventType type);
};

#endif // MENUSCENE_H_INCLUDED
