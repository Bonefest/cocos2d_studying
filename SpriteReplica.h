#ifndef SPRITEREPLICA_H_INCLUDED
#define SPRITEREPLICA_H_INCLUDED

#include "cocos2d.h"
#include <RakPeerInterface.h>
#include "ReplicaManager3.h"
#include "VariableDeltaSerializer.h"
#include "GetTime.h"
#include "NetworkIDManager.h"
#include "MessageIdentifiers.h"
#include "DS_List.h"
#include "RakSleep.h"



class SpriteReplica : public RakNet::Replica3 {
public:
    SpriteReplica(cocos2d::Sprite* spr,bool srvr=false):sprite(spr),server(srvr) { }

    virtual RakNet::RakString getName() const { return "Sprite"; }

    void WriteAllocationID(RakNet::Connection_RM3* connection,RakNet::BitStream* allocationIdBitstream) const {
        allocationIdBitstream->Write(getName());
    }

    void DeallocReplica(RakNet::Connection_RM3* connection) {
        delete this;
    }

    virtual void SerializeConstruction(RakNet::BitStream* constructionBitstream,RakNet::Connection_RM3* connection) {
        variableDeltaSerializer.AddRemoteSystemVariableHistory(connection->GetRakNetGUID());
    }

    virtual bool DeserializeConstruction(RakNet::BitStream* constructionBitstream,RakNet::Connection_RM3* connection) {
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

    virtual RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* parameters) {
        RakNet::VariableDeltaSerializer::SerializationContext context;
        parameters->pro[0].reliability = UNRELIABLE;
        parameters->pro[0].sendReceipt = replicaManager->GetRakPeerInterface()->IncrementNextSendReceipt();
        parameters->messageTimestamp = RakNet::GetTime();

        variableDeltaSerializer.BeginIdenticalSerialize(&context,(parameters->whenLastSerialized==0),&parameters->outputBitstream[0]);

        position = sprite->getPosition();
        contentSize = sprite->getContentSize();

        variableDeltaSerializer.SerializeVariable(&context,position.x);
        variableDeltaSerializer.SerializeVariable(&context,position.y);
        variableDeltaSerializer.SerializeVariable(&context,contentSize.width);
        variableDeltaSerializer.SerializeVariable(&context,contentSize.height);

        variableDeltaSerializer.EndSerialize(&context);

        return RakNet::RM3SR_BROADCAST_IDENTICALLY;
    }

    virtual void Deserialize(RakNet::DeserializeParameters* parameters) {
        RakNet::VariableDeltaSerializer::DeserializationContext context;

        variableDeltaSerializer.BeginDeserialize(&context,&parameters->serializationBitstream[0]);

        cocos2d::Color3B color = sprite->getColor();
        float rotation=0;
        std::string spritePath=sprite->getTexture()->getPath();

        variableDeltaSerializer.DeserializeVariable(&context,position.x);
        variableDeltaSerializer.DeserializeVariable(&context,position.y);
        variableDeltaSerializer.DeserializeVariable(&context,contentSize.width);
        variableDeltaSerializer.DeserializeVariable(&context,contentSize.height);
        sprite->setPosition(position);
        sprite->setContentSize(contentSize);

        variableDeltaSerializer.EndDeserialize(&context);
    }

    void OnPoppedConnection(RakNet::Connection_RM3* connection) {
        variableDeltaSerializer.RemoveRemoteSystemVariableHistory(connection->GetRakNetGUID());
    }

    RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3* connection,RakNet::ReplicaManager3* rmanager) {
        return QueryConstruction_ClientConstruction(connection,isServer());
    }

    bool QueryRemoteConstruction(RakNet::Connection_RM3* connection) {
        return QueryRemoteConstruction_ClientConstruction(connection,isServer());
    }

    RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3* connection) {
        return QuerySerialization_ClientSerializable(connection,isServer());
    }

    RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3* connection) const {
        return QueryActionOnPopConnection_Client(connection);
    }


    bool isServer() const { return server; }
protected:
    RakNet::VariableDeltaSerializer variableDeltaSerializer;

private:
    bool server;
    cocos2d::Sprite* sprite;
    cocos2d::Vec2 position;
    cocos2d::Size contentSize;
};

#endif // SPRITEREPLICA_H_INCLUDED
