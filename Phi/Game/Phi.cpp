// Phi.cpp : Defines the entry point for the console application.
//

#include "PreCompile.h"

#include <Render/ShaderDirectionalPerPixel.h>
#include <Render/ShaderDirectionalPerPixelNoTex.h>

#include <Game/PhiTestGamePart.h>
#include <Game/PhiEditorGamePart.h>
#include <Game/Phi.h>

namespace Game
{
    void        Phi::initialize(Initialize &initialize)
    {
		{

			GamePart* part = new PhiTestGamePart();
			part->initialize(initialize);
			addPart(Test, part);
            mCurrentGamePart = part;
		}

		/*{
			GamePart* part = new PhiEditorGamePart();
			part->initialize(initialize);
			addPart(Editor, part);
			mCurrentGamePart = part;
		}*/

    }
  
    Game::State::List Phi::update(Update& update)
    {
        Game::update(update);
        
        return State::Continue;
    }

    void        Phi::render(RenderArg& render)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0, 0, 0, 0);
        glEnable(GL_DEPTH_TEST);

        render.passFrame = Engine::Normal;
        render.passElement = Engine::Solid;
        Game::render(render);
        
        render.passElement = Engine::Transparent;
        Game::render(render);
    }
};

/*
int main(int argc, char* argv[])
{
    GLFWwindow* window;
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    glfwSetKeyCallback(window, key_callback);
    
    Render::ShaderDirectionalPerPixel shaderDirectional;
    shaderDirectional.create();
    Render::ShaderDirectionalPerPixelNoTex shaderDirectionalNoTex;
    shaderDirectional.create();
    shaderDirectionalNoTex.create();
    
    Game::Level level;
    {
        Game::Level::GenerateArgument arguments;
        level.generate(arguments);
        level.setShader(&shaderDirectional);
    }
    
    Game::Terrain terrain;
    {
        Game::Terrain::GenerateArgument arguments;
        arguments.subDivision = 20.f;
		Render::AABB aabb = level.mLevelAABB;
		aabb.expand(glm::vec3(1, 1, 0));
        terrain.generate(aabb, arguments);
        terrain.setShader(&shaderDirectionalNoTex);
    }

    Camera camera;
    
    Player player;
    
    player.position  = level.getPosition(0.f) + level.getNormal(0.f) * (level.getRadius(0.f) * 1.2f);
    player.direction = level.getPosition(level.mSmallestDelta) + level.getNormal(level.mSmallestDelta) * (level.getRadius(level.mSmallestDelta) * 1.2f) - player.position;
    
    player.sideBoostForce = 0.f;
	player.sideBoostVelocity = glm::vec3(0.f);

    player.gravityForce = 0.f;
	player.gravityVelocity = glm::vec3(0.f);

    player.normal = glm::vec3(0.f, 0.f, 1.f);
    
	player.boosterVelocity = glm::vec3(0.f);
	player.boosterForce = 0.f;
	
    double time = glfwGetTime();
    float oldDeltaOnSpline = 0;

    double lapStartTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1, 0, 1, 0);
        glEnable(GL_DEPTH_TEST);
        
        double currentTime = glfwGetTime();
 double time = glfwGetTime();
 
 float deltaTime = (float)(currentTime - time);
        time = currentTime;
        
        

        player.boosterVelocity += player.direction *  (player.boosterForce - malus) * deltaTime;
		player.boosterVelocity *= 0.98f;
		
		player.sideBoostVelocity += player.right * player.sideBoostForce * deltaTime;
		player.sideBoostVelocity *= 0.98f;

		player.gravityVelocity += player.gravity * player.gravityForce * deltaTime;
		player.gravityVelocity *= 0.98f;
        glm::vec3 previous = player.position;
		player.position = player.position + (player.boosterVelocity + player.sideBoostVelocity + player.gravityVelocity) * deltaTime;
		float deltaOnSpline = oldDeltaOnSpline;

        level.findNearestDelta(player.position, deltaOnSpline, 1);
		if (deltaOnSpline > 1.f)
        {
            deltaOnSpline -= 1.f;
            level.findNearestDelta(player.position, deltaOnSpline, 1);
            float deltaToStart = 1.f - oldDeltaOnSpline;
            float deltaTotal = deltaTime + deltaToStart;
            float percent = deltaToStart / deltaTotal;
            double lapTime = currentTime + (time - currentTime) * percent;
            printf("(%f -> %f) !!! LAP %0.4lf\n", oldDeltaOnSpline, deltaOnSpline, lapTime - lapStartTime);
            lapStartTime = lapTime;
        }
        
        time = currentTime;
        
		oldDeltaOnSpline = deltaOnSpline;
        glm::vec3	pointOnSpline = level.getPosition(deltaOnSpline);
        
        player.direction = glm::normalize(player.direction + glm::normalize(level.getPosition(deltaOnSpline + level.mSmallestDelta * 2.f) - pointOnSpline));
        float radius = level.getRadius(deltaOnSpline);
        
        glm::vec3 vecToPoint = glm::normalize(player.position - pointOnSpline);
        glm::vec3 collisionPoint	= vecToPoint * radius * 1.2f + pointOnSpline;
        glm::vec3 hittingPoint		= vecToPoint * radius + pointOnSpline;
        float distancePlayerToSpline = glm::distance(pointOnSpline, player.position);
        float distanceTrackToSpline = glm::distance(pointOnSpline, hittingPoint);
        
        player.gravityForce = glm::distance(collisionPoint, player.position);
		if (player.gravityForce > std::numeric_limits<float>::epsilon())
			player.gravity = glm::normalize(collisionPoint - player.position);
		else
			player.gravity = glm::vec3(0);		
		
		player.gravityForce *= 12.f;
		if (distanceTrackToSpline > distancePlayerToSpline)
            player.position = hittingPoint;
        
        player.normal = glm::normalize(vecToPoint);
        player.right = glm::normalize(glm::cross(player.direction, player.normal));
        
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			player.boosterForce = glm::clamp(player.boosterForce + 10.f, 0.f, 3000.f);
        else
			player.boosterForce = 0;
        
        bool stopForce = true;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			player.sideBoostForce = -player.boosterForce * 1.2f, stopForce = false;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			player.sideBoostForce = player.boosterForce * 1.2f, stopForce = false;
        if (stopForce)
			player.sideBoostForce = 0.f;
		velocitySmoothed.add(player.boosterVelocity);
        
		float fov = (glm::clamp(glm::length(velocitySmoothed.get()), 0.f, 4000.f) / 4000.0f) * glm::quarter_pi<float>() * 1.5f;

		camera.mProjection = glm::perspective(glm::quarter_pi<float>() + fov, ratio, 0.1f, 10000.0f);
        

        glm::vec3 from = player.position;

        
        {
            level.findNearestDelta(from, deltaOnSpline, 1);
            float       radiusCamera = level.getRadius(deltaOnSpline);
            glm::vec3   pointCamera = level.getPosition(deltaOnSpline);
            glm::vec3   normalCamera = glm::normalize(from - pointCamera);
            glm::vec3   cameraCollisionPoint = normalCamera * radiusCamera * 1.1f + pointCamera;


            if (glm::distance(pointCamera, cameraCollisionPoint) >
                glm::distance(pointCamera, from))
                from = cameraCollisionPoint;
        }
		glm::vec3 to = from + player.direction;
        
        camera.mView = glm::lookAt(from, to, player.normal);
        //camera.mView = glm::lookAt(glm::vec3(0, 0, 200),
        //	level.get(angle + 0.0001), glm::vec3(0, 1, 0));
        
        
        glm::vec3 lightDirection(glm::normalize(glm::vec3(0,0,1)));

		{
			Render::Material material;
			material.MaterialAmbient = glm::vec4(0.2f);
			material.MaterialDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			material.MaterialSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			material.MaterialShininess = 64.f;

            shaderDirectionalNoTex.bind();
			shaderDirectionalNoTex.setMaterial(material);
			shaderDirectionalNoTex.setLightDirection(lightDirection);
			shaderDirectionalNoTex.setCamera(camera);
			for (auto chunk : terrain.mTerrainRenderables)
			{
				Engine::VertexArray::Binder  bind1(chunk->vertexArray);

				glCullFace(GL_BACK);
				glDrawArrays(GL_TRIANGLES, 0, chunk->count);
			}


		}

        {
			Render::Material material;
			material.MaterialAmbient = glm::vec4(0.2f);
			material.MaterialDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			material.MaterialSpecular = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			material.MaterialShininess = 64.f;
			
			shaderDirectional.bind();
            shaderDirectional.setLightDirection(lightDirection);
            shaderDirectional.setMaterial(material);
            shaderDirectional.setCamera(camera);
            
            level.computeChunkDistanceToCamera(from, glm::normalize(to - from));
            std::sort(level.mTrackChunks.begin(), level.mTrackChunks.end(), Game::Level::sortByDistance);
            
            shaderDirectional.mShader.setUniform(shaderDirectional.mTexture, 0);
            level.mTexture.bind(0);
            
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_CULL_FACE);

            for (auto chunk : level.mTrackChunks)
            {
                Engine::VertexArray::Binder  bind1(chunk->vertexArray);
                Engine::IndexBuffer::Binder  bind3(chunk->indexBuffer);
                
                
                glCullFace(GL_FRONT);
                glDrawElements(GL_TRIANGLES, chunk->count, GL_UNSIGNED_SHORT, 0);
                glCullFace(GL_BACK);
                glDrawElements(GL_TRIANGLES, chunk->count, GL_UNSIGNED_SHORT, 0);
            }
            
            
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    level.destroy();
    terrain.destroy();
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
*/
