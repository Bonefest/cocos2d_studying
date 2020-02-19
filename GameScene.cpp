#include "GameScene.h"
#include "BitStream.h"
#include "RakString.h"
#include "DS_List.h"

#include <iostream>

unsigned int alligner(double value) {
    return SIZE*std::round(value/float(SIZE));
}


GameScene* GameScene::create(RakNet::RakPeerInterface* peer,USER_TYPE type,std::string name) {
    GameScene* scene = new GameScene;
    scene->peer = peer;
    scene->userType = type;
    scene->name = name;

    if(scene->init()) {
        scene->autorelease();
        return scene;
    }
    CC_SAFE_DELETE(scene);
    return nullptr;
}

void GameScene::onExit() {


    delete snake;
    peer->Shutdown(300);         //Вызывать его крайне важно, т.к если мы его не вызовем -
                                //Сервер будет ждать ответа и разорвет соединение, из-за чего
                                //В реплике вызывается onPop, вместо уничтожения.Не будут вызываны
                                //ни DeserializeDestruction ни DeserializeDestruction


    RakNet::RakPeerInterface::DestroyInstance(peer);

    delete replicaManager;
    delete networkManager;
    Scene::onExit();
    //deleting...
}

bool GameScene::init() {
    if(!Scene::init()) {
        return false;
    }
    networkManager = new RakNet::NetworkIDManager();
    replicaManager = new ReplicaManager;
    replicaManager->setScene(this);
    replicaManager->SetAutoManageConnections(true,true);
    peer->AttachPlugin(replicaManager);
    replicaManager->SetNetworkIDManager(networkManager);

    prepareScene();

    if(userType == CLIENT);
        onClientInitScene();

    auto keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed,this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener,this);
    scheduleUpdate();
    return true;
}

void GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event) {
    Direction currentDirection = snake->getDirection();
    if(key == cocos2d::EventKeyboard::KeyCode::KEY_W && currentDirection != DOWN ) snake->setDirection(UP);
    else if(key == cocos2d::EventKeyboard::KeyCode::KEY_S && currentDirection != UP   ) snake->setDirection(DOWN);
    else if(key == cocos2d::EventKeyboard::KeyCode::KEY_A && currentDirection != RIGHT) snake->setDirection(LEFT);
    else if(key == cocos2d::EventKeyboard::KeyCode::KEY_D && currentDirection != LEFT ) snake->setDirection(RIGHT);

}

void GameScene::prepareScene() {
    status = WAITING_FOR_PLAYERS;
    teamColor = RED;
    nextColor = GREEN;
    appleTimer = 0.0f;

    visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    connectedUsersDataLabel = cocos2d::Label::createWithTTF("Label","fonts/arial.ttf",32);
    connectedUsersDataLabel->setPosition(WIDTH,9*visibleSize.height/10);
    connectedUsersDataLabel->setAnchorPoint(cocos2d::Vec2(0.0f,1.0f));
    this->addChild(connectedUsersDataLabel);

    scoreLabel = cocos2d::Label::createWithTTF("Score: 1","fonts/arial.ttf",32);
    scoreLabel->setPosition(WIDTH,6*visibleSize.height/10);
    scoreLabel->setAnchorPoint(cocos2d::Vec2(0.0f,1.0f));
    this->addChild(scoreLabel);

    messageLabel = cocos2d::Label::createWithTTF("","fonts/arial.ttf",64);
    messageLabel->setPosition(WIDTH/2,HEIGHT/2);
    this->addChild(messageLabel);

    borderRectNode = cocos2d::DrawNode::create(1.0f);
    borderRectNode->drawRect(cocos2d::Vec2(1,0),cocos2d::Vec2(WIDTH,HEIGHT-1),cocos2d::Color4F::RED);
    this->addChild(borderRectNode);

    players[peer->GetMyGUID()].playerName = name.c_str();
    players[peer->GetMyGUID()].status = FINISHED;
    updateUsersDataLabel();

    snake = new Snake(this,replicaManager,cocos2d::Vec2::ZERO,teamColor,userType);
}

void GameScene::onClientInitScene() {
    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_CLIENT_CONNECTED_TO_ROOM);
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
}

