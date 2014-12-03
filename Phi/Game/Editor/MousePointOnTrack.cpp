#include <PreCompile.h>
#include <Game/Level.h>
#include <Game/Editor/MousePointOnTrack.h>
#include <Math/AABB.h>
#include <Utils/Utils.h>

namespace Editor
{
	
	void MousePointOnTrack::initialize(Game::Initialize &initialize)
	{
		Math::AABB aabb;

		aabb.reset();
		aabb.add(glm::vec3(-1.f));
		aabb.add(glm::vec3(1.f));
		
		aabb.expand(glm::vec3(10.f));
		std::vector<float> vs;
		
		Utils::GenerateCube(aabb, vs);
		Utils::GenerateNormals(&vs[0], 6, vs.size() / 6, 0, 3);
		
		mRenderable.vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
		mRenderable.vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
		mRenderable.count = vs.size() / 6;
		mWasPressed = false;
	}

	
	void MousePointOnTrack::destroy(Game::Destroy &destroy)
	{
		
	}
	
	void MousePointOnTrack::setShader(Render::IShaderDirectionalLightNoTex *shader)
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

	
	void MousePointOnTrack::render(Game::RenderArg &render)
	{
		if (render.passElement == Engine::Solid
            && render.passFrame == Engine::Normal
            && mHavePosition)
		{
			Render::Material material;
			material.MaterialAmbient = glm::vec4(0.2f);
			material.MaterialDiffuse = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
			material.MaterialSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			material.MaterialShininess = 64.f;
			
			mShader->bind();
			mShader->setMaterial(material);
			mShader->setLightDirection(render.sunDirection);

			Engine::VertexArray::Binder  bind1(mRenderable.vertexArray);
			glm::mat4 matrix = glm::translate(mPositionOnSpline);
			mShader->setMatrices(render.projection, render.view, matrix);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glDrawArrays(GL_TRIANGLES, 0, mRenderable.count);
		}
	}
	
	void MousePointOnTrack::update(Game::Update& update)
	{
		if (update.level == NULL)
			return;
				
		Game::Level* level = (Game::Level*)update.level;
		float currentLength = std::numeric_limits<float>::max();

		Game::Level::TrackChunkRenderable* chunk = NULL;
		for (auto& p : level->mTrackChunks)
		{

			glm::vec3 collision;
			if (Utils::RayIntersectBoundingBox(update.mouseProjectedPosition, update.mouseProjectedDirection, p->aabb, collision))
			{
				float length = glm::distance2(collision, update.mouseProjectedPosition);
				if (length < currentLength)
				{
					chunk = p;
					currentLength = length;
					mPositionOnBox = collision;
				}
			}
		}
		mHavePosition = false;
		bool pressed = glfwGetMouseButton(update.window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
		
		if (chunk && !update.mouseTaken)
		{
			mDeltaStart = level->mTrack.points[chunk->id];
			mDeltaEnd = level->getPosition(chunk->deltaEnd);
			float delta = chunk->deltaStart;
			mHavePosition = level->findNearestDelta(mPositionOnBox, delta, 1, false);
			float radius = 0.f;
			if (mHavePosition)
			{
				mPositionOnSpline = level->getPosition(delta);
				radius = level->getRadius(delta);
			}
			
			if (mWasPressed && !pressed)
			{
				level->addControlPoint(chunk->id, delta, mPositionOnSpline, radius);
				level->generate();
				level->setShader(level->mShader);
			}
		}

		mWasPressed = pressed;
		//mMatrix = glm::translate(pointOnSpline);
	}
};