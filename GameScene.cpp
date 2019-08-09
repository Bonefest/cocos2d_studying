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
    status = WAITING_FOR_PLAYERS;
    teamColor = RED;
    nextColor = GREEN;

    if(userType == CLIENT);
        onClientInitScene();

    scheduleUpdate();
    return true;
}

void GameScene::onClientInitScene() {
    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_CLIENT_CONNECTED_TO_ROOM);
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
}

void GameScene::update(float delta) {
    for(packet=peer->Receive();packet;peer->DeallocatePacket(packet),packet=peer->Receive()) {
        switch(packet->data[0]) {

        //SERVER MESSAGES
        case ID_CLIENT_CONNECTED_TO_ROOM:
            players[packet->guid].playerName = RakNet::RakString("");
            askForUserName(packet->guid);
            break;
        case ID_CLIENT_NAME:
            readUserName(packet);
            setTeamColorToClient(packet->guid);
            break;

        //CLIENT MESSAGES
        case ID_ASK_FOR_NAME:
            sendNameTo(packet->guid);
            break;
        case ID_SET_TEAMCOLOR_FOR_CLIENT:
            readTeamColor(packet);
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
