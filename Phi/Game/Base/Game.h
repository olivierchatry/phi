#pragma once

#include <Game/GamePart.h>
#include <Game/GameStructure.h>

namespace Game
{
    class Game
    {
    public:
        struct State
        {
            enum List
            {
                Continue,
                Exit
            };
        };
        
    public:
        virtual ~Game() {}
        Game();
        
    public:
        virtual void            initialize(Initialize& initialize) = 0;
        virtual State::List     update(Update& update);
        virtual void            destroy(Destroy& destroy);
        virtual void            render(RenderArg& render);
    public:
        void addPart(int id, GamePart* part);
    
    protected:
        typedef std::map<int, GamePart*> GameParts;
        
    protected:
        GameParts   mGameParts;
        GamePart*   mCurrentGamePart;
    };
    
};