#pragma once

#include <Game/GameStructure.h>

namespace Game
{
    
    class GamePart
    {
    public:
        virtual ~GamePart() {}
        
    public:
        virtual void    initialize(Intialize& intialize) = 0;
        virtual int     update(Update& update) = 0;
        virtual void    destroy(Destroy& destroy) = 0;
    };
    
};