#ifndef GAMESCENE_H_INCLUDED
#define GAMESCENE_H_INCLUDED

#include "cocos2d.h"
#include "RakPeerInterface.h"
#include "Game.h"
#include "Snake.h"
#include "MessageIdentifiers.h"
#include <string>
#include <map>

enum GAME_STATUS {
    WAITING_FOR_PLAYERS,
    STARTED,
    FINISHED
};

enum GAME_MESSAGES {
    ID_CLIENT_CONNECTED_TO_ROOM = ID_USER_PACKET_ENUM+1,
    ID_ASK_FOR_NAME,
    ID_CLIENT_NAME,
    ID_CONNECTED_USERS_DATA,
    ID_SET_TEAMCOLOR_FOR_CLIENT,
    ID_GET_CONNECTED_COUNT,
    ID_GAME_START
};

class GameScene: public cocos2d::Scene {
public:
    virtual bool init();
    virtual void onExit();

    void menuCloseCallback(cocos2d::Ref* pSender);

    static GameScene* create(RakNet::RakPeerInterface* peer,USER_TYPE type,std::string name);

    void update(float delta);

private:
    void prepareScene();
    void onClientInitScene();
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);

    void readPackets();
    void updateUsersDataLabel();

    void onDisconnectMessage(RakNet::Packet* packet);

    void askForUserName(RakNet::AddressOrGUID address);
    void sendNameTo(RakNet::AddressOrGUID address);
    void readUserName(RakNet::Packet* packet);

    void setTeamColorToClient(RakNet::RakNetGUID guid);
    void readTeamColor(RakNet::Packet* packet);

    void sendUserdataAll();
    void readUserdata(RakNet::Packet* packet);

    void startGame();

    RakNet::RakPeerInterface* peer;
    RakNet::Packet* packet;

    USER_TYPE    userType;
    GAME_STATUS  status;
    TeamColor    teamColor;
    TeamColor    nextColor;

    cocos2d::Size   visibleSize;

    cocos2d::Label* connectedUsersDataLabel;


    std::map<RakNet::RakNetGUID,Player> players;

    std::string name;

};


#endif // GAMESCENE_H_INCLUDED
