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