void GameScene::update(float delta) {
    if(userType == SERVER) tryNotifyWinner();

    appleTimer += delta;

    readPackets();
    if(players.size() == MAX_CONNECTIONS+1 && userType == SERVER && status == WAITING_FOR_PLAYERS) {
        startGame();
    }

    if(status != WAITING_FOR_PLAYERS) {
        if(appleTimer >= APPLE_TIMER && userType == SERVER) {
            generateApple();
            appleTimer = 0.0f;
        }

        if(status == STARTED) {
            snake->update(delta);
            checkCollision();
            updatePlayersParts();
        }

        updateApplesPositions();


    }
}

void GameScene::tryNotifyWinner() {
    size_t loserCount = 0;
    Player lastStarted;
    RakNet::RakNetGUID guid;

    for(auto playerIter = players.begin();playerIter != players.end();playerIter++) {
        if(playerIter->second.status == FINISHED) loserCount++;
        else if(playerIter->second.status == STARTED) {
            lastStarted = playerIter->second;
            guid = playerIter->first;
        }
    }

    if(loserCount == MAX_CONNECTIONS) {
        RakNet::RakString message((std::string("Winner ") + lastStarted.playerName.C_String()).c_str());
        RakNet::BitStream streamOut;
        streamOut.Write((RakNet::MessageID)ID_WINNER);
        streamOut.Write(message);
        peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
        updateMessageLabel(message.C_String());
    }
}

void GameScene::updateApplesPositions() {
    for(auto appleIter = sceneApples.begin();appleIter != sceneApples.end();) {
        //Если у реплики установлен guid удаляющего, значит он удалён - убираем со сцены и списка
        if((*appleIter)->getReplica()->deletingSystemGUID != RakNet::UNASSIGNED_RAKNET_GUID) {
            this->removeChild(*appleIter);
            appleIter = sceneApples.erase(appleIter);
            continue;
        }
        (*appleIter)->setPosition((*appleIter)->getReplica()->getPosition());
        appleIter++;
    }

}

void GameScene::checkCollision() {
    if(snake->isIntersectsSelf()) {
        snake->removeFromScene();
        status = FINISHED;
        if(userType == CLIENT) sendStatusToServer();
        return;
    }

    cocos2d::Rect objectRect;
    for(auto partsIter = playersParts.begin();partsIter != playersParts.end();partsIter++) {
        objectRect = (*partsIter)->getBoundingBox();
        objectRect.size.width -= 2;
        objectRect.size.height -= 2;
        objectRect.origin.add(cocos2d::Vec2(1,1));
        if(snake->isIntersectsWith(objectRect)) {
            snake->removeFromScene();
            status = FINISHED;
            if(userType == CLIENT) sendStatusToServer();
            return;
        }
    }

    for(auto appleIter = sceneApples.begin();appleIter != sceneApples.end();appleIter++) {
        cocos2d::Rect rect = (*appleIter)->getBoundingBox();

        rect.size.width -= 2;           //Уменьшаем размер и смещаем левый нижний угол на
        rect.size.height -= 2;          //Половину уменьшенего размера - это защитит
        rect.origin.x += 1;             //от столкновений, когда змея рядом,а не в клетке
        rect.origin.y += 1;             //(без этого,она будет сталкиваться, когда будет
                                        //даже вокруг еды т.к будет пересекаться самый крайний пиксель)

        if(snake->isIntersectsWith(rect)) {
            replicaManager->BroadcastDestruction((*appleIter)->getReplica(),RakNet::UNASSIGNED_SYSTEM_ADDRESS);
            RakNet::OP_DELETE((*appleIter)->getReplica(),_FILE_AND_LINE_);
            sceneApples.erase(appleIter);
            this->removeChild(*appleIter);
            snake->addPart();
            updateScoreLabel();
            break;
        }
    }
}

void GameScene::sendStatusToServer() {
    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_CLIENT_STATUS);
    streamOut.Write(status);
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
}

void GameScene::updateScoreLabel() {
    scoreLabel->setString(std::string("Score: ") + std::to_string(snake->getSnakeLength()));
}

void GameScene::generateApple() {
    if(sceneApples.size() < MAX_APPLES) {
        cocos2d::Vec2 randomPosition(alligner(cocos2d::RandomHelper::random_int(0,int(WIDTH)))-SIZE/2,alligner(cocos2d::RandomHelper::random_int(0,int(HEIGHT)))-SIZE/2);
        Apple* apple = Apple::createApple();
        AppleReplica* appleReplica = new AppleReplica(userType);
        apple->setReplica(appleReplica);
        apple->setPosition(randomPosition);
        sceneApples.push_back(apple);
        this->addChild(apple);
        replicaManager->Reference(appleReplica);
    }
}

