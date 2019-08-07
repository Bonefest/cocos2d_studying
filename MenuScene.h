#ifndef MENUSCENE_H_INCLUDED
#define MENUSCENE_H_INCLUDED

#include "cocos2d.h"
#include "KeyManager.h"
#include "Entity.h"
#include "Command.h"

class Command;

#include <list>
#include <unordered_map>

const int NUM_PLAYERS = 4;
const float PLAYER_SPEED = 200;

const int MAP_WIDTH = 13;
const int MAP_HEIGHT = 15;

const int BLOCK_SIZE = 32;

const short BOX_SPAWN_CHANCE = 50;
const short BONUS_SPAWN_CHANCE = 100;

class MenuScene;

class BombManager {
public:
    void addBomb(Bomb* bomb);
    void update(float delta);

    bool isAvailablePlace(const cocos2d::Vec2& position) const;
    void setScene(MenuScene* scene) { menuScene = scene; }
private:
    MenuScene* menuScene;

    std::unordered_map<Bomb*,float>::iterator activateBomb(std::unordered_map<Bomb*,float>::iterator bombIter);

    std::unordered_map<Bomb*,float> bombsStatus;  //Bomb - exp.time
};

cocos2d::Vec2 getRandomDirectionRay();

class MenuScene : public cocos2d::Scene {
public:
    virtual bool init();

    CREATE_FUNC(MenuScene);

    static Scene* createScene();
    void menuCloseCallback(cocos2d::Ref* psender);

    void update(float delta);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event);

    std::vector<Command*> handleInput(float delta);

    bool isObjectCollideWith(cocos2d::Node* objectA,cocos2d::Node* objectB) const;
    bool isRectCollideWith(const cocos2d::Rect& rect,cocos2d::Node* objectB) const;

    bool isObjectCollideWithGroup(cocos2d::Node* objectA,std::list<cocos2d::Node*> objects) const;
    bool isRectCollideWithGroup(const cocos2d::Rect& rect,const std::list<cocos2d::Node*>& objects) const;

    bool isGroupContainsPoint(const std::list<cocos2d::Node*>& group,const cocos2d::Vec2& point) const;

    bool isPositionAvailable(const cocos2d::Rect&);

    void onBombExplosion(Bomb* bomb);
    void addBomb(Bomb* bomb) { bombManager.addBomb(bomb); }

    void createExplosion(const cocos2d::Vec2& position);
    void finishExplosion(cocos2d::Node* node);

    void removePlayer(Player* player);

    void spawnBonus(BONUS_TYPE type,cocos2d::Vec2 position);
private:
    void initPlayers();
    void initMap();
    void generateWalls();
    void generateBoxes();

    void findCollisionsWithBonuses();

    Wall* createStandardWallAndAdd();

    void shootRay(cocos2d::Vec2 startPosition,const cocos2d::Vec2& direction,int range,int power=1);
    void shootAllDirectionRay(cocos2d::Vec2 startPosition,int range,int power=1);

    cocos2d::Vec2 getBombPlacePosition(Player* placer);

    KeyManager keyManager;
    BombManager bombManager;

    Player* players[NUM_PLAYERS];

    std::list<Wall*> walls;
    std::list<Node*> explosions;
    std::list<BonusBox*> boxes;
    std::list<Bonus*> bonuses;
};

#endif // MENUSCENE_H_INCLUDED
