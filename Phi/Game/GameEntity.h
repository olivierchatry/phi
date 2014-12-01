#pragma once

#include <Game/Game.h>

namespace Game
{
    
    class GameEntity
    {
    public:
        GameEntity() : mActive(true) {}
        virtual ~GameEntity() {}
    public:
        virtual void initialize(Initialize& intialize) = 0;
        virtual void update(Update& update) = 0;
        virtual void destroy(Destroy& destroy) = 0;
        virtual void render(RenderArg& render) = 0;
    public:
        void setActive(bool active) { mActive = active; }
        bool active() { return mActive;}
        
    protected:
        bool    mActive;
    };
    
};