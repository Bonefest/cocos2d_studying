#ifndef GAMECONFIGMANAGER_H_INCLUDED
#define GAMECONFIGMANAGER_H_INCLUDED

#include <map>
#include <string>

class GameConfigManager {
public:
    static GameConfigManager* getInstance() {
        if(instance == nullptr)
            instance = new GameConfigManager;
        return instance;
    }

    static void close() {
        delete getInstance();
        instance = nullptr;
    }

    void loadConfig(const std::string& path);
    const std::string& getSpritePath(const std::string& key);

private:
    GameConfigManager() { }
    GameConfigManager(const GameConfigManager& manager) { }
    ~GameConfigManager() { }
    GameConfigManager& operator=(const GameConfigManager& manager) { }

    static GameConfigManager* instance;
    std::map<std::string,std::string> spritesTableNamePath;

};

GameConfigManager* GameConfigManager::instance=nullptr;


#endif // GAMECONFIGMANAGER_H_INCLUDED
