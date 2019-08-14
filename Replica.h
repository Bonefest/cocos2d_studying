#ifndef REPLICA_H_INCLUDED
#define REPLICA_H_INCLUDED

#include "ReplicaManager3.h"
#include "VariableDeltaSerializer.h"
#include "BitStream.h"
#include "Game.h"

class ClientReplicaObject: public RakNet::Replica3 {
public:
    ClientReplicaObject(USER_TYPE type);
    virtual RakNet::RakString getName() const=0;
    virtual void WriteAllocationID(RakNet::Connection_RM3* destConnection,RakNet::BitStream* allocationBitStream) const;
    virtual void DeallocReplica(RakNet::Connection_RM3* sourceConnection);
    virtual void SerializeConstruction(RakNet::BitStream* stream,RakNet::Connection_RM3* connection);
    virtual bool DeserializeConstruction(RakNet::BitStream* stream,RakNet::Connection_RM3* connection);
    virtual void SerializeDestruction(RakNet::BitStream* stream,RakNet::Connection_RM3* connection);
    virtual bool DeserializeDestruction(RakNet::BitStream* stream,RakNet::Connection_RM3* connection);
    virtual void OnUserReplicaPreSerializeTick();
    virtual RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* parameters)=0;
    virtual void Deserialize(RakNet::DeserializeParameters* parameters)=0;
    virtual void OnPoppedConnection(RakNet::Connection_RM3* connection);

    virtual RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3* connection,RakNet::ReplicaManager3* manager) { return QueryConstruction_ClientConstruction(connection,isServer()); }
    virtual bool QueryRemoteConstruction(RakNet::Connection_RM3* connection) { return QueryRemoteConstruction_ClientConstruction(connection,isServer()); }
    virtual RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3* connection) { return QuerySerialization_ClientSerializable(connection,isServer()); }
    virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3* connection) const { return QueryActionOnPopConnection_Client(connection); }
    bool isServer() const { return type == SERVER; }
protected:
    RakNet::VariableDeltaSerializer variableDeltaSerializer;
private:
    USER_TYPE type;
};

#include "GameScene.h"


class GameScene;

class UserConnection : public RakNet::Connection_RM3 {
public:
    UserConnection(const RakNet::SystemAddress& address,RakNet::RakNetGUID guid,GameScene* scene);
    RakNet::Replica3* AllocReplica(RakNet::BitStream* stream,RakNet::ReplicaManager3* manager);
private:
    GameScene* _scene;
};

class ReplicaManager: public RakNet::ReplicaManager3 {
public:
    //ReplicaManager(GameScene* scene):ReplicaManager3(),_scene(scene) { }
    void setScene(GameScene* scene) { _scene = scene; }

private:
    RakNet::Connection_RM3* AllocConnection(const RakNet::SystemAddress& address,RakNet::RakNetGUID guid) const;
    void DeallocConnection(RakNet::Connection_RM3* connection) const;

    GameScene* _scene;
};

#endif // REPLICA_H_INCLUDED
