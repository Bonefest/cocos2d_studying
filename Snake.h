#ifndef SNAKE_H_INCLUDED
#define SNAKE_H_INCLUDED

#include <list>

#include "Replica.h"
#include "Game.h"
#include "cocos2d.h"



class SnakePartReplica: public ClientReplicaObject {
public:
    SnakePartReplica(USER_TYPE type);
    virtual RakNet::RakString getName() const { return "SnakePart"; }
    RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* parameters);
    void Deserialize(RakNet::DeserializeParameters* parameters);

    void setPosition(cocos2d::Vec2 position) { this->position = position; }
    cocos2d::Vec2 getPosition() const { return position; }
private:
    USER_TYPE type;
    cocos2d::Vec2 position;
    TeamColor color;

    RakNet::VariableDeltaSerializer variableDeltaSerializer;
};
class SnakePart: public cocos2d::Sprite {
public:
    static SnakePart* createPart(bool head);
    void setReplica(SnakePartReplica* replica) { this->replica.SetCompositeOwner(replica);}
    SnakePartReplica* getReplica() { replica.GetCompositeOwner(); }
    void setPosition(const cocos2d::Vec2& position) {
        cocos2d::Sprite::setPosition(position);
        replica.GetCompositeOwner()->setPosition(position);
    }

    bool isHead() const { return head; }
private:
    RakNet::Replica3Composite<SnakePartReplica> replica;
    bool head;
};

class Snake {
public:
    Snake(cocos2d::Scene* scene,RakNet::ReplicaManager3* manager,cocos2d::Vec2 startPosition,TeamColor color,USER_TYPE type);

    void setDirection(Direction direction);
    void setColor(TeamColor color);
    void addPart(bool head=false);
    void update(float delta);

    //cocos2d::Vec2 getHeadPosition();
private:
    cocos2d::Vec2 getNextPosition() const;

    RakNet::ReplicaManager3* _manager;
    cocos2d::Scene* _scene;
    SnakePart* head;
    TeamColor _color;
    USER_TYPE _type;
    Direction _direction;

    float updateTimer; //if updateTime > 1.0f - update and rest

    std::list<SnakePart*> parts;
};


#endif // SNAKE_H_INCLUDED
