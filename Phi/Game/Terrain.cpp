#include <PreCompile.h>
#include <Game/Terrain.h>
#include <Render/Engine.h>
#include <Utils/Utils.h>

namespace Game
{
    static void generateCube(Render::AABB& aabb, std::vector<float>& v)
    {
        GLushort cube_elements[] = {
            // front
            0, 1, 2,
            2, 3, 0,
            // top
            3, 2, 6,
            6, 7, 3,
            // back
            7, 6, 5,
            5, 4, 7,
            // bottom
            4, 5, 1,
            1, 0, 4,
            // left
            4, 0, 3,
            3, 7, 4,
            // right
            1, 5, 6,
            6, 2, 1,
        };
        
		for (int i = 0; i < 36; ++i)
		{
			glm::vec3 corner = aabb.corner(cube_elements[i]);

			v.push_back(corner.x);
			v.push_back(corner.y);
			v.push_back(corner.z);

			v.push_back(0.f);
			v.push_back(0.f);
			v.push_back(0.f);
		}
    }
    
    void Terrain::addChunk(Render::AABB& aabb, std::vector<float>& vs)
    {
        TerrainRenderable* chunk = new TerrainRenderable();
        Utils::GenerateNormals(&vs[0], 6, vs.size() / 18, 0, 3);
        
        chunk->aabb = aabb;
        chunk->count = (int)vs.size();
        
        chunk->vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
        chunk->vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
        chunk->vertexArray.create();
        mTerrainRenderables.push_back(chunk);
    }
    
    void Terrain::destroy()
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
    
    void Terrain::generate(Render::AABB& aabb, GenerateArgument& argument)
    {
		glm::vec3 delta = aabb.size() / argument.subDivision;
        
        std::vector<float>      vs;
        
        Render::AABB chunkAABB;
        chunkAABB.reset();
        
        for (float y = aabb.min.y; y < aabb.max.y; y += delta.y)
        {
            for (float x = aabb.min.x; x < aabb.max.x; x += delta.x)
            {
                float z = glm::linearRand(0.f, 500.f);
                Render::AABB current;
                current.min = glm::vec3(x, y, 0);
                current.max = glm::vec3(x + delta.x, y + delta.y, z);
                generateCube(current, vs);
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
