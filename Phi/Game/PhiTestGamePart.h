#pragma once

#include <Game/Base/GamePart.h>
#include <Render/ShaderDirectionalPerPixel.h>
#include <Render/ShaderDirectionalPerPixelNoTex.h>


namespace Game
{
    class PhiTestGamePart : public GamePart
    {
    public:
        void    initialize(Initialize& intialize) override;
        int     update(Update& update) override;
        void    destroy(Destroy& destroy) override;
        void    render(RenderArg& render) override;

    private:
        typedef std::vector<GameEntity*> GameEntities;
    private:
        GameEntities mEntities;
        Render::ShaderDirectionalPerPixel       mShaderDirectional;
        Render::ShaderDirectionalPerPixelNoTex  mShaderDirectionalNoTex;
        GameEntities mEditor;
        GameEntities mGame;
    };
}