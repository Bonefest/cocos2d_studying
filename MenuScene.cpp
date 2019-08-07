#include "MenuScene.h"
#include "GameConfigManager.h"

USING_NS_CC;
#include <iostream>
#include <algorithm>

cocos2d::Vec2 getRandomDirectionRay() {
    int value = rand() % 4;
    switch(value) {
    case 0: return cocos2d::Vec2(-1,0);
    case 1: return cocos2d::Vec2(0,-1);
    case 2: return cocos2d::Vec2(1,0);
    default: return cocos2d::Vec2(0,1);
    }
}

void BombManager::addBomb(Bomb* bomb) {
    if(bombsStatus.find(bomb) == bombsStatus.end()) {
        bombsStatus[bomb] = 0.0f;
    }
}

void BombManager::update(float delta) {
    for(auto bombIter=bombsStatus.begin();bombIter!=bombsStatus.end();) {
        bombIter->second += delta;
        if(bombIter->second > bombIter->first->getExplosionTime()) {
            bombIter = activateBomb(bombIter);
        } else bombIter++;

    }
}


bool BombManager::isAvailablePlace(const cocos2d::Vec2& position) const {
    for(auto bombIter = bombsStatus.begin();bombIter!=bombsStatus.end();bombIter++) {
        if(bombIter->first->getBoundingBox().containsPoint(position))
            return false;
    }

    return true;
}

std::unordered_map<Bomb*,float>::iterator  BombManager::activateBomb(std::unordered_map<Bomb*,float>::iterator bombIter) {
    menuScene->onBombExplosion(bombIter->first);
    return bombsStatus.erase(bombIter);
}

void MenuScene::onBombExplosion(Bomb* bomb) {
    auto startPos = bomb->getPosition();
    shootRay(startPos,cocos2d::Vec2(-1,0),bomb->getRange());
    shootRay(startPos,cocos2d::Vec2(1,0),bomb->getRange());
    shootRay(startPos,cocos2d::Vec2(0,1),bomb->getRange());
    shootRay(startPos,cocos2d::Vec2(0,-1),bomb->getRange());
    this->removeChild(bomb);

}

//Круто было бы сделать обёртку, которая автоматически принимает функцию тип к которому необходимо преобразовать
//и масисв объектов

void MenuScene::shootAllDirectionRay(cocos2d::Vec2 startPosition,int range,int power) {
    shootRay(startPosition,cocos2d::Vec2(1,0),range,power);
    shootRay(startPosition,cocos2d::Vec2(-1,0),range,power);
    shootRay(startPosition,cocos2d::Vec2(0,1),range,power);
    shootRay(startPosition,cocos2d::Vec2(0,-1),range,power);
}


void MenuScene::shootRay(cocos2d::Vec2 startPosition,const cocos2d::Vec2& direction,int range,int power) {
        for(int i = 0;i<range && power;++i) {

        std::list<Node*> baseObjects;
        std::for_each(walls.begin(),walls.end(),[&](Node* node){baseObjects.push_back(node);});
        if(isGroupContainsPoint(baseObjects,startPosition+Vec2(1,1))) return;
        baseObjects.clear();
        bool changed = false;
        for(auto boxesIter=boxes.begin();boxesIter != boxes.end();boxesIter++) {
            if((*boxesIter)->getBoundingBox().containsPoint(startPosition + Vec2(1,1))) {
                spawnBonus((*boxesIter)->getBonusType(),(*boxesIter)->getPosition());
                (*boxesIter)->removeFromParent();
                boxes.erase(boxesIter);
                power--;
                changed = true;
                break;
            }
        }
        if(changed) continue;

        for(auto bonusIter=bonuses.begin();bonusIter != bonuses.end();bonusIter++) {
            if((*bonusIter)->getBoundingBox().containsPoint(startPosition + Vec2(1,1))) {
                startPosition = (*bonusIter)->getPosition();
                (*bonusIter)->removeFromParent();
                bonuses.erase(bonusIter);
                shootAllDirectionRay(startPosition,3,2);
                power--;
                changed = true;
                break;
            }
        }
        if(changed) continue;

        for(int j = 0;j<NUM_PLAYERS;++j)
            if(players[j]->getBoundingBox().containsPoint(startPosition))
                removePlayer(players[j]);



        createExplosion(startPosition);

        startPosition += direction*32;
    }
}


void MenuScene::spawnBonus(BONUS_TYPE type,cocos2d::Vec2 position) {
    if(rand() % 100 <= BONUS_SPAWN_CHANCE) {//TYPE SPAWN CHANCE
        Bonus* newBonus = Bonus::createBonus(type);
        newBonus->setPosition(position);
        newBonus->setAnchorPoint(Vec2(0,0));
        this->addChild(newBonus);
        bonuses.push_back(newBonus);
    }
}

