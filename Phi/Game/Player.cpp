#include <PreCompile.h>
#include <Game/Level.h>
#include <Game/Player.h>
#include <Math/AABB.h>
#include <Utils/Utils.h>

namespace Game
{
	
	void Player::initialize(Initialize &initialize)
	{
		Math::AABB aabb;

		aabb.reset();
		aabb.add(glm::vec3(-1.5f, -1.f, 0.f));
		aabb.add(glm::vec3(1.f, 1.f, 1.f));
		
		std::vector<float> vs;
		
		Utils::GenerateCube(aabb, vs);
		Utils::GenerateNormals(&vs[0], 6, vs.size() / 6, 0, 3);
		
		mRenderable.vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
		mRenderable.vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
		mRenderable.count = vs.size() / 6;
		Level* level = (Level*) initialize.level;
		
		mPhysic.position  = level->getPosition(0.f)
		+ level->getNormal(0.f) * (level->getRadius(0.f) * 1.2f);
		mPhysic.direction = level->getPosition(level->smallestDelta())
		+ level->getNormal(level->smallestDelta()) * (level->getRadius(level->smallestDelta()) * 2.f) - mPhysic.position;
		
		mPhysic.sideBoostForce = 0.f;
		mPhysic.sideBoostVelocity = glm::vec3(0.f);
		
		mPhysic.gravityForce = 0.f;
		mPhysic.gravityVelocity = glm::vec3(0.f);
		
		mPhysic.normal = glm::vec3(0.f, 0.f, 1.f);
		
		mPhysic.boosterVelocity = glm::vec3(0.f);
		mPhysic.boosterForce = 0.f;

		mPreviousDeltaOnSpline = 0.f;
		mDirection = mPhysic.direction;
        
        mDeltas.resize(100);
        memset(&mDeltas.front(), 0, mDeltas.size()*sizeof(float));
        mDeltaOffset = 0;
        mDeltaMin = std::numeric_limits<float>::max();
        mDeltaMax = -std::numeric_limits<float>::max();
    }
	
	void Player::destroy(Destroy &destroy)
	{
		
	}
	
	void Player::setShader(Render::IShaderDirectionalLightNoTex *shader)
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

	
	void Player::render(RenderArg &render)
	{
		if (render.passElement == Engine::Solid
			&& render.passFrame == Engine::Normal)
		{
			Render::Material material;
			material.MaterialAmbient = glm::vec4(0.2f);
			material.MaterialDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			material.MaterialSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			material.MaterialShininess = 64.f;
			
			mShader->bind();
			mShader->setMaterial(material);
			mShader->setLightDirection(render.sunDirection);
            mShader->setZPlane(render.near, render.far);

			mShader->setMatrices(render.projection, render.view, mMatrix);

			Engine::VertexArray::Binder  bind1(mRenderable.vertexArray);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glDrawArrays(GL_TRIANGLES, 0, mRenderable.count);
		}
	}
	
