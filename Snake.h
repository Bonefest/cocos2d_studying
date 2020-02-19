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

    void setDirection(Direction direction) { this->direction = direction; }
    Direction getDirection() { return direction; }
    void setPosition(cocos2d::Vec2 position) { this->position = position; }
    cocos2d::Vec2 getPosition() const { return position; }

    bool isHead() const { return head; }
    bool setHead(bool head) { this->head = head; }
private:
    USER_TYPE type;
    bool head;
    Direction direction;
    cocos2d::Vec2 position;
    TeamColor color;
};
class SnakePart: public cocos2d::Sprite {
public:
    static SnakePart* createPart(bool head);
    void setReplica(SnakePartReplica* _replica) { this->replica.SetCompositeOwner(_replica);}
    SnakePartReplica* getReplica() { replica.GetCompositeOwner(); }
    void setPosition(const cocos2d::Vec2& position) {
        cocos2d::Sprite::setPosition(position);
        replica.GetCompositeOwner()->setPosition(position);
    }

    void setDirection(Direction direction) { replica.GetCompositeOwner()->setDirection(direction); }
    Direction getDirection() { replica.GetCompositeOwner()->getDirection(); }
    bool isHead() { return getReplica()->isHead(); }
    bool setHead(bool head) { getReplica()->setHead(head); }
private:
    RakNet::Replica3Composite<SnakePartReplica> replica;
};

class Snake {
public:
    Snake(cocos2d::Scene* scene,RakNet::ReplicaManager3* manager,cocos2d::Vec2 startPosition,TeamColor color,USER_TYPE type);
    ~Snake() { removeFromScene(); }

    cocos2d::Vec2 getPosition() { return head->getReplica()->getPosition(); }

    void setDirection(Direction direction) { head->setDirection(direction); }
    Direction getDirection() const { return head->getDirection(); }

    void setColor(TeamColor color);
    void addPart(bool head=false);
    void setPosition(const cocos2d::Vec2& vec) { head->setPosition(vec); }
    void update(float delta);

    bool isIntersectsWith(const cocos2d::Rect& rect) const { return head->getBoundingBox().intersectsRect(rect); }
    bool isIntersectsSelf();
    void removeFromScene();

    size_t getSnakeLength() const { return parts.size(); }
    //cocos2d::Vec2 getHeadPosition();
private:
    cocos2d::Vec2 getNextPosition() const;
    void warpAndSetPosition(cocos2d::Vec2 position);

    RakNet::ReplicaManager3* _manager;
    cocos2d::Scene* _scene;
    SnakePart* head;
    TeamColor _color;
    USER_TYPE _type;

    float updateTimer; //if updateTime > 1.0f - update and rest

    std::list<SnakePart*> parts;
};

class AppleReplica: public ClientReplicaObject {
public:
    AppleReplica(USER_TYPE type);
    virtual RakNet::RakString getName() const { return "Apple"; }
    RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* parameters);
    void Deserialize(RakNet::DeserializeParameters* parameters);

    void setPosition(cocos2d::Vec2 position) { this->position = position; }
    cocos2d::Vec2 getPosition() const { return position; }
private:
    USER_TYPE type;
    cocos2d::Vec2 position;
};

class Apple: public cocos2d::Sprite {
public:
    static Apple* createApple();
    void setReplica(AppleReplica* replica) { this->replica = replica; }
    AppleReplica* getReplica() { return replica;}

    void setPosition(const cocos2d::Vec2& position) {
        cocos2d::Sprite::setPosition(position);
        replica->setPosition(position);
    }

private:
    AppleReplica* replica;
};

#endif // SNAKE_H_INCLUDED
