#include <PreCompile.h>

#include <Game/Editor/ImGuiEntity.h>

namespace Editor
{
	// GLFW callbacks to get events
	static void glfw_error_callback(int error, const char* description)
	{
		fputs(description, stderr);
	}
	
	
	static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += (float)yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
	}
	
	static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (action == GLFW_PRESS)
			io.KeysDown[key] = true;
		if (action == GLFW_RELEASE)
			io.KeysDown[key] = false;
		io.KeyCtrl = (mods & GLFW_MOD_CONTROL) != 0;
		io.KeyShift = (mods & GLFW_MOD_SHIFT) != 0;
	}
	
	static void glfw_char_callback(GLFWwindow* window, unsigned int c)
	{
		if (c > 0 && c < 0x10000)
			ImGui::GetIO().AddInputCharacter((unsigned short)c);
	}

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
		
		size_t total_vtx_count = 0;
		for (int n = 0; n < cmd_lists_count; n++)
			total_vtx_count += cmd_lists[n]->vtx_buffer.size();

		Engine::VertexBuffer::Binder     bind1(entity->mVertexBuffer);
		int needed_buffer_size = (int) total_vtx_count * sizeof(ImDrawVert);
		if (needed_buffer_size > entity->mVertexBuffer.size())
			entity->mVertexBuffer.update(NULL, 0, needed_buffer_size);

		unsigned char* buffer_data = (unsigned char*)entity->mVertexBuffer.map(GL_WRITE_ONLY);
		if (!buffer_data)
			return;
		for (int n = 0; n < cmd_lists_count; n++)
		{
			const ImDrawList* cmd_list = cmd_lists[n];
			memcpy(buffer_data, &cmd_list->vtx_buffer[0], cmd_list->vtx_buffer.size() * sizeof(ImDrawVert));
			buffer_data += cmd_list->vtx_buffer.size() * sizeof(ImDrawVert);
		}
		entity->mVertexBuffer.unmap();

		Engine::VertexArray::Binder     bind2(entity->mVertexArray);
		// Render command lists
		int cmd_offset = 0;
		for (int n = 0; n < cmd_lists_count; n++)
		{
			const ImDrawList* cmd_list = cmd_lists[n];
			int vtx_offset = cmd_offset;
			const ImDrawCmd* pcmd_end = cmd_list->commands.end();
			for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
			{
				glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
				glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
				vtx_offset += pcmd->vtx_count;
			}
			cmd_offset = vtx_offset;
		}
		
		// Restore modified state
		glScissor(0, 0, (int) width, (int) height);
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_TEXTURE_2D);
	}

	void ImGuiEntity::initialize(Game::Initialize &initialize)
	{
		int width, height;
		glfwGetFramebufferSize(initialize.window, &width, &height);
		glfwSetCharCallback(initialize.window, glfw_char_callback);
		glfwSetKeyCallback(initialize.window, glfw_key_callback);
		glfwSetScrollCallback(initialize.window, glfw_scroll_callback);
		
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)width, (float)height);  // Display size, in pixels. For clamping windows positions.
		io.PixelCenterOffset = 0.0f;                                  // Align OpenGL texels
		io.RenderDrawListsFn = RenderDrawLists;
		io.UserData = (void*)this;
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

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
			mVertexArray.attrib(mVsColor,		4, GL_UNSIGNED_BYTE,	GL_TRUE, sizeof(ImDrawVert),	offsetof(ImDrawVert, col));
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
		io.DisplaySize = ImVec2((float)width, (float)height);  // Display size, in pixels. For clamping windows positions.
		io.MousePos = ImVec2((float)currentMousePosition.x, (float)currentMousePosition.y);
		io.MouseDown[0] = glfwGetMouseButton(update.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		io.MouseDown[1] = glfwGetMouseButton(update.window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
		io.DeltaTime = update.delta;

		ImGui::NewFrame();
		//ImGui::SetWindowFontScale(2.f);
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
				ImGui::Render();
			}
		}
	}
	
}