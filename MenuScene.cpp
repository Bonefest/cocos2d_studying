#include "MenuScene.h"

USING_NS_CC;
#include <iostream>
#include <cstdlib>

#include <BitStream.h>
#include <RakNetTypes.h>

MessageQueue* MessageQueue::createBoxTTF(const std::string& path,int fontSize,int maxMessages) {
    MessageQueue* result = new MessageQueue;
    if(result->initWithTTF("",path,fontSize)) {
        result->autorelease();
        result->maxMessages = maxMessages;

        return result;
    }
    CC_SAFE_DELETE(result);
    return nullptr;
}

void MessageQueue::addNewMessage(const std::string& message) {
    messagesQueue.push_back(message);
    if(messagesQueue.size() > maxMessages)
        messagesQueue.pop_front();
    reorganize();
}

void MessageQueue::reorganize() {
    std::string result = "";
    short counter = 0;
    for(auto iter = messagesQueue.begin();iter!=messagesQueue.end();iter++)
        result = std::to_string(counter++) + "." + *iter + "\n" + result;

    this->setString(result);
}

Scene* MenuScene::createScene() {
    return MenuScene::create();
}

bool MenuScene::init() {
    if(!Scene::init())
        return false;

    peer = RakNet::RakPeerInterface::GetInstance();
    connected = connecting = false;

    auto screenSize = cocos2d::Director::getInstance()->getWinSize();


    messageField = cocos2d::ui::TextField::create("Enter your message here","fonts/slkscr.ttf",18);
    messageField->setPosition(Vec2(screenSize.width/2,screenSize.height - screenSize.height/4));
    messageField->setMaxLength(32);
    this->addChild(messageField);

    addressField = cocos2d::ui::TextField::create("IP","fonts/slkscr.ttf",26);
    addressField->setPosition(Vec2(screenSize.width/2,messageField->getPosition().y - 2*messageField->getContentSize().height));
    addressField->setMaxLength(18);
    this->addChild(addressField);

    portField = cocos2d::ui::TextField::create("Port","fonts/slkscr.ttf",26);
    portField->setPosition(Vec2(screenSize.width/2,addressField->getPosition().y - 2*addressField->getContentSize().height));
    portField->setMaxLength(6);
    this->addChild(portField);

    connectButton = cocos2d::ui::Button::create("CloseNormal.png","CloseSelected.png");
    connectButton->setPosition(Vec2(screenSize.width/2,portField->getPosition().y - 2*portField->getContentSize().height));
    this->addChild(connectButton);

    messageQueue = MessageQueue::createBoxTTF("fonts/slkscr.ttf",26,15);
    messageQueue->setMaxLineWidth(screenSize.width);
    messageQueue->setAnchorPoint(Vec2(0.5f,1.0f));
    messageQueue->setPosition(Vec2(screenSize.width/2,screenSize.height/2));
    this->addChild(messageQueue);

    connectButton->addTouchEventListener(CC_CALLBACK_2(MenuScene::startConnection,this));
    this->scheduleUpdate();

    return true;
}

void MenuScene::update(float delta) {
    if(connecting && !isServer) {
        for(packet=peer->Receive();packet&&connecting;peer->DeallocatePacket(packet),packet=peer->Receive()) {
            switch(packet->data[0]) {
            case ID_CONNECTION_ATTEMPT_FAILED:{
                    messageQueue->addNewMessage("Can't connect to a server!Try again");
                    connecting=connected=false;
                }
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED: {
                    messageQueue->addNewMessage("Yo've connected to a server!Yeeah!");
                    connecting=false;
                    connected=true;
                    serverAddress=packet->systemAddress;
            }
                break;

            }
        }
    }

    if(connected) {
        for(packet=peer->Receive();packet;peer->DeallocatePacket(packet),packet=peer->Receive()) {
            switch(packet->data[0]) {
            case ID_NEW_INCOMING_CONNECTION:{
                messageQueue->addNewMessage(std::string("New client: ") + packet->systemAddress.ToString(true,':'));
            }
            case TEXT_MESSAGE: {
                RakNet::RakString rstr;
                RakNet::BitStream bsIn(packet->data,packet->length,false);
                bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
                bsIn.Read(rstr);
                messageQueue->addNewMessage(rstr.C_String());
            }
                break;
            }
        }
    }
}


void MenuScene::startConnection(cocos2d::Ref* pSender,cocos2d::ui::Widget::TouchEventType type) {
    if(cocos2d::ui::Widget::TouchEventType::BEGAN == type) {
        if(!connected && !connecting) {
            int port = atoi(portField->getString().c_str());
            if(port <= 0) {
                connected=false;
                return;
            }

            if(addressField->getString().size() == 0) {
                RakNet::SocketDescriptor sdesc(port,0);
                peer->Startup(MAX_CONNECTIONS,&sdesc,1);
                peer->SetMaximumIncomingConnections(MAX_CONNECTIONS);
                connected=isServer=true;

                messageQueue->addNewMessage("Server has been created!");
            } else {
                if(addressField->getString() == "0")
                    addressField->setString("127.0.0.1");

                RakNet::SocketDescriptor sdesc;
                peer->Startup(1,&sdesc,1);
                peer->Connect(addressField->getString().c_str(),port,nullptr,0);
                connecting=true;
                isServer=false;

                messageQueue->addNewMessage("Connecting...");
            }
        }
        else if(connected) {
            if(messageField->getString().size() > 0) {
                RakNet::BitStream bsOut;
                bsOut.Write((RakNet::MessageID)TEXT_MESSAGE);
                bsOut.Write(messageField->getString().c_str());
                messageField->setString("");
                if(isServer) {
                    RakNet::SystemAddress users[MAX_CONNECTIONS];
                    unsigned short connectedUsers = MAX_CONNECTIONS;
                    peer->GetConnectionList(users,&connectedUsers);
                    for(unsigned short count=0;count<connectedUsers;++count)
                        peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE,0,users[count],false);
                } else
                    peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE,0,serverAddress,false);

            } else
                messageQueue->addNewMessage("Error!Message field is empty!");
        }
    }
}

void MenuScene::menuCloseCallback(cocos2d::Ref* psender) {

    RakNet::RakPeerInterface::DestroyInstance(peer);
    cocos2d::Director::getInstance()->end();
}
