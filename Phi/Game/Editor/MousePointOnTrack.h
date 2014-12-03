#pragma once

#include <Game/Base/GameEntity.h>
#include <Render/Engine.h>
#include <Render/IShaderDirectionalLight.h>

namespace Editor
{
	class MousePointOnTrack : public Game::GameEntity
	{
	public:
		void initialize(Game::Initialize &initialize) override;
		void update(Game::Update& update) override;
		void destroy(Game::Destroy& destroy) override;
		void render(Game::RenderArg& render) override;
		
		void setShader(Render::IShaderDirectionalLightNoTex* shader);
	public:
		
	public:
	
	private:
		struct Renderable
		{
			Engine::VertexBuffer				vertexBuffer;
			int									count;
			Engine::VertexArray					vertexArray;
		};
		

	private:
		Renderable								mRenderable;
		Render::IShaderDirectionalLightNoTex*	mShader;
		glm::vec3								mPositionOnSpline;
		glm::vec3								mPositionOnBox;
		bool									mHavePosition;
		glm::vec3								mDeltaStart;
		glm::vec3								mDeltaEnd;
		bool									mWasPressed;
	};
};


