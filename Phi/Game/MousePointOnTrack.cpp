#include <PreCompile.h>
#include <Game/Level.h>
#include <Game/MousePointOnTrack.h>
#include <Render/AABB.h>
#include <Utils/Utils.h>

namespace Game
{
	
	void MousePointOnTrack::initialize(Initialize &initialize)
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
	}
	
	void MousePointOnTrack::destroy(Destroy &destroy)
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

	
	void MousePointOnTrack::render(RenderArg &render)
	{
		if (render.passElement == Engine::Solid)
		{
			Render::Material material;
			material.MaterialAmbient = glm::vec4(0.2f);
			material.MaterialDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			material.MaterialSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			material.MaterialShininess = 64.f;
			
			mShader->bind();
			mShader->setMaterial(material);
			mShader->setLightDirection(render.sunDirection);

			mShader->setMatrices(render.projection, render.view, mMatrix);

			Engine::VertexArray::Binder  bind1(mRenderable.vertexArray);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glDrawArrays(GL_TRIANGLES, 0, mRenderable.count);
		}
	}
	
	void MousePointOnTrack::update(Update& update)
	{
		if (update.level == NULL)
			return;
		Level* level = (Level*) update.level;
		//mMatrix = glm::translate(pointOnSpline);
	}
};