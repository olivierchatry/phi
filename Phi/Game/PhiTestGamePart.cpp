#include <PreCompile.h>
#include <Game/PhiTestGamePart.h>
#include <Game/Level.h>
#include <Game/Player.h>
#include <Game/Terrain.h>
#include <Game/Camera/CameraFollowPlayer.h>
#include <Game/Camera/CameraFPS.h>
#include <Game/Editor/TrackControlPoint.h>
#include <Game/Editor/MousePointOnTrack.h>
#include <Game/Editor/ImGuiEntity.h>

namespace Game
{
	void    PhiTestGamePart::initialize(Initialize& initialize)
	{
		mShaderDirectional.create();
		mShaderDirectionalNoTex.create();
		
		Level* level = new Level();
		Terrain* terrain = new Terrain();
        {
            Editor::ImGuiEntity* imgui = new Editor::ImGuiEntity();
            imgui->initialize(initialize);
            mEntities.push_back(imgui);
        }
        {
			initialize.level = level;
			Level::GenerateArgument  arguments;
			arguments.circleSubDivice = 40.f;
			level->initialize(initialize);
			level->generate(arguments);
			level->setShader(&mShaderDirectional);
			mEntities.push_back(level);
		}
		{
			Terrain::GenerateArgument arguments;
			arguments.subDivision = 40.f;
			Math::AABB aabb = level->mLevelAABB;
			aabb.expand(glm::vec3(2.f, 1.f, 0));
			terrain->initialize(initialize);
			terrain->generate(aabb, arguments);
			terrain->setShader(&mShaderDirectionalNoTex);
			mEntities.push_back(terrain);

		}
        
		{
			Player* player = new Player();
			player->initialize(initialize);
			player->setShader(&mShaderDirectionalNoTex);
			mEntities.push_back(player);
		}
		{
			CameraFollowPlayer* camera = new CameraFollowPlayer();
			camera->initialize(initialize);
			mEntities.push_back(camera);
            mGame.push_back(camera);
        }
        {
            CameraFPS* cameraFps = new CameraFPS();
            cameraFps->initialize(initialize);
            cameraFps->setShader(&mShaderDirectionalNoTex);
            
            mEntities.push_back(cameraFps);
            mEditor.push_back(cameraFps);
            cameraFps->setActive(false);
        }
        {
			Editor::MousePointOnTrack* mousePointOnTrack = new Editor::MousePointOnTrack();
			mousePointOnTrack->initialize(initialize);
			mousePointOnTrack->setShader(&mShaderDirectionalNoTex);
			mEntities.push_back(mousePointOnTrack);
			mEditor.push_back(mousePointOnTrack);
			mousePointOnTrack->setActive(false);
        }

		{
			Editor::TrackControlPoint* trackElement = new Editor::TrackControlPoint();
			trackElement->initialize(initialize);
			trackElement->setShader(&mShaderDirectionalNoTex);
			mEntities.push_back(trackElement);
			mEditor.push_back(trackElement);
			trackElement->setActive(false);
		}


	}
	
	int     PhiTestGamePart::update(Update& update)
	{
		for (auto entity : mEntities)
		{
            if (entity->active())
                entity->update(update);
		}
		
        
        if (glfwGetKey(update.window, GLFW_KEY_0))
        {
            for (auto entity : mGame) entity->setActive(true);
            for (auto entity : mEditor) entity->setActive(false);
        }

        if (glfwGetKey(update.window, GLFW_KEY_1))
        {
            for (auto entity : mGame) entity->setActive(false);
            for (auto entity : mEditor) entity->setActive(true);
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
            if (entity->active())
                entity->render(render);
		}
	}
	
}