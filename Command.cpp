#include "Command.h"
MoveByCommand::MoveByCommand(cocos2d::Sprite* unit,
                                 MenuScene* scene,
                                 const cocos2d::Vec2 newPosition)
                                 :_unit(unit),_scene(scene),newPos(newPosition) { }


void MoveByCommand::execute() {
    cocos2d::Rect unitRect = _unit->getBoundingBox();
    cocos2d::Vec2 newTotalPos = newPos + _unit->getPosition();
    unitRect.setRect(newTotalPos.x+2,newTotalPos.y+2,unitRect.size.width-4,unitRect.size.height-4);
    if(_scene->isPositionAvailable(unitRect))
        _unit->setPosition(newTotalPos);
}
