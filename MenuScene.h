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
static const short PORT = 34000;
static const char* IP = "127.0.0.1";

class Player;
class MenuScene;

static bool isServer = false;

class PlayerReplica : public RakNet::Replica3 {
public:
    PlayerReplica():position(cocos2d::Vec2::ZERO),health(0) {}

    void setPosition(const cocos2d::Vec2& pos) { position = pos; }
    cocos2d::Vec2 getPosition() { return position; }

    void WriteAllocationID(RakNet::Connection_RM3* connection,RakNet::BitStream* allocationIdBitstream) const {
        allocationIdBitstream->Write("Player");
    }

    void DeallocReplica(RakNet::Connection_RM3* connection) {
        delete this;
    }

    void SerializeConstruction(RakNet::BitStream* constructionBitstream,RakNet::Connection_RM3* connection) {
        variableDeltaSerializer.AddRemoteSystemVariableHistory(connection->GetRakNetGUID());
    }

    bool DeserializeConstruction(RakNet::BitStream* constructionBitstream,RakNet::Connection_RM3* connection) {
        return true;
    }

    void SerializeDestruction(RakNet::BitStream* destructionBitstream,RakNet::Connection_RM3* connection) {
        variableDeltaSerializer.RemoveRemoteSystemVariableHistory(connection->GetRakNetGUID());
    }

    bool DeserializeDestruction(RakNet::BitStream* destructionBitstream,RakNet::Connection_RM3* connection) {
        return true;
    }

    void OnUserReplicaPreSerializeTick() {
        variableDeltaSerializer.OnPreSerializeTick();
    }

    RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* parameters) {
        RakNet::VariableDeltaSerializer::SerializationContext context;
        parameters->pro[0].reliability = UNRELIABLE;
        parameters->pro[0].sendReceipt = replicaManager->GetRakPeerInterface()->IncrementNextSendReceipt();
        parameters->messageTimestamp = RakNet::GetTime();

        variableDeltaSerializer.BeginIdenticalSerialize(&context,(parameters->whenLastSerialized==0),&parameters->outputBitstream[0]);
        variableDeltaSerializer.SerializeVariable(&context,position.x);
        variableDeltaSerializer.SerializeVariable(&context,position.y);
        variableDeltaSerializer.EndSerialize(&context);

        return RakNet::RM3SR_BROADCAST_IDENTICALLY;
    }

    void Deserialize(RakNet::DeserializeParameters* parameters) {
        RakNet::VariableDeltaSerializer::DeserializationContext context;

        variableDeltaSerializer.BeginDeserialize(&context,&parameters->serializationBitstream[0]);
        variableDeltaSerializer.DeserializeVariable(&context,position.x);
        variableDeltaSerializer.DeserializeVariable(&context,position.y);
        variableDeltaSerializer.EndDeserialize(&context);
    }

    void OnPoppedConnection(RakNet::Connection_RM3* connection) {
        variableDeltaSerializer.RemoveRemoteSystemVariableHistory(connection->GetRakNetGUID());
    }

    RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3* connection,RakNet::ReplicaManager3* rmanager) {
        return QueryConstruction_ClientConstruction(connection,isServer);
    }

    bool QueryRemoteConstruction(RakNet::Connection_RM3* connection) {
        return QueryRemoteConstruction_ClientConstruction(connection,isServer);
    }

    RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3* connection) {
        return QuerySerialization_ClientSerializable(connection,isServer);
    }

    RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3* connection) const {
        return QueryActionOnPopConnection_Client(connection);
    }

private:
    cocos2d::Vec2 position;
    int health;

    RakNet::VariableDeltaSerializer variableDeltaSerializer;
};


class Player : public cocos2d::Sprite {
public:
    static Player* createPlayer(PlayerReplica* playerReplica);
    void setPosition(const cocos2d::Vec2& position) {
        cocos2d::Sprite::setPosition(position);
        replica.GetCompositeOwner()->setPosition(position);
    }

    void updateState(float delta);
private:
    RakNet::Replica3Composite<PlayerReplica> replica;

};


class ReplicaManager;

class MenuScene : cocos2d::Scene {
public:
    virtual bool init();
    static Scene* createScene();

    CREATE_FUNC(MenuScene);
    void menuCloseCallback(cocos2d::Ref* psender);

    void onConnectNewUser(PlayerReplica* playerReplica);

    void update(float delta);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);
private:
    bool typeChoosen;

    RakNet::NetworkIDManager nmanager;
    RakNet::RakPeerInterface* peer;
    ReplicaManager* rmanager;

    std::vector<Player*> connectedPlayers;
    Player* myPlayer;

    RakNet::Packet* packet;

    bool connected;
};


class SampleConnection : public RakNet::Connection_RM3 {
public:
    SampleConnection(const RakNet::SystemAddress& address,RakNet::RakNetGUID _guid,MenuScene* menuScene):Connection_RM3(address,_guid),scene(menuScene) { }

    RakNet::Replica3* AllocReplica(RakNet::BitStream* bitstream,RakNet::ReplicaManager3* rmanager) {
        RakNet::RakString type;
        bitstream->Read(type);
        std::cout << "new user!\n";
        if(type == "Player") {

        std::cout << "it's a player!\n";
            PlayerReplica* playerReplica = new PlayerReplica;
            scene->onConnectNewUser(playerReplica);
            return playerReplica;
        }

        return nullptr;
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