void GameScene::updatePlayersParts() {
    for(auto snakePartIter = playersParts.begin();snakePartIter != playersParts.end();){
        if((*snakePartIter)->getReplica()->deletingSystemGUID != RakNet::UNASSIGNED_RAKNET_GUID) {
            this->removeChild(*snakePartIter);
            snakePartIter = playersParts.erase(snakePartIter);
            continue;
        }

        (*snakePartIter)->setPosition((*snakePartIter)->getReplica()->getPosition());
        if( (*snakePartIter)->isHead()) {
            (*snakePartIter)->setTexture("box.png");
            (*snakePartIter)->setContentSize(cocos2d::Size(SIZE,SIZE));
        }
        snakePartIter++;
    }
}

void GameScene::startGame() {
    status = STARTED;


    initPlayersSnakes();
}


void GameScene::initPlayersSnakes() {
    snake->setPosition(cocos2d::Vec2(SIZE/2,SIZE/2));
    snake->setDirection(RIGHT);

    RakNet::SystemAddress connectedUsers[MAX_CONNECTIONS];
    unsigned short connectedUsersCount = MAX_CONNECTIONS;
    peer->GetConnectionList(connectedUsers,&connectedUsersCount);
    std::cout << connectedUsersCount << std::endl;
    for(unsigned short i = 0;i<connectedUsersCount;++i) {
        RakNet::BitStream streamOut;
        streamOut.Write((RakNet::MessageID)ID_GAME_START);
        streamOut.Write( ((i+2)%2 == 0)?LEFT:RIGHT);
        streamOut.Write( ((i+2)%2 == 0)?WIDTH-SIZE/2:SIZE/2 );
        streamOut.Write( ((i+2)<=2)?SIZE/2:HEIGHT-SIZE/2);
        peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,connectedUsers[i],false);

    }

}

void GameScene::readPackets() {
    for(packet=peer->Receive();packet;peer->DeallocatePacket(packet),packet=peer->Receive()) {
        switch(packet->data[0]) {

        //SERVER MESSAGES
        case ID_DISCONNECTION_NOTIFICATION:
        case ID_CONNECTION_LOST:
            onDisconnectMessage(packet);
            break;

        case ID_CLIENT_CONNECTED_TO_ROOM:
            players[packet->guid].playerName = RakNet::RakString("");
            askForUserName(packet->guid);
            break;
        case ID_CLIENT_NAME:
            readUserName(packet);
            setTeamColorToClient(packet->guid);
            sendUserdataAll();
            updateUsersDataLabel();
            break;
        case ID_CLIENT_STATUS:
            readClientStatus(packet);
            break;

        //CLIENT MESSAGES
        case ID_ASK_FOR_NAME:
            sendNameTo(packet->guid);
            break;
        case ID_SET_TEAMCOLOR_FOR_CLIENT:
            readTeamColor(packet);
            break;
        case ID_CONNECTED_USERS_DATA:
            readUserdata(packet);
            updateUsersDataLabel();
            break;
        case ID_GAME_START:
            onGameStartedMessage(packet);
            break;
        case ID_WINNER:
            readWinner(packet);
            break;
        }
    }
}

void GameScene::readWinner(RakNet::Packet* packet) {
    RakNet::BitStream streamIn(packet->data,packet->length,false);
    streamIn.IgnoreBytes(sizeof(RakNet::MessageID));
    RakNet::RakString message;
    streamIn.Read(message);
    updateMessageLabel(message.C_String());
}

void GameScene::updateMessageLabel(std::string message) {
    messageLabel->setString(message);
}

void GameScene::readClientStatus(RakNet::Packet* packet) {
    RakNet::BitStream streamIn(packet->data,packet->length,false);
    streamIn.IgnoreBytes(sizeof(RakNet::MessageID));
    streamIn.Read(players[packet->guid].status);
    std::cout << "Message!\n" << std::endl;
}

void GameScene::onGameStartedMessage(RakNet::Packet* packet) {
    status = STARTED;

    int posX,posY;
    Direction direction;
    RakNet::BitStream streamIn(packet->data,packet->length,false);
    streamIn.IgnoreBytes(sizeof(RakNet::MessageID));

    streamIn.Read(direction);
    streamIn.Read(posX);
    streamIn.Read(posY);

    snake->setDirection(direction);
    snake->setPosition(cocos2d::Vec2(posX,posY));
}

