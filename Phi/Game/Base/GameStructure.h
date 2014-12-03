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
        float           delta;
        GameEntity*     level;
        GameEntity*     player;
        GameEntity*     editor;
        
        GLFWwindow*     window;
        
        glm::mat4       projection;
        glm::mat4       view;
        glm::vec3       from;
        glm::vec3       to;

		glm::mat4       unproject;
        glm::vec3       mouseProjectedPosition;
		glm::vec3       mouseProjectedDirection;

		glm::vec3       centerProjectedPosition;
		glm::vec3       centerProjectedDirection;

		bool			mouseTaken;
        float           far;
        float           near;
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
			mouseProjectedPosition = update.mouseProjectedPosition;
            mouseProjectedDirection = update.mouseProjectedDirection;
			centerProjectedPosition = update.centerProjectedPosition;
			centerProjectedDirection = update.centerProjectedDirection;
            far = update.far;
            near = update.near;
		}

        Engine::PassFrame   passFrame;
        Engine::PassElement passElement;

        glm::vec3		sunDirection;
        glm::mat4		projection;
        glm::mat4		view;
		glm::mat4		unproject;
        glm::vec3		from;
        glm::vec3		to;
        glm::vec3       mouseProjectedPosition;
		glm::vec3		mouseProjectedDirection;
		
		glm::vec3       centerProjectedPosition;
		glm::vec3       centerProjectedDirection;
        float           far;
        float           near;

		GameEntity*		level;

    };
};
