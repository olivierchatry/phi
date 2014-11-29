#pragma once

#include <Game/Game.h>

namespace Game
{
    
    class GameEntity
    {
    public:
        virtual void initialize(Intialize& intialize) = 0;
        virtual void update(Update& update) = 0;
    };
    
};