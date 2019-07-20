#include "MenuScene.h"

USING_NS_CC;

Player* Player::createPlayer(PlayerReplica* replica) {
    Player* player = new Player;
    if(player->initWithFile("brick.png")) {
        player->autorelease();
        player->replica.SetCompositeOwner(replica);
        return player;
    }
    CC_SAFE_DELETE(player);
    return nullptr;
}

void Player::updateState(float delta) {
    setPosition(replica.GetCompositeOwner()->getPosition());
}

Scene* MenuScene::createScene() {
    return MenuScene::create();
}

bool MenuScene::init() {
    if(!Scene::init())
        return false;

    rmanager = new ReplicaManager;
    rmanager->setScene(this);

    typeChoosen = false;
    connected = false;

    myPlayer = nullptr;

    peer = RakNet::RakPeerInterface::GetInstance();
    peer->AttachPlugin(rmanager);
    rmanager->SetNetworkIDManager(&nmanager);

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(MenuScene::onKeyPressed,this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);
    this->scheduleUpdate();

    return true;
}

void MenuScene::onConnectNewUser(PlayerReplica* playerReplica) {
    Player* newPlayer = Player::createPlayer(playerReplica);
    if(newPlayer != nullptr) {
            std::cout << "here" << std::endl;
        connectedPlayers.push_back(newPlayer);
        this->addChild(newPlayer);

            std::cout << "here2" << std::endl;
    }
}

void MenuScene::update(float delta) {
    if(typeChoosen) {
        for(packet=peer->Receive();packet;peer->DeallocatePacket(packet),packet=peer->Receive()) {
            switch(packet->data[0]) {
            case ID_CONNECTION_REQUEST_ACCEPTED:
                {
                    PlayerReplica* replica = new PlayerReplica;
                    myPlayer = Player::createPlayer(replica);
                    myPlayer->setPosition(Vec2(cocos2d::RandomHelper::random_int(50,200),cocos2d::RandomHelper::random_int(50,200)));
                    this->addChild(myPlayer);
                    rmanager->Reference(replica);
                    std::cout << "Succesfully connected!\n";
                    connected=true;
                }
                break;
            case ID_NEW_INCOMING_CONNECTION:
                std::cout << "New connection from: " << packet->systemAddress.ToString() << std::endl;
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                if(isServer) {
                    //Ищем и удаляем объект
                } else menuCloseCallback(nullptr);
                break;
            }
        }
    }

    if(connected) {
        for(auto iter=connectedPlayers.begin();iter!=connectedPlayers.end();iter++)
            (*iter)->updateState(delta);
    }
}

void MenuScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode key,cocos2d::Event* event) {
    if(!typeChoosen) {
        if(key == cocos2d::EventKeyboard::KeyCode::KEY_C) {
            RakNet::SocketDescriptor sdesc;
            peer->Startup(1,&sdesc,1);
            peer->Connect(IP,PORT,nullptr,0);
            isServer = false;
            typeChoosen = true;
        } else if(key == cocos2d::EventKeyboard::KeyCode::KEY_S) {
            RakNet::SocketDescriptor sdesc(PORT,0);
            peer->Startup(4,&sdesc,1);
            peer->SetMaximumIncomingConnections(4);
            isServer = true;
            typeChoosen = true;
            connected = true;

            std::cout << "Server has been started!\n" << std::endl;

            PlayerReplica* replica = new PlayerReplica;
            myPlayer = Player::createPlayer(replica);
            myPlayer->setPosition(Vec2(50,50));
            this->addChild(myPlayer);
            rmanager->Reference(replica);
        }
    } else {
        if(connected && myPlayer) {
            auto pos = myPlayer->getPosition();
            if(key == cocos2d::EventKeyboard::KeyCode::KEY_A) {
                pos.add(Vec2(-20,0));
            } else if(key == cocos2d::EventKeyboard::KeyCode::KEY_D) {
                pos.add(Vec2(20,0));
            }

            myPlayer->setPosition(pos);

        }
    }
}

void MenuScene::menuCloseCallback(cocos2d::Ref* psender) {
    cocos2d::Director::getInstance()->end();
    RakNet::RakPeerInterface::DestroyInstance(peer);
    delete rmanager;
}
