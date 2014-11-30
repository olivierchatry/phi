#pragma once

#include <Game/Game.h>

namespace Game
{
    class Phi : public Game
    {
    public:
        enum Part
        {
            Test = 1,
            Editor
        };
    public:
        void            initialize(Initialize& initialize) override;
        State::List     update(Update& update) override;
        void            render(RenderArg& render) override;
    };
    
}