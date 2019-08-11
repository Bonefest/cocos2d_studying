#include "GameScene.h"
#include "BitStream.h"
#include "RakString.h"

#include <iostream>
GameScene* GameScene::create(RakNet::RakPeerInterface* peer,USER_TYPE type,std::string name) {
    GameScene* scene = new GameScene;
    scene->peer = peer;
    scene->userType = type;
    scene->name = name;

    if(scene->init()) {
        scene->autorelease();
        return scene;
    }
    CC_SAFE_DELETE(scene);
    return nullptr;
}

void GameScene::onExit() {
    RakNet::RakPeerInterface::DestroyInstance(peer);
    Scene::onExit();
    //deleting...
}

bool GameScene::init() {
    if(!Scene::init()) {
        return false;
    }
    prepareScene();

    if(userType == CLIENT);
        onClientInitScene();

    auto keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed,this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener);
    scheduleUpdate();
    return true;
}

void GameScene::prepareScene() {
    status = WAITING_FOR_PLAYERS;
    teamColor = RED;
    nextColor = GREEN;

    visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    connectedUsersDataLabel = cocos2d::Label::create("Label","fonts/arial.ttf",32);
    connectedUsersDataLabel->setPosition(7*visibleSize.width/10,9*visibleSize.height/10);
    connectedUsersDataLabel->setAnchorPoint(cocos2d::Vec2(0.0f,1.0f));
    this->addChild(connectedUsersDataLabel);

    players[peer->GetMyGUID()].playerName = name.c_str();
    updateUsersDataLabel();

    //generateSnakeHere
}

void GameScene::onClientInitScene() {
    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_CLIENT_CONNECTED_TO_ROOM);
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
}

void GameScene::update(float delta) {
    readPackets();
    if(players.size() == MAX_CONNECTIONS+1 && userType == SERVER && status == WAITING_FOR_PLAYERS) {
        startGame();
    }
}

void GameScene::startGame() {
    status = STARTED;

    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_GAME_START);
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,false);
}

void GameScene::readPackets() {
    for(packet=peer->Receive();packet;peer->DeallocatePacket(packet),packet=peer->Receive()) {
        switch(packet->data[0]) {

        //SERVER MESSAGES
        case ID_DISCONNECTION_NOTIFICATION:
        case ID_CONNECTION_LOST:
            onDisconnectMessage(packet);
            break;

        case ID_CLIENT_CONNECTED_TO_ROOM:
            players[packet->guid].playerName = RakNet::RakString("");
            askForUserName(packet->guid);
            break;
        case ID_CLIENT_NAME:
            readUserName(packet);
            setTeamColorToClient(packet->guid);
            sendUserdataAll();
            updateUsersDataLabel();
            break;

        //CLIENT MESSAGES
        case ID_ASK_FOR_NAME:
            sendNameTo(packet->guid);
            break;
        case ID_SET_TEAMCOLOR_FOR_CLIENT:
            readTeamColor(packet);
            break;
        case ID_CONNECTED_USERS_DATA:
            readUserdata(packet);
            updateUsersDataLabel();
            break;

        }
    }
}

void GameScene::onDisconnectMessage(RakNet::Packet* packet) {
    switch(userType) {
        case SERVER: {
            players.erase(packet->guid);
            sendUserdataAll();
            updateUsersDataLabel();
            break;
        }
        case CLIENT: {
            break;
        }
    }
}

void GameScene::askForUserName(RakNet::AddressOrGUID address) {
    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_ASK_FOR_NAME);
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,address,false);
}

void GameScene::readUserName(RakNet::Packet* packet) {
    RakNet::RakString userName;
    RakNet::BitStream streamIn(packet->data,packet->length,false);
    streamIn.IgnoreBytes(sizeof(RakNet::MessageID));
    streamIn.Read(userName);
    players[packet->guid].playerName = userName;
}

void GameScene::sendNameTo(RakNet::AddressOrGUID address) {
    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_CLIENT_NAME);
    streamOut.Write(RakNet::RakString(name.c_str()));
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,address,false);
}

void GameScene::setTeamColorToClient(RakNet::RakNetGUID guid) {
    players[guid].color = nextColor;
    nextColor = TeamColor(int(nextColor) + 1);

    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_SET_TEAMCOLOR_FOR_CLIENT);
    streamOut.Write(players[guid].color);
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,guid,false);
}

void GameScene::readTeamColor(RakNet::Packet* packet) {
    RakNet::BitStream streamIn(packet->data,packet->length,false);
    streamIn.IgnoreBytes(sizeof(RakNet::MessageID));
    streamIn.Read(teamColor);
}

void GameScene::sendUserdataAll() {
    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_CONNECTED_USERS_DATA);
    streamOut.Write(players.size());
    for(auto mapIter = players.begin();mapIter != players.end(); mapIter++) {
        streamOut.Write(mapIter->first);
        streamOut.Write(mapIter->second.playerName);
    }

    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
}

void GameScene::readUserdata(RakNet::Packet* packet) {
    RakNet::BitStream streamIn(packet->data,packet->length,false);
    streamIn.IgnoreBytes(sizeof(RakNet::MessageID));
    players.clear();

    size_t playersCount;
    streamIn.Read(playersCount);

    RakNet::RakNetGUID lastGuid;
    Player lastPlayer;


    for(size_t i = 0;i<playersCount;++i) {
        streamIn.Read(lastGuid);
        streamIn.Read(lastPlayer.playerName);
        players[lastGuid] = lastPlayer;
    }

}

void GameScene::updateUsersDataLabel() {
    std::string usersData = std::to_string(players.size()) + "/" + std::to_string(MAX_CONNECTIONS+1) + "\n";

    for(auto playerIter = players.begin();playerIter != players.end();playerIter++) {
        usersData += std::string(playerIter->second.playerName.C_String()) + "\n";
    }

    connectedUsersDataLabel->setString(usersData);
}
