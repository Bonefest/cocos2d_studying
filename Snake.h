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

    void setPosition(cocos2d::Vec2 position);
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
    SnakePartReplica* getReplica();
    bool isHead() const;
private:
    RakNet::Replica3Composite<SnakePartReplica> replica;
};

class Snake {
public:
    Snake(cocos2d::Vec2 startPosition,TeamColor);
    void setDirection(Direction direction);
    void setColor(TeamColor color);
    void addPart();
    void update(float delta);
private:
    std::list<SnakePart*> parts;
    SnakePart* head;
    TeamColor _color;
    Direction _direction;
};


#endif // SNAKE_H_INCLUDED
