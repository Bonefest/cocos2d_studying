#include "Command.h"

NewSpeedCommand::NewSpeedCommand(Player* unit,cocos2d::Vec2 newSpeed):_unit(unit),_speed(newSpeed) { }

void NewSpeedCommand::execute() {
    _unit->setSpeed(_speed);
}