	void Player::update(Update& update)
	{
		if (update.level == NULL)
			return;
		update.player = this;
		Level* level = (Level*) update.level;
		float deltaTime = update.delta;
		
		mPhysic.boosterVelocity += mPhysic.direction *  (mPhysic.boosterForce) * deltaTime;
		mPhysic.boosterVelocity *= 0.97f;
		
		mPhysic.sideBoostVelocity += mPhysic.right * mPhysic.sideBoostForce * deltaTime;
		mPhysic.sideBoostVelocity *= 0.97f;
		
		mPhysic.gravityVelocity += mPhysic.gravity * mPhysic.gravityForce * deltaTime;
		mPhysic.gravityVelocity *= 0.97f;
		mPhysic.previousPosition = mPhysic.position;
		
		mPhysic.position = mPhysic.position + (mPhysic.boosterVelocity + mPhysic.sideBoostVelocity + mPhysic.gravityVelocity) * deltaTime;
		
		float deltaOnSpline = mPreviousDeltaOnSpline;
		level->findNearestDelta(mPhysic.position, deltaOnSpline, 1);
		if (deltaOnSpline > 1.f)
		{
			deltaOnSpline -= 1.f;
			level->findNearestDelta(mPhysic.position, deltaOnSpline, 1);
			// float deltaToStart = 1.f - mPreviousDeltaOnSpline;
			// float deltaTotal = deltaTime + deltaToStart;
			// float percent = deltaToStart / deltaTotal;
			// double lapTime = currentTime + (time - currentTime) * percent;
			// printf("(%f -> %f) !!! LAP %0.4lf\n", mPreviousDeltaOnSpline, deltaOnSpline, lapTime - lapStartTime);
			// lapStartTime = lapTime;
		}

        glm::vec3	pointOnSpline = level->getPosition(deltaOnSpline);
		if (mPreviousDeltaOnSpline != deltaOnSpline)
			mPhysic.direction = glm::normalize(pointOnSpline - level->getPosition(mPreviousDeltaOnSpline));
		
		if ((deltaOnSpline - mPreviousDeltaOnSpline) > 0.f)
			mDirection = glm::normalize(mPhysic.previousPosition - mPhysic.position);
		/*else
			mDirection = mPhysic.direction;*/
		
		//mDirection = mDirection;


		float radius = level->getRadius(deltaOnSpline);
		glm::vec3 vecToPoint = mPhysic.position - pointOnSpline;
		// float currentRadius = glm::length(vecToPoint);
		vecToPoint = glm::normalize(vecToPoint);

        bool showWindow = true;
        {
            float value = deltaOnSpline - mPreviousDeltaOnSpline;
            if (value < 0.f)
                value = 1.f + value;
            
            mDeltaMin = glm::min(value, mDeltaMin);
            mDeltaMax = glm::max(value, mDeltaMax);
        
            mDeltas[mDeltaOffset] = value;
            mDeltaOffset = (mDeltaOffset + 1) % mDeltas.size();
            ImGui::Begin("Player", &showWindow, ImVec2(100,100), -1.0f, ImGuiWindowFlags_ShowBorders);
            ImGui::SetWindowFontScale(2.f);
            ImGui::SliderFloat("delta", &deltaOnSpline, 0.f, 1.f);
            ImGui::SliderFloat("radius", &radius, 0.f, 2000.f);
            ImGui::SliderFloat3("position", &vecToPoint.x, 0.f, 1.f);
            ImGui::Text("delta %f max %f", value, mDeltaMax);
            ImGui::PlotLines("deltas", &mDeltas.front(), (int)mDeltas.size(), (int)mDeltaOffset, "", 0.f, mDeltaMax, ImVec2(0.f,70.f));
            ImGui::End();
        }
		
        mPreviousDeltaOnSpline = deltaOnSpline;

		glm::vec3 collisionPoint	= vecToPoint * radius * 1.2f + pointOnSpline;
		// glm::vec3 hittingPoint		= vecToPoint * radius * 1.1f + pointOnSpline;
		
		// float distancePlayerToSpline = glm::distance(pointOnSpline, mPhysic.position);
		// float distanceTrackToSpline = glm::distance(pointOnSpline, hittingPoint);
		
		mPhysic.gravityForce = glm::distance(collisionPoint, mPhysic.position);
		if (mPhysic.gravityForce > std::numeric_limits<float>::epsilon())
			mPhysic.gravity = glm::normalize(collisionPoint - mPhysic.position);
		else
			mPhysic.gravity = glm::vec3(0);
		
		mPhysic.gravityForce *= 12.f;
		
		//if (distanceTrackToSpline > distancePlayerToSpline)
		//	mPhysic.position = hittingPoint;
		
		mPhysic.normal = glm::normalize(vecToPoint);
		mPhysic.right = glm::normalize(glm::cross(mPhysic.direction, mPhysic.normal));
		
		if (glfwGetKey(update.window, GLFW_KEY_UP) == GLFW_PRESS)
			mPhysic.boosterForce = glm::clamp(mPhysic.boosterForce + 10.f, 0.f, 3000.f);
		else
			mPhysic.boosterForce = 0;
		
		bool stopForce = true;
		if (glfwGetKey(update.window, GLFW_KEY_LEFT) == GLFW_PRESS)
			mPhysic.sideBoostForce = -mPhysic.boosterForce * 1.2f, stopForce = false;
		if (glfwGetKey(update.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			mPhysic.sideBoostForce = mPhysic.boosterForce * 1.2f, stopForce = false;
		if (stopForce)
			mPhysic.sideBoostForce = 0.f;

		// mPreviousDirection = mPhysic.direction;
		
		glm::vec3 right = glm::normalize(glm::cross(mPhysic.normal, mDirection));
		glm::vec3 normal = glm::normalize(glm::cross(mDirection, right));

		mMatrix = glm::colMajor4(glm::vec4(mDirection, 0.f),
								 glm::vec4(right, 0.f),
								 glm::vec4(normal,0.f),
								 glm::vec4(mPhysic.position,1.f));
		//mMatrix = glm::translate(pointOnSpline);
	}
};