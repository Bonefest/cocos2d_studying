#ifndef MENUSCENE_H_INCLUDED
#define MENUSCENE_H_INCLUDED

#include "cocos2d.h"
#include <vector>
#include <iostream>

#include <RakPeerInterface.h>
#include "ReplicaManager3.h"
#include "VariableDeltaSerializer.h"
#include "GetTime.h"
#include "NetworkIDManager.h"
#include "MessageIdentifiers.h"
#include "DS_List.h"
#include "RakSleep.h"

#include "SpriteReplica.h"
#include "ui/UIButton.h"

static const short PORT = 34000;
static const char* IP = "127.0.0.1";

static const int SPEED_BALL = 150;
static const int SPEED_PLAYER = 100;

enum GameMessages {
    ID_READY_START_MESSAGE = ID_USER_PACKET_ENUM + 1,
    ID_NEW_SCORE_MESSAGE
};

#include <algorithm>
class KeyManager {
public:
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);

    bool isPressed(cocos2d::EventKeyboard::KeyCode key) const { return std::find(pressedKeys.begin(),pressedKeys.end(),key)!=pressedKeys.end(); }

private:
    std::list<cocos2d::EventKeyboard::KeyCode> pressedKeys;
};

class Command {
public:
    virtual void execute()=0;
};

class NullCommand: public Command {
public:
    virtual void execute() { }
};

class MoveCommand: public Command {
public:
    MoveCommand(cocos2d::Sprite* sprite,const cocos2d::Vec2& position):target(sprite),newPosition(position) { }

    void execute() {
        target->setPosition(newPosition);
    }
private:
    cocos2d::Sprite* target;
    cocos2d::Vec2 newPosition;
};

class SpriteSerializable: public cocos2d::Sprite {
public:
    static SpriteSerializable* createWithFile(const std::string& file) {
        SpriteSerializable* result = new SpriteSerializable;
        if(result->initWithFile(file)) {
            result->autorelease();
            return result;
        }

        CC_SAFE_DELETE(result);
        return nullptr;
    }
    void setReplica(SpriteReplica* spriteReplica) {
        replica.SetCompositeOwner(spriteReplica);
    }

    RakNet::Replica3* getReplica() { return replica.GetCompositeOwner(); }
private:
    RakNet::Replica3Composite<SpriteReplica> replica;
};

class ReplicaManager;

class MenuScene : cocos2d::Scene {
public:
    virtual bool init();
    static Scene* createScene();

    CREATE_FUNC(MenuScene);
    void menuCloseCallback(cocos2d::Ref* psender);

    RakNet::Replica3* replicaFactory(const RakNet::RakString& str);

    void update(float delta);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);
    Command* handleInput(float delta);

    //void onConnectedAsClient();
private:
    RakNet::NetworkIDManager  nmanager;
    RakNet::RakPeerInterface* peer;
    RakNet::Packet* packet;
    ReplicaManager* rmanager;

    cocos2d::ui::Button* serverButton;
    cocos2d::ui::Button* clientButton;
    cocos2d::ui::Button* readyButton;

    cocos2d::Label* serverScoreLabel;
    uint64_t serverScore;

    cocos2d::Label* clientScoreLabel;
    uint64_t clientScore;

    SpriteSerializable* player;
    SpriteSerializable* ball;

    std::vector<cocos2d::Node*> physicsObjects;

    RakNet::RakNetGUID remoteGUID;

    KeyManager keyManager;

    cocos2d::Vec2 ballDirection;

    bool connected;
    bool ready;

    void onClientChoosen(cocos2d::Ref* sender,cocos2d::ui::Widget::TouchEventType type);
    void onServerChoosen(cocos2d::Ref* sender,cocos2d::ui::Widget::TouchEventType type);
    void onReadyChoosen (cocos2d::Ref* sender,cocos2d::ui::Widget::TouchEventType type);

    void initServerPart();
    void initScenePart();
    void initServerBall();
    void initGame();
    void initPlayer();

    void checkBallOut();

    void updateScoreStatus();

    void startServer();

    void updateBall(float delta);

};


class SampleConnection : public RakNet::Connection_RM3 {
public:
    SampleConnection(const RakNet::SystemAddress& address,RakNet::RakNetGUID _guid,MenuScene* menuScene):Connection_RM3(address,_guid),scene(menuScene) { }

    RakNet::Replica3* AllocReplica(RakNet::BitStream* bitstream,RakNet::ReplicaManager3* rmanager) {
        RakNet::RakString type;
        bitstream->Read(type);

        return scene->replicaFactory(type);
    }
private:
    MenuScene* scene;
};


class ReplicaManager: public RakNet::ReplicaManager3 {
    RakNet::Connection_RM3* AllocConnection(const RakNet::SystemAddress& address,RakNet::RakNetGUID rguid) const {
        return new SampleConnection(address,rguid,scene);
    }

    void DeallocConnection(RakNet::Connection_RM3* connection) const {
        delete connection;
    }
public:
    void setScene(MenuScene* scene) { this->scene = scene; }

private:
    MenuScene* scene;
};

#endif // MENUSCENE_H_INCLUDED
