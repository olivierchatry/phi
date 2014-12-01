#pragma once

#include <Game/GameEntity.h>
#include <Utils/Smooth.h>

namespace Game
{
    class CameraFPS : public GameEntity
    {
    public:
		CameraFPS() {}
    public:
        void initialize(Initialize &initialize) override;
        void update(Update& update) override;
        void destroy(Destroy& destroy) override;
        void render(RenderArg& render) override;
    private:
		glm::vec3   mPosition = glm::vec3(0, 0, 5);
		float       mHorizontalAngle = 3.14f;
		float       mVerticalAngle = 0.0f;
		float       mSpeed = 300.0f; // 3 units / second
		float       mMouseSpeed = 0.1f;
		glm::dvec2  mPreviousMousePosition;
		
    };
}