bool MenuScene::isGroupContainsPoint(const std::list<cocos2d::Node*>& group,const cocos2d::Vec2& point) const {
    for(auto objectIter = group.begin();objectIter != group.end();objectIter++) {
        if((*objectIter)->getBoundingBox().containsPoint(point)) return true;
    }

    return false;
}

void MenuScene::createExplosion(const cocos2d::Vec2& position) {
    Explosion* explosion = Explosion::createExplosion(GameConfigManager::getInstance()->getSpritePath("Explosion"));
    explosion->setAnchorPoint(Vec2(0,0));
    explosion->setPosition(position);
    this->addChild(explosion);
    explosions.push_back(explosion);
    explosion->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(1.0f),cocos2d::CallFuncN::create(CC_CALLBACK_1(MenuScene::finishExplosion,this)),nullptr));
}

void MenuScene::finishExplosion(Node* node) {
    node->removeFromParent();
    explosions.remove(node);
}

Scene* MenuScene::createScene() {
   return MenuScene::create();
}

bool MenuScene::init() {
    if(!Scene::init())
        return false;

    srand(time(0));

    auto keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(MenuScene::onKeyPressed,this);
    keyListener->onKeyReleased = CC_CALLBACK_2(MenuScene::onKeyReleased,this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener,this);

    initPlayers();
    initMap();

    this->scheduleUpdate();
    return true;
}

void MenuScene::initPlayers() {
    for(int i = 0;i<NUM_PLAYERS;++i) {
        players[i] = Player::createPlayer(GameConfigManager::getInstance()->getSpritePath("Player"));
        players[i]->setAnchorPoint(Vec2(0,0));
        players[i]->setSpeed(PLAYER_SPEED);
        this->addChild(players[i]);
    }

    players[0]->setPlayerKeys({cocos2d::EventKeyboard::KeyCode::KEY_A,
                               cocos2d::EventKeyboard::KeyCode::KEY_W,
                               cocos2d::EventKeyboard::KeyCode::KEY_D,
                               cocos2d::EventKeyboard::KeyCode::KEY_S,
                               cocos2d::EventKeyboard::KeyCode::KEY_SPACE,
                               cocos2d::EventKeyboard::KeyCode::KEY_E});

    players[0]->setPosition(Vec2(32,32));

}

void MenuScene::initMap() {
    generateWalls();
    generateBoxes();
    bombManager.setScene(this);
}

void MenuScene::generateWalls() {
    Wall* newWall;
    for(size_t x = 0;x<MAP_WIDTH;++x) {
        newWall = createStandardWallAndAdd();
        newWall->setPosition(x*BLOCK_SIZE,0);

        newWall = createStandardWallAndAdd();
        newWall->setPosition(x*BLOCK_SIZE,(MAP_HEIGHT-1)*BLOCK_SIZE);
    }

    for(size_t y = 1;y<MAP_HEIGHT-1;++y) {
        newWall = createStandardWallAndAdd();
        newWall->setPosition(0,y*BLOCK_SIZE);

        newWall = createStandardWallAndAdd();
        newWall->setPosition( (MAP_WIDTH-1)*BLOCK_SIZE, y*BLOCK_SIZE);
    }

    for(size_t y = 2;y<MAP_HEIGHT-1;y+=2) {
        for(size_t x = 2;x<MAP_WIDTH-1;x+=2) {
            newWall = createStandardWallAndAdd();
            newWall->setPosition(x*BLOCK_SIZE,y*BLOCK_SIZE);
        }
    }

}

void MenuScene::generateBoxes() {
    BonusBox* box;
    for(size_t y = 1;y < MAP_HEIGHT-1;y++) {
        for(size_t x = 1;x < MAP_WIDTH-1;x++) {
            if( (x < 3 || x >= MAP_WIDTH-3) && (y == 1 || y == MAP_HEIGHT-2)) continue;
            if( (y < 3 || y >= MAP_HEIGHT-3) && (x == 1 || x == MAP_WIDTH-2)) {
                if(y % 2 == 0) x++;
                continue;
            }

            if(rand() % 100 <= BOX_SPAWN_CHANCE) {
                box = BonusBox::createBonusBox(BONUS_TYPE(rand()%BONUS_MAX));
                box->setAnchorPoint(Vec2(0,0));
                box->setPosition(Vec2(x*BLOCK_SIZE,y*BLOCK_SIZE));
                this->addChild(box);
                boxes.push_back(box);
            }
            if(y % 2 == 0) x++;
        }
    }
}

