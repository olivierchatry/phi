#pragma once

#include <Game/GameEntity.h>
#include <Render/Engine.h>
#include <Render/IShaderDirectionalLight.h>

namespace Game
{
	class MousePointOnTrack : public GameEntity
	{
	public:
		void initialize(Initialize &initialize) override;
		void update(Update& update) override;
		void destroy(Destroy& destroy) override;
		void render(RenderArg& render) override;
		
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


