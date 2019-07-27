#include "GameConfigManager.h"
GameConfigManager* GameConfigManager::instance=nullptr;


std::string GameConfigManager::getSpritePath(const std::string& key) {
    if(key == "Player") return "brick.png";
    if(key == "Wall") return "brick.png";

    return "";
}
