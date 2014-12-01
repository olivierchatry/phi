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
		glm::vec3 position = glm::vec3(0, 0, 5);
		float horizontalAngle = 3.14f;
		float verticalAngle = 0.0f;
		float initialFoV = 45.0f;
		float speed = 300.0f; // 3 units / second
		float mouseSpeed = 0.08f;
		glm::dvec2 mPreviousMousePosition;
		
    };
}