void GameScene::onDisconnectMessage(RakNet::Packet* packet) {
    switch(userType) {
        case SERVER: {
            players.erase(packet->guid);
            sendUserdataAll();
            updateUsersDataLabel();

            DataStructures::List<RakNet::Replica3*> replicasCreatedBy;
            replicaManager->GetReplicasCreatedByGuid(packet->guid,replicasCreatedBy);
            replicaManager->BroadcastDestructionList(replicasCreatedBy,RakNet::UNASSIGNED_SYSTEM_ADDRESS);

            break;
        }
        case CLIENT:
            break;
    }
}

void GameScene::askForUserName(RakNet::AddressOrGUID address) {
    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_ASK_FOR_NAME);
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,address,false);
}

void GameScene::readUserName(RakNet::Packet* packet) {
    RakNet::RakString userName;
    RakNet::BitStream streamIn(packet->data,packet->length,false);
    streamIn.IgnoreBytes(sizeof(RakNet::MessageID));
    streamIn.Read(userName);
    players[packet->guid].playerName = userName;
    players[packet->guid].status = STARTED;
}

void GameScene::sendNameTo(RakNet::AddressOrGUID address) {
    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_CLIENT_NAME);
    streamOut.Write(RakNet::RakString(name.c_str()));
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,address,false);
}

void GameScene::setTeamColorToClient(RakNet::RakNetGUID guid) {
    players[guid].color = nextColor;
    nextColor = TeamColor(int(nextColor) + 1);

    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_SET_TEAMCOLOR_FOR_CLIENT);
    streamOut.Write(players[guid].color);
    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,guid,false);
}

void GameScene::readTeamColor(RakNet::Packet* packet) {
    RakNet::BitStream streamIn(packet->data,packet->length,false);
    streamIn.IgnoreBytes(sizeof(RakNet::MessageID));
    streamIn.Read(teamColor);
}

void GameScene::sendUserdataAll() {
    RakNet::BitStream streamOut;
    streamOut.Write((RakNet::MessageID)ID_CONNECTED_USERS_DATA);
    streamOut.Write(players.size());
    for(auto mapIter = players.begin();mapIter != players.end(); mapIter++) {
        streamOut.Write(mapIter->first);
        streamOut.Write(mapIter->second.playerName);
    }

    peer->Send(&streamOut,HIGH_PRIORITY,RELIABLE,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
}

void GameScene::readUserdata(RakNet::Packet* packet) {
    RakNet::BitStream streamIn(packet->data,packet->length,false);
    streamIn.IgnoreBytes(sizeof(RakNet::MessageID));
    players.clear();

    size_t playersCount;
    streamIn.Read(playersCount);

    RakNet::RakNetGUID lastGuid;
    Player lastPlayer;


    for(size_t i = 0;i<playersCount;++i) {
        streamIn.Read(lastGuid);
        streamIn.Read(lastPlayer.playerName);
        players[lastGuid] = lastPlayer;
    }

}

void GameScene::updateUsersDataLabel() {
    std::string usersData = std::to_string(players.size()) + "/" + std::to_string(MAX_CONNECTIONS+1) + "\n";

    for(auto playerIter = players.begin();playerIter != players.end();playerIter++) {
        usersData += std::string(playerIter->second.playerName.C_String()) + "\n";
    }

    connectedUsersDataLabel->setString(usersData);
}


RakNet::Replica3* GameScene::replicaFactory(RakNet::RakString type) {
    if(type == "SnakePart") {
        SnakePart* snakePart = SnakePart::createPart(false);
        SnakePartReplica* replica = new SnakePartReplica(userType);
        snakePart->setReplica(replica);
        snakePart->setPosition(cocos2d::Vec2(-SIZE*3,-SIZE*3));

        this->addChild(snakePart);
        playersParts.push_back(snakePart);
        return replica;
    } else if( type == "Apple") {
        Apple* apple = Apple::createApple();
        AppleReplica* replica = new AppleReplica(userType);
        apple->setReplica(replica);
        this->addChild(apple);
        sceneApples.push_back(apple);

        return replica;
    }

    return nullptr;
}
