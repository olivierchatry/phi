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
        update.editor = this;
        bool showWindow = true;
        update.mouseTaken = ImGui::Begin("Phi Track Editor", &showWindow, ImVec2(200,100), -1.0f, ImGuiWindowFlags_ShowBorders);
        ImGui::SetWindowFontScale(2.f);
        update.mouseTaken = ImGui::RadioButton("modify control point", &mSelectedTool, Modify);
        update.mouseTaken = ImGui::RadioButton("add control point", &mSelectedTool, Add);
        ImGui::End();
    }

    void EditorUI::destroy(Game::Destroy& destroy)
    {
        
    }

    void EditorUI::render(Game::RenderArg& render)
    {
        if (render.passFrame == Engine::Normal
            && render.passElement == Engine::Solid)
        {
        }
    }

}