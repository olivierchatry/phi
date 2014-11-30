#include <PreCompile.h>
#include <Game/PhiTestGamePart.h>
#include <Game/Level.h>
#include <Game/Player.h>
#include <Game/Terrain.h>
#include <Game/CameraFollowPlayer.h>

namespace Game
{
    void    PhiTestGamePart::initialize(Initialize& initialize)
    {
        mShaderDirectional.create();
        mShaderDirectionalNoTex.create();
        
        Level* level = new Level();
        Terrain* terrain = new Terrain();
        {
            initialize.level = level;
            Level::GenerateArgument  arguments;
            level->initialize(initialize);
            level->generate(arguments);
            level->setShader(&mShaderDirectional);
            mEntities.push_back(level);
        }
        {
            Terrain::GenerateArgument arguments;
            arguments.subDivision = 20.f;
            Render::AABB aabb = level->mLevelAABB;
            aabb.expand(glm::vec3(1, 1, 0));
            terrain->initialize(initialize);
            terrain->generate(aabb, arguments);
            terrain->setShader(&mShaderDirectionalNoTex);
        }
        {
            Player* player = new Player();
            player->initialize(initialize);
            player->setShader(&mShaderDirectionalNoTex);
            mEntities.push_back(player);
            CameraFollowPlayer* camera = new CameraFollowPlayer();
            mEntities.push_back(camera);
        }
    }
    
    int     PhiTestGamePart::update(Update& update)
    {
        for (auto entity : mEntities)
        {
            entity->update(update);
        }
        
        return 0;
    }
    
    void    PhiTestGamePart::destroy(Destroy& destroy)
    {
        for (auto entity : mEntities)
        {
            entity->destroy(destroy);
            delete entity;
        }
        mEntities.clear();
    }
    
    void    PhiTestGamePart::render(RenderArg &render)
    {
        render.sunDirection = glm::vec3(0, 0, 1);
        
        for (auto entity : mEntities)
        {
            entity->render(render);
        }
    }
    
}