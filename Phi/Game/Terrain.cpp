#include <PreCompile.h>
#include <Game/Terrain.h>
#include <Render/Engine.h>
#include <Utils/Utils.h>

namespace Game
{
	void Terrain::addChunk(Math::AABB& aabb, std::vector<float>& vs)
	{
		TerrainRenderable* chunk = new TerrainRenderable();
		Utils::GenerateNormals(&vs[0], 6, vs.size() / 6, 0, 3);
		
		chunk->aabb = aabb;
		chunk->count = (int)vs.size();
		
		chunk->vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
		chunk->vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
		chunk->vertexArray.create();
		mTerrainRenderables.push_back(chunk);
	}
	
	void Terrain::update(Update& update)
	{
		
	}
	
	void Terrain::initialize(Initialize& initialize)
	{
		
	}

	void Terrain::destroy(Destroy& destroy)
	{
		for (auto chunk : mTerrainRenderables)
		{
			delete chunk;
		}
		mTerrainRenderables.clear();
		mShader = NULL;
	}

	void Terrain::setShader(Render::IShaderDirectionalLightNoTex *shader)
	{
		mShader = shader;
		for (auto chunk : mTerrainRenderables)
		{
			chunk->vertexArray.destroy();
			chunk->vertexArray.create();
			{
				Engine::VertexArray::Binder     bind1(chunk->vertexArray);
				Engine::VertexBuffer::Binder    bind2(chunk->vertexBuffer);
				
				chunk->vertexArray.attrib(mShader->getVsPosition(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
				chunk->vertexArray.attrib(mShader->getVsNormal(), 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), 3 * sizeof(float));
			}
		}
	}
	
	void Terrain::render(RenderArg& render)
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
			glm::mat4 model(1.f);
			mShader->setMatrices(render.projection, render.view, model);
			for (auto chunk : mTerrainRenderables)
			{
				Engine::VertexArray::Binder  bind1(chunk->vertexArray);
			
				glCullFace(GL_BACK);
				glDrawArrays(GL_TRIANGLES, 0, chunk->count);
			}
		}
		
	}

	
	void Terrain::generate(Math::AABB& aabb, GenerateArgument& argument)
	{
		glm::vec3 delta = aabb.size() / argument.subDivision;
		
		std::vector<float>      vs;
		
		Math::AABB chunkAABB;
		chunkAABB.reset();
		
		for (float y = aabb.min.y; y < aabb.max.y; y += delta.y)
		{
			for (float x = aabb.min.x; x < aabb.max.x; x += delta.x)
			{
				float z = glm::linearRand(0.f, 1000.f);
				Math::AABB current;
				current.min = glm::vec3(x, y, 0);
				current.max = glm::vec3(x + delta.x, y + delta.y, z);
				Utils::GenerateCube(current, vs);
				chunkAABB.add(aabb);
				
				if (vs.size() > 32000)
				{
					addChunk(chunkAABB, vs);
					chunkAABB.reset();
					vs.clear();
				}
			}
		}
		if (!vs.empty())
			addChunk(chunkAABB, vs);
	}
}
