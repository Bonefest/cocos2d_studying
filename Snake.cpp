#include "Snake.h"
#include "RakPeerInterface.h"
#include "GetTime.h"
#include <iostream>
SnakePartReplica::SnakePartReplica(USER_TYPE type):ClientReplicaObject(type) { }

RakNet::RM3SerializationResult SnakePartReplica::Serialize(RakNet::SerializeParameters* parameters) {
    RakNet::VariableDeltaSerializer::SerializationContext context;
    parameters->pro[0].reliability = UNRELIABLE;
    parameters->pro[0].sendReceipt = replicaManager->GetRakPeerInterface()->IncrementNextSendReceipt();
    parameters->messageTimestamp = RakNet::GetTime();

    variableDeltaSerializer.BeginIdenticalSerialize(&context,(parameters->whenLastSerialized==0),&parameters->outputBitstream[0]);
    variableDeltaSerializer.SerializeVariable(&context,position.x);
    variableDeltaSerializer.SerializeVariable(&context,position.y);
    //std::cout << position.x << " " << position.y << std::endl;
    variableDeltaSerializer.SerializeVariable(&context,direction);
    variableDeltaSerializer.SerializeVariable(&context,color);
    variableDeltaSerializer.EndSerialize(&context);

    return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void SnakePartReplica::Deserialize(RakNet::DeserializeParameters* parameters) {
    RakNet::VariableDeltaSerializer::DeserializationContext context;

    variableDeltaSerializer.BeginDeserialize(&context,&parameters->serializationBitstream[0]);
    variableDeltaSerializer.DeserializeVariable(&context,position.x);
    variableDeltaSerializer.DeserializeVariable(&context,position.y);
    variableDeltaSerializer.DeserializeVariable(&context,direction);
    variableDeltaSerializer.DeserializeVariable(&context,color);
    //std::cout << position.x << " " << position.y << std::endl;
    variableDeltaSerializer.EndDeserialize(&context);
}

AppleReplica::AppleReplica(USER_TYPE type):ClientReplicaObject(type) { }

RakNet::RM3SerializationResult AppleReplica::Serialize(RakNet::SerializeParameters* parameters) {
    RakNet::VariableDeltaSerializer::SerializationContext context;
    parameters->pro[0].reliability = UNRELIABLE;
    parameters->pro[0].sendReceipt = replicaManager->GetRakPeerInterface()->IncrementNextSendReceipt();
    parameters->messageTimestamp = RakNet::GetTime();

    variableDeltaSerializer.BeginIdenticalSerialize(&context,(parameters->whenLastSerialized==0),&parameters->outputBitstream[0]);
    variableDeltaSerializer.SerializeVariable(&context,position.x);
    variableDeltaSerializer.SerializeVariable(&context,position.y);
    variableDeltaSerializer.EndSerialize(&context);

    return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void AppleReplica::Deserialize(RakNet::DeserializeParameters* parameters) {
    RakNet::VariableDeltaSerializer::DeserializationContext context;

    variableDeltaSerializer.BeginDeserialize(&context,&parameters->serializationBitstream[0]);
    variableDeltaSerializer.DeserializeVariable(&context,position.x);
    variableDeltaSerializer.DeserializeVariable(&context,position.y);
    variableDeltaSerializer.EndDeserialize(&context);
}


///////////////////////////////////////////////////////////////////////////////////////////

SnakePart* SnakePart::createPart(bool head) {
    SnakePart* part = new SnakePart;
    if(part->initWithFile("brick.png")) {
        part->autorelease();
        part->head = head;
        return part;
    }
    CC_SAFE_DELETE(part);
    return nullptr;
}

Apple* Apple::createApple() {
    Apple* apple = new Apple;
    if(apple->initWithFile("brick.png")) {
        apple->autorelease();
        return apple;
    }
    CC_SAFE_DELETE(apple);
    return nullptr;
}


///////////////////////////////////////////////////////////////////////////////////////////
Snake::Snake(cocos2d::Scene* scene,RakNet::ReplicaManager3* manager,cocos2d::Vec2 startPosition,TeamColor color,USER_TYPE type):_scene(scene),_manager(manager),_color(color),_type(type),updateTimer(0.0f) {
    addPart(true);
    head = *parts.begin();
    head->setPosition(cocos2d::Vec2(100,100));
    setDirection(RIGHT);
    for(int i = 0;i<5;++i)
        addPart(false);
}

void Snake::addPart(bool head) {
    SnakePart* part = SnakePart::createPart(head);

    parts.push_back(part);
    SnakePartReplica* partReplica = new SnakePartReplica(_type);
    part->setReplica(partReplica);
    _manager->Reference(partReplica);
    _scene->addChild(part);

}

void Snake::update(float delta) {
    updateTimer += delta;
    if(updateTimer > MOVE_TIMER) {
        Direction prevDirection;
        cocos2d::Vec2 previousPartPosition = head->getPosition();
        head->setPosition(previousPartPosition + getNextPosition());

        for(auto partIter = (++parts.begin());partIter != parts.end();partIter++) {
            cocos2d::Vec2 tempPosition = (*partIter)->getPosition();
            (*partIter)->setPosition(previousPartPosition);
            previousPartPosition = tempPosition;
        }

        updateTimer = 0.0f;
    }
}


cocos2d::Vec2 Snake::getNextPosition() const {
    switch(getDirection()) {
    case LEFT :return (cocos2d::Vec2(-SNAKE_SPEED,0));
    case UP   :return (cocos2d::Vec2(0,SNAKE_SPEED));
    case RIGHT:return (cocos2d::Vec2(SNAKE_SPEED,0));
    case DOWN :return (cocos2d::Vec2(0,-SNAKE_SPEED));
    }

    return cocos2d::Vec2::ZERO;
}

