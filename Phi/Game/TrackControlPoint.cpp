#include <PreCompile.h>
#include <Game/Level.h>
#include <Game/TrackControlPoint.h>
#include <Render/AABB.h>
#include <Utils/Utils.h>

namespace Game
{
	
	void TrackControlPoint::initialize(Initialize &initialize)
	{
		Render::AABB aabb;

		aabb.reset();
		aabb.add(glm::vec3(-10.f, -10.f, -10.f));
		aabb.add(glm::vec3(10.f, 10.f, 10.f));
		
		std::vector<float> vs;
		
		Utils::GenerateCube(aabb, vs);
		Utils::GenerateNormals(&vs[0], 6, vs.size() / 6, 0, 3);
		
		mRenderablePoint.vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
		mRenderablePoint.vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
		mRenderablePoint.count = vs.size() / 6;
		mRenderablePoint.color = glm::vec4(1.f, 0.f, 1.f, 1.f);

		float arrowLen = 40.f;
		aabb.reset();
		aabb.add(glm::vec3(0, -1.f, -1.f));
		aabb.add(glm::vec3(arrowLen, 1.f, 1.f));

		vs.clear();
		Utils::GenerateCube(aabb, vs);
		Utils::GenerateNormals(&vs[0], 6, vs.size() / 6, 0, 3);

		mRenderableSide.vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
		mRenderableSide.vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
		mRenderableSide.count = vs.size() / 6;
		mRenderableSide.color = glm::vec4(0.f, 1.f, 0.f, 1.f);

		aabb.reset();
		aabb.add(glm::vec3(-1.f, 0.f, -1.f));
		aabb.add(glm::vec3(1.f, arrowLen, 1.f));
		vs.clear();
		Utils::GenerateCube(aabb, vs);
		Utils::GenerateNormals(&vs[0], 6, vs.size() / 6, 0, 3);

		mRenderableTop.vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
		mRenderableTop.vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
		mRenderableTop.count = vs.size() / 6;
		mRenderableTop.color = glm::vec4(1.f, 0.f, 0.f, 1.f);

		aabb.reset();
		aabb.add(glm::vec3(-1.f, -1.f, 0.f));
		aabb.add(glm::vec3(1.f, 1.f, arrowLen));
		vs.clear();
		Utils::GenerateCube(aabb, vs);
		Utils::GenerateNormals(&vs[0], 6, vs.size() / 6, 0, 3);

		mRenderableFront.vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
		mRenderableFront.vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
		mRenderableFront.count = vs.size() / 6;
		mRenderableFront.color = glm::vec4(0.f, 0.f, 1.f, 1.f);

	}
	
	void TrackControlPoint::destroy(Destroy &destroy)
	{
		
	}
	
	void TrackControlPoint::setShader(Render::IShaderDirectionalLightNoTex *shader)
	{
		mShader = shader;
		
		Renderable* renderables[] = { &mRenderablePoint, &mRenderableSide, &mRenderableFront, &mRenderableTop };

		for (int i = 0; i < 4; ++i)
		{
			renderables[i]->vertexArray.destroy();
			renderables[i]->vertexArray.create();
			{
				Engine::VertexArray::Binder     bind1(renderables[i]->vertexArray);
				Engine::VertexBuffer::Binder    bind2(renderables[i]->vertexBuffer);

				renderables[i]->vertexArray.attrib(shader->getVsPosition(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
				renderables[i]->vertexArray.attrib(shader->getVsNormal(), 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), 3 * sizeof(float));
			}

		}
	}

	
	void TrackControlPoint::render(RenderArg &render)
	{
		if (render.level == NULL)
			return;
		Level* level = (Level*)render.level;
		if (render.passElement == Engine::Solid)
		{
			Renderable* renderables[] = { &mRenderablePoint, &mRenderableSide, &mRenderableFront, &mRenderableTop };

			mShader->bind();
			mShader->setLightDirection(render.sunDirection);


			Render::Material material;
			material.MaterialAmbient = glm::vec4(0.2f);
				
			material.MaterialSpecular = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			material.MaterialShininess = 64.f;



			for (auto& p : level->mTrack.points)
			{
				glm::mat4 matrix = glm::translate(p);
				mShader->setMatrices(render.projection, render.view, matrix);

				for (int i = 0; i < 4; ++i)
				{					
					material.MaterialDiffuse = renderables[i]->color;
					mShader->setMaterial(material);

					Engine::VertexArray::Binder  bind1(renderables[i]->vertexArray);
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
					glDrawArrays(GL_TRIANGLES, 0, renderables[i]->count);
				}
			}
		}
	}
	
	void TrackControlPoint::update(Update& update)
	{
		if (update.level == NULL)
			return;
		
	}
};