#pragma once


#include <Game/Base/GameEntity.h>

namespace Editor
{
	class ImGuiEntity : public Game::GameEntity
	{
	public:
		void initialize(Game::Initialize &initialize) override;
		void update(Game::Update& update) override;
		void destroy(Game::Destroy& destroy) override;
		void render(Game::RenderArg& render) override;
	public:
		static void RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count);

	private:
		Engine::Shader			mShader;
		Engine::Texture			mTextureFont;
		Engine::VertexArray		mVertexArray;
		Engine::VertexBuffer	mVertexBuffer;
		int             mMatrixProjection;
		
		int				mTexture;
		
		int             mVsPosition;
		int             mVsUV;
		int             mVsColor;
	};
};