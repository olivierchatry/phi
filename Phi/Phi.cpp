// Phi.cpp : Defines the entry point for the console application.
//

#include "PreCompile.h"

#include <Render/ShaderDirectionalPerPixel.h>
#include <Game/Level.h>


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
    
    Game::Level level;
    level.generate(shaderDirectional);
    
    Camera camera;

    float playerDelta = 0;
    
    
    float playerVelocity = 0;
    float playerAcceleration = 0;
    double time = glfwGetTime();
    
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
        float delta = (float) (currentTime - time);
        time = currentTime;
        
        {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                playerAcceleration += 10.0f;
            else
                playerAcceleration = 0;
            playerAcceleration = glm::clamp(playerAcceleration, 0.0f, 1000.0f);
            
            playerVelocity  += playerAcceleration * delta;
            playerDelta += (playerVelocity * delta) / level.mTotalLength;
            playerVelocity *= 0.97f;
        }

        camera.mProjection = glm::perspective(glm::quarter_pi<float>(), ratio, 0.1f, 10000.0f);
		glm::vec3 from = level.getPosition(playerDelta)
                    + level.getNormal(playerDelta) * (level.getRadius(playerDelta) * 1.1f);

        glm::vec3 to = level.getPosition(playerDelta + level.mSmallestDelta)
                    + level.getNormal(playerDelta + level.mSmallestDelta) * (level.getRadius(playerDelta + level.mSmallestDelta) * 1.1f);


		camera.mView = glm::lookAt(from, to, level.getNormal(playerDelta));
		//camera.mView = glm::lookAt(glm::vec3(0, 0, 200),
		//	level.get(angle + 0.0001), glm::vec3(0, 1, 0));

        Render::IShaderDirectionalLight::Material material;
        material.MaterialAmbient = glm::vec4(0.2f);
		material.MaterialDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		material.MaterialSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        material.MaterialShininess = 64.f;
		
        {
			shaderDirectional.bind();
            glm::vec3 lightDirection(glm::normalize(glm::vec3(0,0,1)));
            shaderDirectional.setLightDirection(lightDirection);
			shaderDirectional.setMaterial(material);
			shaderDirectional.setCamera(camera);
			
			level.computeChunkDistanceToCamera(from, glm::normalize(to - from));
			std::sort(level.mTrackChunks.begin(), level.mTrackChunks.end(), Game::Level::sortByDistance);
					
			// shaderDirectional.mShader.setUniform(shaderDirectional.getVsUV(), 0);
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

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
