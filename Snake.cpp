#include "Snake.h"
#include "RakPeerInterface.h"
#include "GetTime.h"

SnakePartReplica::SnakePartReplica(USER_TYPE type):ClientReplicaObject(type) { }

RakNet::RM3SerializationResult SnakePartReplica::Serialize(RakNet::SerializeParameters* parameters) {
    RakNet::VariableDeltaSerializer::SerializationContext context;
    parameters->pro[0].reliability = RELIABLE;
    parameters->pro[0].sendReceipt = replicaManager->GetRakPeerInterface()->IncrementNextSendReceipt();
    parameters->messageTimestamp = RakNet::GetTime();

    variableDeltaSerializer.BeginIdenticalSerialize(&context,(parameters->whenLastSerialized==0),&parameters->outputBitstream[0]);
    variableDeltaSerializer.SerializeVariable(&context,type);
    variableDeltaSerializer.SerializeVariable(&context,position.x);
    variableDeltaSerializer.SerializeVariable(&context,position.y);
    variableDeltaSerializer.SerializeVariable(&context,color);
    variableDeltaSerializer.EndSerialize(&context);

    return RakNet::RM3SR_BROADCAST_IDENTICALLY;
}

void SnakePartReplica::Deserialize(RakNet::DeserializeParameters* parameters) {
    RakNet::VariableDeltaSerializer::DeserializationContext context;

    variableDeltaSerializer.BeginDeserialize(&context,&parameters->serializationBitstream[0]);
    variableDeltaSerializer.DeserializeVariable(&context,type);
    variableDeltaSerializer.DeserializeVariable(&context,position.x);
    variableDeltaSerializer.DeserializeVariable(&context,position.y);
    variableDeltaSerializer.DeserializeVariable(&context,color);

    variableDeltaSerializer.EndDeserialize(&context);
}


