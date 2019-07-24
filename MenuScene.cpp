#include "MenuScene.h"
#include <cstdlib>

USING_NS_CC;

void KeyManager::onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event) {
    if(!isPressed(key))
        pressedKeys.push_back(key);
}

void KeyManager::onKeyReleased(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event) {
    pressedKeys.remove(key);
}

static bool isServer = false;

Scene* MenuScene::createScene() {
    return MenuScene::create();
}

bool MenuScene::init() {
    if(!Scene::init())
        return false;

    initServerPart();
    initScenePart();

    this->scheduleUpdate();

    return true;
}

void MenuScene::initServerPart() {
    rmanager = new ReplicaManager;
    rmanager->setScene(this);

    peer = RakNet::RakPeerInterface::GetInstance();
    peer->AttachPlugin(rmanager);
    rmanager->SetNetworkIDManager(&nmanager);

    connected = false;
    ready = false;
}

void MenuScene::initScenePart() {
    auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();

    serverButton = cocos2d::ui::Button::create("brick.png");
    serverButton->setTitleText("Server");
    serverButton->setPosition(Vec2(screenSize.width/4,screenSize.height/2));
    serverButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onServerChoosen,this));
    this->addChild(serverButton);

    clientButton = cocos2d::ui::Button::create("brick.png");
    clientButton->setTitleText("Client");
    clientButton->setPosition(Vec2(screenSize.width - screenSize.width/4,screenSize.height/2));
    clientButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onClientChoosen,this));
    this->addChild(clientButton);

    ball = nullptr;
    player = nullptr;

    serverScore = 0;
    clientScore = 0;

    //Generating top and bottom sides
    auto top = cocos2d::Node::create();
    top->setContentSize(cocos2d::Size(screenSize.width,1));
    top->setAnchorPoint(Vec2(0,0));
    top->setPosition(Vec2(0,screenSize.height-32));
    physicsObjects.push_back(top);
    this->addChild(top);

    auto bottom = cocos2d::Node::create();
    bottom->setContentSize(cocos2d::Size(screenSize.width,1));
    bottom->setAnchorPoint(Vec2(0,0));
    bottom->setPosition(Vec2(0,0));
    physicsObjects.push_back(bottom);
    this->addChild(bottom);

    auto keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(KeyManager::onKeyPressed,&keyManager);
    keyListener->onKeyReleased = CC_CALLBACK_2(KeyManager::onKeyReleased,&keyManager);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener,this);
}

void MenuScene::update(float delta) {
    for(packet=peer->Receive();packet;peer->DeallocatePacket(packet),packet=peer->Receive()) {
        switch(packet->data[0]) {
        case ID_CONNECTION_REQUEST_ACCEPTED:
            {
                connected=true;
                std::cout << "You've connected to server: " << packet->systemAddress.ToString() << std::endl;

                remoteGUID = peer->GetGuidFromSystemAddress(packet->systemAddress);

                this->removeChild(serverButton);
                this->removeChild(clientButton);

                auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
                readyButton = cocos2d::ui::Button::create("brick.png");
                readyButton->setPosition(Vec2(screenSize.width/2,screenSize.height/2));
                readyButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::onReadyChoosen,this));
                this->addChild(readyButton);
            }
            break;
        case ID_REMOTE_NEW_INCOMING_CONNECTION:
            {
                std::cout << "New user has been connected : " << packet->systemAddress.ToString() << std::endl;
                remoteGUID = peer->GetGuidFromSystemAddress(packet->systemAddress);
            }
            break;

        case ID_CONNECTION_ATTEMPT_FAILED:
            {
                std::cout << "Connection attempt failed!\n";
            }
            break;
        case ID_READY_START_MESSAGE:
            {
                connected=true;
                std::cout << packet->systemAddress.ToString() << " ready to start!\n";
                initGame();
                physicsObjects.push_back(player);
                ballDirection.set(0.7,0.7);
            }
            break;
        case ID_NEW_SCORE_MESSAGE:
            {
                std::cout << "Message\n";
                RakNet::RakString message;
                RakNet::BitStream inputStream(packet->data,packet->length,false);
                inputStream.IgnoreBytes(sizeof(RakNet::MessageID));
                inputStream.Read(message);
                std::cout << message << std::endl;
                serverScore = atoi(message.C_String());
                std::cout << serverScore << std::endl;
                inputStream.Read(message);

                std::cout << message << std::endl;
                clientScore = atoi(message.C_String());

                updateScoreStatus();
            }
            break;
        }
    }

    if(connected && ready) {
        Command* command = handleInput(delta);
        command->execute();
        delete command;

        if(isServer)
            updateBall(delta);
    }
}

void MenuScene::updateBall(float delta) {
    auto ballRect = ball->getBoundingBox();
    auto ballPosition = ball->getPosition();

    for(auto physicsIter=physicsObjects.begin();physicsIter!=physicsObjects.end();physicsIter++) {
        if((*physicsIter)->getBoundingBox().intersectsRect(ballRect)) {
            //CHANGE DIR
            auto physicsObjectRect = (*physicsIter)->getBoundingBox();
            if(ballRect.getMaxX() > physicsObjectRect.getMaxX() ||    //Если самая правая точка шара находится дальше самой правой точки прямоугольника или наоборот
            ballRect.getMinX() < physicsObjectRect.getMinX()) {
                ballDirection.set(-ballDirection.x,ballDirection.y);
            } else {
                ballDirection.set(ballDirection.x,-ballDirection.y);
            }

        }
    }


    ballPosition.add(Vec2(ballDirection.x*SPEED_BALL*delta,ballDirection.y*SPEED_BALL*delta));
    ball->setPosition(ballPosition);

    //check out of border
    checkBallOut();



}

