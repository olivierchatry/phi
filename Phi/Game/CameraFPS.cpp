#include <PreCompile.h>
#include <Game/CameraFPS.h>
#include <Game/Player.h>
#include <Game/Level.h>

namespace Game
{
	void CameraFPS::initialize(Initialize& intialize)
    {
		glfwGetCursorPos(intialize.window, &mPreviousMousePosition.x, &mPreviousMousePosition.y);
    }
    
	void CameraFPS::update(Update& update)
    {
        Level* level = (Level*)update.level;
        
        if (level == NULL)
            return;
        
        float ratio;
        int width, height;
        
        glfwGetFramebufferSize(update.window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);

		// Get mouse position
		glm::dvec2 currentMousePosition;
		glfwGetCursorPos(update.window, &currentMousePosition.x, &currentMousePosition.y);

		glm::dvec2 delta = mPreviousMousePosition - currentMousePosition;
		mPreviousMousePosition = currentMousePosition;
		
		if (glfwGetMouseButton(update.window, GLFW_MOUSE_BUTTON_2))
		{
			mHorizontalAngle += mMouseSpeed * update.delta * float(delta.x);
			mVerticalAngle -= mMouseSpeed * update.delta * float(delta.y);
		}

		glm::quat quaternion(glm::vec3(0.f, mVerticalAngle, mHorizontalAngle));
		glm::mat4 view = glm::toMat4(quaternion);

		glm::vec3 direction(view[0]);
		glm::vec3 up(view[2]);
		glm::vec3 right(view[1]);
		// Up vector : perpendicular to both direction and right
		float actualSpeed = mSpeed;
		if (glfwGetKey(update.window, GLFW_KEY_LEFT_SHIFT))
			actualSpeed = mSpeed * 10.f;

		if (glfwGetKey(update.window, GLFW_KEY_W) == GLFW_PRESS){
			mPosition += direction * update.delta * actualSpeed;
		}
		// Move backward
		if (glfwGetKey(update.window, GLFW_KEY_S) == GLFW_PRESS){
			mPosition -= direction * update.delta * actualSpeed;
		}
		// Strafe right
		if (glfwGetKey(update.window, GLFW_KEY_A) == GLFW_PRESS){
			mPosition += right * update.delta * actualSpeed;
		}
		// Strafe left
		if (glfwGetKey(update.window, GLFW_KEY_D) == GLFW_PRESS){
			mPosition -= right * update.delta * actualSpeed;
		}
		
		
        update.projection = glm::perspective(glm::quarter_pi<float>(), ratio, 0.1f, 100000.0f);
        
               
		update.from = mPosition;
		update.to = mPosition + direction;

		update.view = glm::lookAt(update.from, update.to, up); 

		update.unproject = glm::inverse(update.projection * update.view);
		update.mouseDirection = glm::unProject(glm::vec3(currentMousePosition.x, currentMousePosition.y, 0.1001f), update.view, update.projection, glm::vec4(0, 0, width, height));
		update.mouseDirection = glm::unProject(glm::vec3(currentMousePosition.x, currentMousePosition.y, 1000.f), update.view, update.projection, glm::vec4(0, 0, width, height)) - update.mouseDirection;
		
    }

	void CameraFPS::destroy(Destroy& destroy)
    {
        
    }

	void CameraFPS::render(RenderArg& render)
    {
        
    }
    
}