Wall* MenuScene::createStandardWallAndAdd() {
   auto newWall = Wall::createWall(GameConfigManager::getInstance()->getSpritePath("Wall"));
   newWall->setAnchorPoint(Vec2(0,0));
   this->addChild(newWall);
   walls.push_back(newWall);

   return newWall;
}

void MenuScene::update(float delta) {

    bombManager.update(delta);

    std::vector<Command*> commands(handleInput(delta));
    for(auto commandIter = commands.begin();commandIter != commands.end();commandIter++) {
        (*commandIter)->execute();
        delete *commandIter;
    }

    findCollisionsWithBonuses();

}

void MenuScene::findCollisionsWithBonuses() {
    for(int i = 0;i<NUM_PLAYERS;++i) {
        //if is player active
        for(auto bonusIter = bonuses.begin();bonusIter != bonuses.end();bonusIter++) {

        }
    }
}

std::vector<Command*> MenuScene::handleInput(float delta) {
    KeyConfig keys;
    cocos2d::Vec2 resultPlayerSpeed;
    bool keyPressed = false;
    std::vector<Command*> inputtedCommands;

    for(size_t i = 0;i<NUM_PLAYERS;++i) {
        keys=players[i]->getPlayerKeys();
        auto playerPosition = players[i]->getPosition();
        float playerSpeed = players[i]->getSpeed();

        if(keyManager.isPressed(keys.keyLeft)) {
            inputtedCommands.push_back(new MoveByCommand(players[i],this,Vec2(-playerSpeed*delta,0)));
        }

        if(keyManager.isPressed(keys.keyRight)) {
            inputtedCommands.push_back(new MoveByCommand(players[i],this,Vec2(playerSpeed*delta,0)));
        }

        if(keyManager.isPressed(keys.keyUp)) {
            inputtedCommands.push_back(new MoveByCommand(players[i],this,Vec2(0,playerSpeed*delta)));
        }

        if(keyManager.isPressed(keys.keyDown)) {
            inputtedCommands.push_back(new MoveByCommand(players[i],this,Vec2(0,-playerSpeed*delta)));
        }

        if(keyManager.isPressed(keys.keyPlace)) {
            auto bombPlacePosition = getBombPlacePosition(players[i]);
            if(players[i]->getAvailableBombsCount() > 0 && bombManager.isAvailablePlace(bombPlacePosition)) {
                inputtedCommands.push_back(new PlaceBomb(getBombPlacePosition(players[i]),
                                                         this,players[i]->getBombsRange()));
            }
        }

    }

    return inputtedCommands;
}


cocos2d::Vec2 MenuScene::getBombPlacePosition(Player* placer) {
    auto placerPosition = placer->getPosition();
    return  cocos2d::Vec2(BLOCK_SIZE*std::round(placerPosition.x/BLOCK_SIZE),
                          BLOCK_SIZE*std::round(placerPosition.y/BLOCK_SIZE));

    return placerPosition;
}

void MenuScene::removePlayer(Player* player) {
    player->removeFromParent();
}

bool MenuScene::isPositionAvailable(const cocos2d::Rect& rect) {
    std::list<Node*> solidSceneObjects;
    std::for_each(walls.begin(),walls.end(),[&](Wall* wall){solidSceneObjects.push_back(wall);});
    std::for_each(boxes.begin(),boxes.end(),[&](BonusBox* box){solidSceneObjects.push_back(box);});
    if(isRectCollideWithGroup(rect,solidSceneObjects)) return false;
    //collisionManager.findCollisionNodeWithGroup(Node*,std::vector<Node*> group)

    return true;
}

bool MenuScene::isObjectCollideWithGroup(cocos2d::Node* objectA,std::list<cocos2d::Node*> objects) const {
    return isRectCollideWithGroup(objectA->getBoundingBox(),objects);
}


bool MenuScene::isRectCollideWithGroup(const cocos2d::Rect& rect,const std::list<cocos2d::Node*>& objects) const {
    for(auto& object: objects) {
        if(isRectCollideWith(rect,object)) return true;
    }

    return false;
}



bool MenuScene::isObjectCollideWith(cocos2d::Node* objectA,cocos2d::Node* objectB) const {
    return isRectCollideWith(objectA->getBoundingBox(),objectB);
}

bool MenuScene::isRectCollideWith(const cocos2d::Rect& rect,cocos2d::Node* objectB) const {
    return rect.intersectsRect(objectB->getBoundingBox());
}




void MenuScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event) {
    keyManager.onKeyPressed(key,event);

}
void MenuScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event) {
    keyManager.onKeyReleased(key,event);

}


void MenuScene::menuCloseCallback(cocos2d::Ref* psender) {

    cocos2d::Director::getInstance()->end();
}
