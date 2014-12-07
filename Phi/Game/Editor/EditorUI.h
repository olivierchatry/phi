#pragma once


#include <Game/Base/GameEntity.h>
#include <Utils/HttpQuery.h>

namespace Editor
{
    class EditorUI : public Game::GameEntity
    {
    public:
        enum
        {
            Modify = 0,
            Add = 1
        };
    public:
        void initialize(Game::Initialize &initialize) override;
        void update(Game::Update& update) override;
        void destroy(Game::Destroy& destroy) override;
        void render(Game::RenderArg& render) override;
    
    public:
        int     getSelectedTool() { return mSelectedTool; }

    private:
        int     mSelectedTool;
		bool	mSave;
		char	mFileName[255];
        
        Utils::HttpQuery mQuery;
    };
};