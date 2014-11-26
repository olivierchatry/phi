// Phi.cpp : Defines the entry point for the console application.
//

#include "PreCompile.h"

#include <Render/ShaderDirectionalPerPixel.h>
#include <Render/ShaderDirectionalPerPixelNoTex.h>

#include <Game/Level.h>
#include <Game/Terrain.h>


static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}




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
    shaderDirectionalNoTex.create();
    
    Game::Level level;
    {
        Game::Level::GenerateArgument arguments;
        level.generate(shaderDirectional, arguments);
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
    
    struct Player
    {
        glm::vec3	position;
        glm::vec3	direction;
        glm::vec3   force;
        
        glm::vec3	gravity;
        float		gravityForce;
        
        glm::vec3	normal;
        glm::vec3	right;
        
        float		acceleration;
        glm::vec3	velocity;
        
    };
    
    Player player;
    
    player.position  = level.getPosition(0.f) + level.getNormal(0.f) * (level.getRadius(0.f) * 1.1f);
    player.direction = level.getPosition(level.mSmallestDelta) + level.getNormal(level.mSmallestDelta) * (level.getRadius(level.mSmallestDelta) * 1.1f) - player.position;
    
    player.force = glm::vec3(0.f);
    player.gravityForce = 0.f;
    
    player.normal = glm::vec3(0.f, 0.f, 1.f);
    
    player.velocity = glm::vec3(0.f);
    player.acceleration = 0.f;
    
    double time = glfwGetTime();
    float oldDelta = 0;
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /*glClearColor(1, 0, 1, 0);*/
        glEnable(GL_DEPTH_TEST);
        
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - time);
        time = currentTime;
        
        
        
        player.velocity += (player.direction + player.force) * player.acceleration * deltaTime;
        player.velocity += player.gravity * player.gravityForce * deltaTime;
        /*player.velocity += player.right * player.force * deltaTime;*/
        player.velocity *= 0.97f;
        
        
        float deltaOnSpline = oldDelta;
        level.findNearestDelta(player.position, deltaOnSpline, 1);
        oldDelta = deltaOnSpline;
        glm::vec3	pointOnSpline = level.getPosition(deltaOnSpline);
        
        player.direction = glm::normalize(player.direction + glm::normalize(level.getPosition(deltaOnSpline + level.mSmallestDelta) - pointOnSpline));
        
        float radius = level.getRadius(deltaOnSpline);
        
        glm::vec3 vecToPoint = glm::normalize(player.position - pointOnSpline);
        glm::vec3 collisionPoint = vecToPoint * radius * 1.2f + pointOnSpline;
        glm::vec3 hittingPoint = vecToPoint * radius + pointOnSpline;
        float distancePlayerToSpline = glm::distance(pointOnSpline, player.position);
        float distanceTrackToSpline = glm::distance(pointOnSpline, hittingPoint);
        
        player.gravityForce = glm::distance(collisionPoint, player.position);
        
        if (player.gravityForce > std::numeric_limits<float>::epsilon())
        {
            player.gravityForce = player.gravityForce * 30.f;
            player.gravity = glm::normalize(collisionPoint - player.position);
        }
        else
            player.gravity = glm::vec3(0);
        
        if (distanceTrackToSpline > distancePlayerToSpline)
            player.position = hittingPoint;
        
        player.normal = glm::normalize(vecToPoint);
        player.right = glm::normalize(glm::cross(player.direction, player.normal));
        
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            player.acceleration = glm::clamp(player.acceleration + 10.f, 0.f, 3000.f);
        else
            player.acceleration = 0;
        
        bool stopForce = true;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            player.force = glm::clamp(player.force - player.right * 0.5f, -4.f, 4.f), stopForce = false;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            player.force = glm::clamp(player.force + player.right * 0.5f, -4.f, 4.f), stopForce = false;
        if (stopForce)
            player.force = glm::vec3(0);
        
        camera.mProjection = glm::perspective(glm::quarter_pi<float>() + glm::length(player.velocity) / 2000.0f, ratio, 0.1f, 100000.0f);
        player.position = player.position + player.velocity * deltaTime;

        glm::vec3 from = player.position - player.direction * glm::length(player.velocity) / 5.0f;

        
        {
            level.findNearestDelta(from, deltaOnSpline, 1);
            float       radiusCamera = level.getRadius(deltaOnSpline);
            glm::vec3   pointCamera = level.getPosition(deltaOnSpline);
            glm::vec3   normalCamera = glm::normalize(from - pointCamera);
            glm::vec3   cameraCollisionPoint = normalCamera * radiusCamera * 1.25f + pointCamera;


            /*if (glm::distance(pointCamera, cameraCollisionPoint) >
                glm::distance(pointCamera, from))
                from = cameraCollisionPoint;*/
        }
        glm::vec3 to = from + player.direction * 2.f;
        
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
            
            for (auto chunk : level.mTrackChunks)
            {
                Engine::VertexArray::Binder  bind1(chunk->vertexArray);
                Engine::IndexBuffer::Binder  bind3(chunk->indexBuffer);
                
                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                glDrawElements(GL_TRIANGLES, chunk->count, GL_UNSIGNED_SHORT, 0);
            }
            
            
            for (auto chunk : level.mTrackChunks)
            {
                Engine::VertexArray::Binder  bind1(chunk->vertexArray);
                Engine::IndexBuffer::Binder  bind3(chunk->indexBuffer);
                
                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                glEnable(GL_CULL_FACE);
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
