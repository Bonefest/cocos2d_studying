#ifndef GAMESCENE_H_INCLUDED
#define GAMESCENE_H_INCLUDED

#include "cocos2d.h"
#include "RakPeerInterface.h"
#include "Game.h"
#include "Snake.h"
#include "MessageIdentifiers.h"
#include "NetworkIDManager.h"
#include "Replica.h"
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

class Snake;
class ReplicaManager;
class SnakePart;
class Apple;

class GameScene: public cocos2d::Scene {
public:
    virtual bool init();
    virtual void onExit();

    void menuCloseCallback(cocos2d::Ref* pSender);

    static GameScene* create(RakNet::RakPeerInterface* peer,USER_TYPE type,std::string name);

    virtual RakNet::Replica3* replicaFactory(RakNet::RakString type);
    void update(float delta);

    USER_TYPE getUserType() const { return userType; }

private:
    void prepareScene();
    void onClientInitScene();
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);

    void readPackets();
    void updateUsersDataLabel();

    void onDisconnectMessage(RakNet::Packet* packet);
    void onGameStartedMessage(RakNet::Packet* packet);

    void askForUserName(RakNet::AddressOrGUID address);
    void sendNameTo(RakNet::AddressOrGUID address);
    void readUserName(RakNet::Packet* packet);

    void setTeamColorToClient(RakNet::RakNetGUID guid);
    void readTeamColor(RakNet::Packet* packet);

    void sendUserdataAll();
    void readUserdata(RakNet::Packet* packet);

    void startGame();
    void initPlayersSnakes();
    void updatePlayersParts();
    void updateApplesPositions();
    void generateApple();

    void checkCollision();

    RakNet::RakPeerInterface* peer;
    RakNet::NetworkIDManager* networkManager;
    ReplicaManager* replicaManager;
    RakNet::Packet* packet;

    USER_TYPE    userType;
    GAME_STATUS  status;
    TeamColor    teamColor;
    TeamColor    nextColor;

    cocos2d::Size   visibleSize;

    cocos2d::Label* connectedUsersDataLabel;


    std::map<RakNet::RakNetGUID,Player> players;
    std::list<SnakePart*> playersParts;

    std::list<Apple*> sceneApples;
    float appleTimer;

    Snake* snake;
    std::string name;

};


#endif // GAMESCENE_H_INCLUDED
