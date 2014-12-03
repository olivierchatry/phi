#include <PreCompile.h>
#include <Game/PhiEditorGamePart.h>
#include <Game/Level.h>
#include <Game/Player.h>
#include <Game/Terrain.h>
#include <Game/Camera/CameraFPS.h>
#include <Game/Editor/TrackControlPoint.h>
#include <Game/Editor/MousePointOnTrack.h>

namespace Game
{
	void    PhiEditorGamePart::initialize(Initialize& initialize)
	{
		mShaderDirectional.create();
		mShaderDirectionalNoTex.create();

		Level* level = new Level();
		Terrain* terrain = new Terrain();
		{
			initialize.level = level;
			Level::GenerateArgument  arguments;
			arguments.circleSubDivice = 10.f;
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
            CameraFPS* camera = new CameraFPS();
            camera->initialize(initialize);
            camera->setShader(&mShaderDirectionalNoTex);

            mEntities.push_back(camera);
        }
		{
			Editor::TrackControlPoint* trackElement = new Editor::TrackControlPoint();
			trackElement->initialize(initialize);
			trackElement->setShader(&mShaderDirectionalNoTex);
			mEntities.push_back(trackElement);
		}

	}

	int     PhiEditorGamePart::update(Update& update)
	{
		for (auto entity : mEntities)
		{
			entity->update(update);
		}

		return 0;
	}

	void    PhiEditorGamePart::destroy(Destroy& destroy)
	{
		for (auto entity : mEntities)
		{
			entity->destroy(destroy);
			delete entity;
		}
		mEntities.clear();
	}

	void    PhiEditorGamePart::render(RenderArg &render)
	{
		render.sunDirection = glm::vec3(0, 0, 1);

		for (auto entity : mEntities)
		{
			entity->render(render);
		}
	}

}