void MenuScene::checkBallOut() {

    auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    auto ballPosition = ball->getPosition();


    if(ballPosition.x <= 0 || ballPosition.x >= screenSize.width) {

        if(ballPosition.x <= 0) {
            ballPosition.x = screenSize.width/2;
            ballPosition.y = screenSize.height/2;
            clientScore++;
        } else if(ballPosition.x > screenSize.width) {
            ballPosition.x = screenSize.width/2;
            ballPosition.y = screenSize.height/2;
            serverScore++;
        }
        ball->setPosition(ballPosition);

        updateScoreStatus();

        RakNet::BitStream message;
        message.Write((RakNet::MessageID)ID_NEW_SCORE_MESSAGE);
        RakNet::RakString test;

        message.Write(RakNet::RakString::ToString(serverScore));
        message.Write(RakNet::RakString::ToString(clientScore));
        peer->Send(&message,HIGH_PRIORITY,RELIABLE,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
    }
}

Command* MenuScene::handleInput(float delta) {
    if(keyManager.isPressed(cocos2d::EventKeyboard::KeyCode::KEY_W)) {
        auto playerPosition = player->getPosition();
        playerPosition.add(Vec2(0,SPEED_PLAYER*delta));
        return new MoveCommand(player,playerPosition);
    }
    else if(keyManager.isPressed(cocos2d::EventKeyboard::KeyCode::KEY_S)) {
        auto playerPosition = player->getPosition();
        playerPosition.add(Vec2(0,-SPEED_PLAYER*delta));
        return new MoveCommand(player,playerPosition);
    } else if(keyManager.isPressed(cocos2d::EventKeyboard::KeyCode::KEY_R)) {
        if(isServer) {
            auto ballPosition = Vec2(300,300);
            return new MoveCommand(ball,ballPosition);
        }
    }

    return new NullCommand;
}

void MenuScene::onClientChoosen(cocos2d::Ref* sender,cocos2d::ui::Widget::TouchEventType type) {
    RakNet::SocketDescriptor sdesc;
    peer->Startup(1,&sdesc,1);
    peer->Connect(IP,PORT,nullptr,0);
    peer->SetMaximumIncomingConnections(1);
    isServer=false;
}

void MenuScene::onServerChoosen(cocos2d::Ref* sender,cocos2d::ui::Widget::TouchEventType type) {
    RakNet::SocketDescriptor sdesc(PORT,0);
    peer->SetMaximumIncomingConnections(1);
    if(peer->Startup(1,&sdesc,1) == RakNet::StartupResult::RAKNET_STARTED) {
        startServer();
    } else std::cout << "Can't create a server!\n";
}

void MenuScene::onReadyChoosen(cocos2d::Ref* sender,cocos2d::ui::Widget::TouchEventType type) {
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)ID_READY_START_MESSAGE);
    peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE,0,remoteGUID,false);
    this->removeChild(readyButton);

    initGame();
}


void MenuScene::updateScoreStatus() {
    serverScoreLabel->setString("Server:" + std::to_string(serverScore));
    clientScoreLabel->setString("Client:" + std::to_string(clientScore));
}

void MenuScene::initGame() {
    initPlayer();

    auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();

    serverScoreLabel = cocos2d::Label::createWithTTF("Server:0","fonts/slkscr.ttf",26);
    serverScoreLabel->setPosition(Vec2(screenSize.width*0.3,screenSize.height*0.75));
    this->addChild(serverScoreLabel);

    clientScoreLabel = cocos2d::Label::createWithTTF("Client:0","fonts/slkscr.ttf",26);
    clientScoreLabel->setPosition(Vec2(screenSize.width*0.7,screenSize.height*0.75));
    this->addChild(clientScoreLabel);

}

void MenuScene::initPlayer() {
    player = SpriteSerializable::createWithFile("brick.png");
    auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    if(isServer)
        player->setPosition(screenSize.width/4,screenSize.height/2);
    else player->setPosition(screenSize.width - screenSize.width/4,screenSize.height/2);

    player->setContentSize(Size(32,128));
    SpriteReplica* replica = new SpriteReplica(player,isServer);
    player->setReplica(replica);
    rmanager->Reference(replica);
    this->addChild(player);

    ready=true;
}

void MenuScene::startServer() {
    isServer = true;
    initServerBall();

    this->removeChild(serverButton);
    this->removeChild(clientButton);
}

void MenuScene::initServerBall() {
    auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    ball = SpriteSerializable::createWithFile("brick.png");
    ball->setPosition(screenSize.width/2,screenSize.height/2);
    this->addChild(ball);

    SpriteReplica* ballReplica = new SpriteReplica(ball,isServer);
    ball->setReplica(ballReplica);
    rmanager->Reference(ballReplica);
}

RakNet::Replica3* MenuScene::replicaFactory(const RakNet::RakString& str) {
    if(str == "Sprite") {
        SpriteSerializable* sprite = SpriteSerializable::createWithFile("brick.png");
        sprite->setPosition(0,0);
        SpriteReplica* replica = new SpriteReplica(sprite,isServer);
        sprite->setReplica(replica);
        this->addChild(sprite);
        physicsObjects.push_back(sprite);

        return replica;
    }

    return nullptr;
}



void MenuScene::menuCloseCallback(cocos2d::Ref* psender) {
    cocos2d::Director::getInstance()->end();
    RakNet::RakPeerInterface::DestroyInstance(peer);
    delete rmanager;
}
