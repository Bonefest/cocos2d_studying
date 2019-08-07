#include "GameConfigManager.h"
GameConfigManager* GameConfigManager::instance=nullptr;


#include <iostream>
std::string GameConfigManager::getSpritePath(const std::string& key) {

    if(key == "Player") return "brick.png";
    if(key == "Wall") return "brick.png";
    if(key == "Bomb") return "bomb.png";
    if(key == "Explosion") return "explosion.png";
    if(key == "BonusBoxRange") return "box.png";
    if(key == "BonusBoxSpeed") return "box.png";
    if(key == "BonusBoxBomb") return "box.png";
    if(key == "BonusRange") return "bonus.png";
    if(key == "BonusSpeed") return "bonus.png";
    if(key == "BonusBomb") return "bonus.png";
    if(key == "None") return "brick.png";

    return "brick.png";
}
