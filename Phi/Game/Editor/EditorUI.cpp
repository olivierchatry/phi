#include <PreCompile.h>
#include <Game/Editor/EditorUI.h>
#include <Game/Level.h>

namespace Editor
{
	
	void EditorUI::initialize(Game::Initialize &initialize)
	{
		mSelectedTool = 0;
		memset(mFileName, 0, 200);
	}

	void EditorUI::update(Game::Update& update)
	{
		update.editor = this;
		bool showWindow = true;
		ImGui::Begin("Phi Track Editor", &showWindow, ImVec2(200,100), -1.0f, ImGuiWindowFlags_ShowBorders);
		ImGui::SetWindowFontScale(2.f);
		if (ImGui::CollapsingHeader("tools"))
		{
			ImGui::RadioButton("modify control point", &mSelectedTool, Modify);
			ImGui::RadioButton("add control point", &mSelectedTool, Add);
		}
		if (ImGui::CollapsingHeader("file"))
		{
			ImGui::InputText("file name", mFileName, 200);
            if (ImGui::Button("save"))
            {
                Game::Level* level = (Game::Level*)update.level;
                if (level)
                {
                    std::string levelJSON;
                    level->save(levelJSON);
                }
            }
		}
		ImGui::End();
		update.mouseTaken = ImGui::GetIO().WantCaptureMouse;
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