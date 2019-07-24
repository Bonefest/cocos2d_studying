#include "GameConfigManager.h"

const std::string& GameConfigManager::getSpritePath(const std::string& key) {
    if(key == "Player") return "brick.png";
}
