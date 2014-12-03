#include <PreCompile.h>
#include <Game/Editor/EditorUI.h>

namespace Editor
{
    
    void EditorUI::initialize(Game::Initialize &initialize)
    {
        mSelectedTool = 0;
    }

    void EditorUI::update(Game::Update& update)
    {
        
    }

    void EditorUI::destroy(Game::Destroy& destroy)
    {
        
    }

    void EditorUI::render(Game::RenderArg& render)
    {
        bool showWindow = true;
        if (render.passFrame == Engine::Normal
            && render.passElement == Engine::Solid)
        {
            ImGui::Begin("Phi Track Editor", &showWindow, ImVec2(0,0), -1.0f, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::SetWindowFontScale(2.f);
            ImGui::RadioButton("modify control point", &mSelectedTool, 0);
            ImGui::RadioButton("add control point", &mSelectedTool, 1);
            
            ImGui::End();
        }
    }

}