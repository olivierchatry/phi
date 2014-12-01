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

		glm::mat4	 unproject;
		glm::vec3		mouseDirection;
    };
    
    struct Destroy
    {
        
    };
    
    struct RenderArg
    {
		RenderArg(Update& update) {
			projection = update.projection;
			view = update.view;
			from = update.from;
			to = update.to;
			level = update.level;
			unproject = update.unproject;
			mouseDirection = update.mouseDirection;
		}

        Engine::PassFrame   passFrame;
        Engine::PassElement passElement;

        glm::vec3		sunDirection;
        glm::mat4		projection;
        glm::mat4		view;
		glm::mat4		unproject;
        glm::vec3		from;
        glm::vec3		to;
		glm::vec3		mouseDirection;
		
		GameEntity*		level;

    };
};
