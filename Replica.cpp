#include "Replica.h"
#include "RakPeerInterface.h"
#include "GetTime.h"

ClientReplicaObject::ClientReplicaObject(USER_TYPE tpe):type(tpe) { }

void ClientReplicaObject::WriteAllocationID(RakNet::Connection_RM3* destConnection,RakNet::BitStream* stream) const {
    stream->Write(getName());
}

void ClientReplicaObject::DeallocReplica(RakNet::Connection_RM3* connection) {
    delete this;
}

void ClientReplicaObject::SerializeConstruction(RakNet::BitStream* stream,RakNet::Connection_RM3* connection) {
    variableDeltaSerializer.AddRemoteSystemVariableHistory(connection->GetRakNetGUID());
}

bool ClientReplicaObject::DeserializeConstruction(RakNet::BitStream* constructionBitstream,RakNet::Connection_RM3* connection) {
    return true;
}

void ClientReplicaObject::SerializeDestruction(RakNet::BitStream* stream,RakNet::Connection_RM3* connection) {
    variableDeltaSerializer.RemoveRemoteSystemVariableHistory(connection->GetRakNetGUID());
}

bool ClientReplicaObject::DeserializeDestruction(RakNet::BitStream* constructionBistream,RakNet::Connection_RM3* connection) {
    return true;
}

void ClientReplicaObject::OnUserReplicaPreSerializeTick() {
    variableDeltaSerializer.OnPreSerializeTick();
}

void ClientReplicaObject::OnPoppedConnection(RakNet::Connection_RM3* connection) {
    variableDeltaSerializer.RemoveRemoteSystemVariableHistory(connection->GetRakNetGUID());
}

///////////////////////////////////////////////////////////////////////////////////////


UserConnection::UserConnection(const RakNet::SystemAddress& address,RakNet::RakNetGUID guid,GameScene* scene):Connection_RM3(address,guid),_scene(scene) { }

RakNet::Replica3* UserConnection::AllocReplica(RakNet::BitStream* stream,RakNet::ReplicaManager3* manager) {
    RakNet::RakString objectType;
    stream->Read(objectType);
    if(objectType == "SnakePart") {
        return _scene->replicaFactory(objectType);
    }

    return nullptr;
}

RakNet::Connection_RM3* ReplicaManager::AllocConnection(const RakNet::SystemAddress& address,RakNet::RakNetGUID guid) const {
    return new UserConnection(address,guid,_scene);
}

void ReplicaManager::DeallocConnection(RakNet::Connection_RM3* connection) const {
    delete connection;
}
