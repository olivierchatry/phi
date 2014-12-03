#include <PreCompile.h>

#include <Game/Editor/ImGuiEntity.h>

namespace Editor
{
	void ImGuiEntity::RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
	{
		if (cmd_lists_count == 0)
			return;
		ImGuiIO& io = ImGui::GetIO();

		ImGuiEntity* entity = (ImGuiEntity*) io.UserData;
		// We are using the OpenGL fixed pipeline to make the example code simpler to read!
		// A probable faster way to render would be to collate all vertices from all cmd_lists into a single vertex buffer.
		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_TEXTURE_2D);

		// Setup texture
		
		// Setup orthographic projection matrix
		float width = ImGui::GetIO().DisplaySize.x;
		float height = ImGui::GetIO().DisplaySize.y;
		glm::mat4 projection = glm::ortho(0.f, width, height, 0.f, -1.f, 1.f);
		entity->mShader.bind();
		entity->mShader.setUniform(entity->mMatrixProjection, projection);
		entity->mShader.setUniform(entity->mTexture, 0);
		entity->mTextureFont.bind(0);

		// Render command lists
		for (int n = 0; n < cmd_lists_count; n++)
		{
			const ImDrawList* cmd_list = cmd_lists[n];
			const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->vtx_buffer.front();
			int size_update = 0;
			
			for (size_t cmd_i = 0; cmd_i < cmd_list->commands.size(); cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->commands[cmd_i];
				size_update += pcmd->vtx_count;
			}

			{
				Engine::VertexBuffer::Binder     bind1(entity->mVertexBuffer);
				entity->mVertexBuffer.update(vtx_buffer, 0, size_update * sizeof(ImDrawVert));
			}

			{
				Engine::VertexArray::Binder     bind1(entity->mVertexArray);
				int vtx_offset = 0;
				for (size_t cmd_i = 0; cmd_i < cmd_list->commands.size(); cmd_i++)
				{
					const ImDrawCmd* pcmd = &cmd_list->commands[cmd_i];
					glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
					glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
					vtx_offset += pcmd->vtx_count;
				}
			}
		}
		
		// Restore modified state
		glPopAttrib();
		glScissor(0, 0, width, height);
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_TEXTURE_2D);
	}

	void ImGuiEntity::initialize(Game::Initialize &initialize)
	{
		int width, height;
		glfwGetFramebufferSize(initialize.window, &width, &height);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)width, (float)height);  // Display size, in pixels. For clamping windows positions.
		io.PixelCenterOffset = 0.0f;                                  // Align OpenGL texels
		io.RenderDrawListsFn = RenderDrawLists;
		io.UserData = (void*)this;
		const void* png_data;
		unsigned int png_size;
		ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);

		int tex_x, tex_y, tex_comp;
		GLubyte* tex_data = stbi_load_from_memory((const unsigned char*)png_data, (int)png_size, &tex_x, &tex_y, &tex_comp, 0);

		mTextureFont.createFromBuffer(tex_data, tex_x, tex_y, GL_TEXTURE_2D, tex_comp);
		mTextureFont.bind(0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		stbi_image_free(tex_data);
		
		mShader.create();
		mShader.addFromFile("Shaders/ImGUI.vert", GL_VERTEX_SHADER);
		mShader.addFromFile("Shaders/ImGUI.frag", GL_FRAGMENT_SHADER);
		mShader.link();
		
		mVsPosition = mShader.getAttribute("in_Position");
		mVsColor = mShader.getAttribute("in_Color");
		mVsUV = mShader.getAttribute("in_UV");
		
		mMatrixProjection = mShader.getUniform("uni_ProjectionMatrix");
		mTexture = mShader.getUniform("uni_Texture");
		mVertexBuffer.create(GL_DYNAMIC_DRAW, 1000000);
		mVertexArray.create();
		{
			Engine::VertexArray::Binder     bind1(mVertexArray);
			Engine::VertexBuffer::Binder    bind2(mVertexBuffer);

			mVertexArray.attrib(mVsPosition,	2, GL_FLOAT,			GL_FALSE, sizeof(ImDrawVert),	offsetof(ImDrawVert, pos));
			mVertexArray.attrib(mVsColor,		4, GL_UNSIGNED_BYTE,	GL_FALSE, sizeof(ImDrawVert),	offsetof(ImDrawVert, col));
			mVertexArray.attrib(mVsUV,			2, GL_FLOAT,			GL_FALSE, sizeof(ImDrawVert),	offsetof(ImDrawVert, uv));
		}
	}
	
	void ImGuiEntity::update(Game::Update& update)
	{
		int width, height;
		glfwGetFramebufferSize(update.window, &width, &height);
		int windowWidth, windowHeight;
		glfwGetWindowSize(update.window, &windowWidth, &windowHeight);
		glm::dvec2 scale((double)width / (double)windowWidth, (double)height / (double)windowHeight);
		

		ImGuiIO& io = ImGui::GetIO();
		glm::dvec2 currentMousePosition;
		glfwGetCursorPos(update.window, &currentMousePosition.x, &currentMousePosition.y);
		currentMousePosition *= scale;
		// currentMousePosition.y = height - currentMousePosition.y ;

		io.MousePos = ImVec2((float)currentMousePosition.x, (float)currentMousePosition.y);
		io.MouseDown[0] = glfwGetMouseButton(update.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		io.MouseDown[1] = glfwGetMouseButton(update.window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
		io.DeltaTime = update.delta;

		ImGui::NewFrame();
}
	
	void ImGuiEntity::destroy(Game::Destroy& destroy)
	{
		ImGui::Shutdown();
	}
	void ImGuiEntity::render(Game::RenderArg& render)
	{
		if (render.passElement == Engine::Solid)
		{
			if (render.passFrame == Engine::Post)
			{
				ImGuiIO& io = ImGui::GetIO();

				//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
				//glClearColor(0.8f, 0.6f, 0.6f, 1.0f);
				//glClear(GL_COLOR_BUFFER_BIT);

				ImGui::Render();
			}
		}
	}
	
}