#pragma once

#include <Render/AABB.h>
#include <Render/Engine.h>
#include <Render/IShaderDirectionalLight.h>

namespace Game
{
    struct Terrain
    {
    public:
        struct GenerateArgument
        {
            GenerateArgument()
            {
                vertexPerMeter = 0.1;
            }
            
            float vertexPerMeter;
        };
    public:
        void generate(Render::AABB& aabb, GenerateArgument& argument);
        void setShader(Render::IShaderDirectionalLightNoTex* shader);
        void destroy();
        
    protected:
        void addChunk(Render::AABB& aabb, std::vector<float>& vs, std::vector<GLushort>& indices);
        
    public:
        struct TerrainRenderable
        {
            Engine::VertexBuffer				vertexBuffer;
            Engine::IndexBuffer					indexBuffer;
            int									count;
            Engine::VertexArray					vertexArray;
            Render::AABB						aabb;
        };
        
        
        std::vector<TerrainRenderable*> mTerrainRenderables;
        Render::IShaderDirectionalLightNoTex* mShader;
    };
};


