#pragma once

#include <Game/Base/GameEntity.h>
#include <Render/Engine.h>
#include <Render/IShaderDirectionalLight.h>
#include <Math/AABB.h>
#include <Math/Plane.h>

namespace Editor
{
	class TrackControlPoint : public Game::GameEntity
	{
	public:
		void initialize(Game::Initialize &initialize) override;
		void update(Game::Update& update) override;
		void destroy(Game::Destroy& destroy) override;
		void render(Game::RenderArg& render) override;
		
		void setShader(Render::IShaderDirectionalLightNoTex* shader);
	
	private:
		enum Type  {
			POINT = 0,
			RIGHT,
			UP,
			FRONT
		};

		struct Renderable
		{
			Engine::VertexBuffer				vertexBuffer;
			int									count;
			Engine::VertexArray					vertexArray;
			glm::vec4							color;
            Math::AABB							aabb;
            glm::vec3                           direction;
			glm::vec3							normal;			
		};
		
	private:
		glm::vec3 getPlaneNormal(Game::Update& update, Type type);

	private:
		Renderable                              mRenderableHelper[4];
        Render::IShaderDirectionalLightNoTex*   mShader;
        Renderable*                             mSelected;
		Math::Plane								mMovePlane;

        bool                                    mButtonWasPressed;
        int                                     mSelectedTrackPoint;
		glm::vec3								mDistanceWithInitialClick;
		glm::vec3								mInitialClick;

	};
};


