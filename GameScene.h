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
    ID_SET_TEAMCOLOR_FOR_CLIENT
};

class GameScene: public cocos2d::Scene {
public:
    virtual bool init();
    virtual void onExit();

    void menuCloseCallback(cocos2d::Ref* pSender);

    static GameScene* create(RakNet::RakPeerInterface* peer,USER_TYPE type,std::string name);

    void update(float delta);

private:
    RakNet::RakPeerInterface* peer;
    RakNet::Packet* packet;

    void onClientInitScene();
    void askForUserName(RakNet::AddressOrGUID address);
    void sendNameTo(RakNet::AddressOrGUID address);
    void readUserName(RakNet::Packet* packet);
    void setTeamColorToClient(RakNet::RakNetGUID guid);
    void readTeamColor(RakNet::Packet* packet);

    USER_TYPE    userType;
    GAME_STATUS  status;
    TeamColor    teamColor;
    TeamColor    nextColor;

    std::map<RakNet::RakNetGUID,Player> players;

    std::string name;

};


#endif // GAMESCENE_H_INCLUDED
