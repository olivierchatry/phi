#include <PreCompile.h>
#include <Game/CameraFPS.h>
#include <Game/Player.h>
#include <Game/Level.h>
#include <Utils/Utils.h>

namespace Game
{
	void CameraFPS::initialize(Initialize& intialize)
    {
		glfwGetCursorPos(intialize.window, &mPreviousMousePosition.x, &mPreviousMousePosition.y);
        Render::AABB aabb;
        
        aabb.reset();
        aabb.add(glm::vec3(-100.f));
        aabb.add(glm::vec3(100.f));
        
        std::vector<float> vs;
        
        Utils::GenerateCube(aabb, vs);
        Utils::GenerateNormals(&vs[0], 6, vs.size() / 6, 0, 3);
        
        mRenderable.vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
        mRenderable.vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
        mRenderable.count = vs.size() / 6;
    }
    
    void CameraFPS::setShader(Render::IShaderDirectionalLightNoTex *shader)
    {
        mShader = shader;
        mRenderable.vertexArray.destroy();
        mRenderable.vertexArray.create();
        {
            Engine::VertexArray::Binder     bind1(mRenderable.vertexArray);
            Engine::VertexBuffer::Binder    bind2(mRenderable.vertexBuffer);
            
            mRenderable.vertexArray.attrib(shader->getVsPosition(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
            mRenderable.vertexArray.attrib(shader->getVsNormal(), 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), 3 * sizeof(float));
        }
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
		
		
        update.projection = glm::perspective(glm::quarter_pi<float>(), ratio, 10.f, 100000.0f);
        
               
		update.from = mPosition;
		update.to = mPosition + direction;

		update.view = glm::lookAt(update.from, update.to, up); 

		update.unproject = glm::inverse(update.projection * update.view);
        int windowWidth, windowHeight;
        glfwGetWindowSize(update.window, &windowWidth, &windowHeight);
        
        currentMousePosition.x *= (width / (double) windowWidth);
        currentMousePosition.y *= (height / (double) windowHeight);
        currentMousePosition.y = height - currentMousePosition.y ;

        update.mouseProjectedPosition =
            glm::unProject(glm::vec3(currentMousePosition.x, currentMousePosition.y, 0.0f),
                           update.view,
                           update.projection,
                           glm::vec4(0, 0, width, height));
        
		update.mouseProjectedDirection =
            glm::unProject(glm::vec3(currentMousePosition.x, currentMousePosition.y, 0.99f),
                           update.view,
                           update.projection,
                           glm::vec4(0, 0, width, height));
        
        update.mouseProjectedDirection = glm::normalize(update.mouseProjectedDirection - update.mouseProjectedPosition);
		
    }

	void CameraFPS::destroy(Destroy& destroy)
    {
        
    }

	void CameraFPS::render(RenderArg& render)
    {
        if (render.passElement == Engine::Solid)
        {
            Render::Material material;
            material.MaterialAmbient = glm::vec4(0.2f);
            material.MaterialDiffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            material.MaterialSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            material.MaterialShininess = 64.f;
            
            mShader->bind();
            mShader->setMaterial(material);
            mShader->setLightDirection(render.sunDirection);
            glm::mat4 matrix = glm::translate(render.mouseProjectedDirection);
            
            mShader->setMatrices(render.projection, render.view, matrix);
            
            Engine::VertexArray::Binder  bind1(mRenderable.vertexArray);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glDrawArrays(GL_TRIANGLES, 0, mRenderable.count);
        }
        
    }
    
}
