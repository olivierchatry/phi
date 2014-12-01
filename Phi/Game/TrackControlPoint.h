#pragma once

#include <Game/GameEntity.h>
#include <Render/Engine.h>
#include <Render/IShaderDirectionalLight.h>

namespace Game
{
	class TrackControlPoint : public GameEntity
	{
	public:
		void initialize(Initialize &initialize) override;
		void update(Update& update) override;
		void destroy(Destroy& destroy) override;
		void render(RenderArg& render) override;
		
		void setShader(Render::IShaderDirectionalLightNoTex* shader);
	
	private:
		struct Renderable
		{
			Engine::VertexBuffer				vertexBuffer;
			int									count;
			Engine::VertexArray					vertexArray;
			glm::vec4							color;
		};
		

	private:
		Renderable      mRenderablePoint;
		Renderable      mRenderableFront;
		Renderable      mRenderableSide;
		Renderable      mRenderableTop;
		Render::IShaderDirectionalLightNoTex* mShader;
	};
};


