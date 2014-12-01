#pragma once

#include <Render/Engine.h>

namespace Game
{
    class GameEntity;
    
    struct Initialize
    {
        GameEntity* level; 
		GLFWwindow* window;
    };
    
    struct Update
    {
        float       delta;
        GameEntity* level;
        GameEntity* player;

        GLFWwindow* window;
        
        glm::mat4    projection;
        glm::mat4    view;
        glm::vec3    from;
        glm::vec3    to;
    };
    
    struct Destroy
    {
        
    };
    
    struct RenderArg
    {
        Engine::PassFrame   passFrame;
        Engine::PassElement passElement;

        glm::vec3    sunDirection;
        glm::mat4    projection;
        glm::mat4    view;
        glm::vec3    from;
        glm::vec3    to;

    };
};
