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
	Game::Level::GenerateArgument arguments;
	level.generate(shaderDirectional, arguments);

	Camera camera;

	float playerDelta = 0;
	float playerVelocity = 0;
	float playerAcceleration = 0;

	struct Player
	{
		glm::vec3 points[2];
		float	force[2];
		float	velocityTurn[2];

		glm::vec3 normal;
		glm::vec3 right;

		float	  acceleration;
		float	  velocity;

		glm::vec3 direction() {
			return glm::normalize(points[1] - points[0]);
		}

		glm::vec3& position() {
			return points[0];
		}
	};

	Player player;

	player.points[0] = level.getPosition(playerDelta) + level.getNormal(playerDelta) * (level.getRadius(playerDelta) * 1.1f);
	player.points[1] = level.getPosition(playerDelta + level.mSmallestDelta) + level.getNormal(playerDelta + level.mSmallestDelta) * (level.getRadius(playerDelta + level.mSmallestDelta) * 1.1f);

	player.force[0] = 0.f;
	player.force[1] = 0.f;

	player.velocityTurn[0] = 0.f;
	player.velocityTurn[1] = 0.f;
	player.normal = glm::vec3(0.f, 0.f, 1.f);

	player.velocity = 0.f;
	player.acceleration = 0.f;

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
		float deltaTime = (float)(currentTime - time);
		time = currentTime;

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			player.acceleration = glm::clamp(player.acceleration + 10.f, 0.f, 1000.f);
		else
			player.acceleration = 0;	

		bool stopForce = true;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			player.force[1] = glm::clamp(player.force[1] + 1.f, 0.f, 10.f), stopForce = false;
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			player.force[1] = glm::clamp(player.force[1] - 1.f, 0.f, 10.f), stopForce = false;
		if (stopForce)
			player.force[1] = 0;
			
		
		player.velocity += player.acceleration * deltaTime;
		player.velocity *= 0.97f;

		for (int i = 0; i < 2; ++i)
		{
			glm::vec3 nextPosition = player.points[i] + (player.direction()) * (player.velocity * deltaTime);
			float deltaOnSpline;
			level.findNearestDelta(nextPosition, deltaOnSpline, 3);
			
			glm::vec3	pointOnSpline = level.getPosition(deltaOnSpline);
			float		radius = level.getRadius(deltaOnSpline);
			
			glm::vec3 vecToPoint = glm::normalize(nextPosition - pointOnSpline);
			glm::vec3 collisionPoint = vecToPoint * radius * 1.1f + pointOnSpline;
			player.points[i] = collisionPoint;
			
			player.normal = glm::normalize(vecToPoint);
		}

		player.right = glm::normalize(glm::cross(player.direction(), player.normal));
		player.normal = glm::normalize(glm::cross(player.right, player.direction()));

		/*


        {
		            
            

			
			float deltaOnSpline;
			level.findNearestDelta(nextPlayerPosition, deltaOnSpline, 3);
			
			// gravity !
			glm::vec3 pointOnSpline = level.getPosition(deltaOnSpline);
			float radius = level.getRadius(deltaOnSpline);

			glm::vec3 vecToPoint = glm::normalize(nextPlayerPosition - pointOnSpline);
			glm::vec3 collisionPoint = vecToPoint * radius * 1.1f + pointOnSpline;
			
			float distance = glm::distance(nextPlayerPosition, collisionPoint);
			glm::vec3 gravity(0);

			if (distance > std::numeric_limits<float>::epsilon())
				gravity = glm::normalize(collisionPoint - nextPlayerPosition);								
			
			nextPlayerPosition += gravity * distance * delta;
			glm::vec3 playerDirection = (level.getPosition(deltaOnSpline + level.mSmallestDelta) + level.getNormal(deltaOnSpline + level.mSmallestDelta) * (level.getRadius(deltaOnSpline + level.mSmallestDelta) * 1.1f)) - playerPosition;

			distance = glm::distance(nextPlayerPosition, playerPosition);
			playerPosition = nextPlayerPosition;
			// playerDirection = glm::normalize(playerPosition - nextPlayerPosition);
            
			normal = vecToPoint;
        }
*/

        camera.mProjection = glm::perspective(glm::quarter_pi<float>(), ratio, 0.1f, 10000.0f);
		glm::vec3 from	= player.position();		
		glm::vec3 to = player.position() + player.direction();


		camera.mView = glm::lookAt(from, to, player.normal);
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
