#pragma once

#include <Game/GameEntity.h>

namespace Game
{
    class Player : public GameEntity
    {
        void initialize(Intialize &intialize) override;
        void update(Update& intialize) override;
    };
};


