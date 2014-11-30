#pragma once

#include <Render/AABB.h>
#include <Render/Engine.h>
#include <Render/IShaderDirectionalLight.h>
#include <Game/GameEntity.h>

namespace Game
{
    struct Terrain : public GameEntity
    {
    public:
        struct GenerateArgument
        {
            GenerateArgument()
            {
				subDivision = 100.f;
            }
            
            float subDivision;
        };
    public:
        void update(Update& update) override;
        void initialize(Initialize& initialize) override;
        void destroy(Destroy& destroy) override;
        void    render(RenderArg& render) override;

        void generate(Render::AABB& aabb, GenerateArgument& argument);
        void setShader(Render::IShaderDirectionalLightNoTex* shader);
        
    protected:
        void addChunk(Render::AABB& aabb, std::vector<float>& vs);
        
    public:
        struct TerrainRenderable
        {
            Engine::VertexBuffer				vertexBuffer;
            int									count;
            Engine::VertexArray					vertexArray;
            Render::AABB						aabb;
        };
        
        
        std::vector<TerrainRenderable*>     mTerrainRenderables;
        Render::IShaderDirectionalLightNoTex*    mShader;
    };
};


