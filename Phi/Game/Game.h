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
        virtual void            initialize(Intialize& intialize) = 0;
        virtual State::List     update(Update& update);
        virtual void            destroy(Destroy& destroy) = 0;
    
    public:
        void addPart(int id, GamePart* part);
    
    private:
        typedef std::map<int, GamePart*> GameParts;
        
    private:
        GameParts   mGameParts;
        GamePart*   mCurrentGamePart;
    };
    
};