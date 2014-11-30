#pragma once

#include <Game/GameEntity.h>
#include <Utils/Smooth.h>

namespace Game
{
    class CameraFollowPlayer : public GameEntity
    {
    public:
        CameraFollowPlayer() : mVelocitySmoothed(10) {}
    public:
        void initialize(Initialize &initialize) override;
        void update(Update& update) override;
        void destroy(Destroy& destroy) override;
        void render(RenderArg& render) override;
    private:
        Utils::Smooth<glm::vec3>    mVelocitySmoothed;
        float                       mDeltaOnSpline;

    };
}
