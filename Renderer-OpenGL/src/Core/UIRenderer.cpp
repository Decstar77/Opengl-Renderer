#include "UIRenderer.h"

namespace cm
{

	UIRenderer::UIRenderer()
	{

	}

	UIRenderer::UIRenderer(GLFWwindow *window)
	{
		Init(window);
	}

	UIRenderer::~UIRenderer()
	{
		Free();
	}

	void UIRenderer::Init(GLFWwindow *window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		ImGui::StyleColorsClassic();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}


		bool window_res = ImGui_ImplGlfw_InitForOpenGL(window, true);
		bool opengl_res = ImGui_ImplOpenGL3_Init("#version 410");

		Assert(window_res); // @REASON: Failed init for glfw openGl
		Assert(opengl_res); // @REASON: Failed init for openGl3 
		initialized = true;
	}

	void UIRenderer::Free()
	{
		if (initialized)
		{
			ImGui::DestroyContext();
		}
		else
		{
			LOGC("UIRENNDERER::FREE -> Tried to free something that was not created");
		}
	}

	void UIRenderer::EndFrame()
	{
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		io.DisplaySize = ImVec2(1280, 720);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

	}

	void UIRenderer::BeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void UIRenderer::CreateUIWindow(const std::string &title)
	{
		ImGui::Begin(title.c_str());
	}

	void UIRenderer::EndUIWindow()
	{
		ImGui::End();
	}

	void UIRenderer::SetWindowSize(Vec2 size)
	{
		ImGui::SetWindowSize(ImVec2(size.x, size.y));
	}

	void UIRenderer::CreateText(const std::string &text)
	{
		ImGui::Text(text.c_str());

	}

	void UIRenderer::CreateText(const int32 &text)
	{
		std::stringstream ss;
		ss << text;
		ImGui::Text(ss.str().c_str());
	}

	void UIRenderer::CreateCheckBox(const std::string &label, bool* var)
	{
		ImGui::Checkbox(label.c_str(), var);
	}

	bool UIRenderer::CreateButton(const std::string &label)
	{
		return ImGui::Button(label.c_str());
	}

	bool UIRenderer::CreateButtonImage(uint32 id, int32 width, int32 height, const Vec4 &bg_col, const Vec4 &tn_col)
	{

		return ImGui::ImageButton(reinterpret_cast<void*>(id), ImVec2(static_cast<float>(width), static_cast<float>(height)), ImVec2(0, 0), ImVec2(1, 1), 1,
			ImVec4(bg_col.x, bg_col.y, bg_col.z, bg_col.w), ImVec4(tn_col.x, tn_col.y, tn_col.z, tn_col.w));
	}

	void UIRenderer::CreateFloatSlider(const std::string &label, float* var, float min, float max)
	{
		ImGui::SliderFloat(label.c_str(), var, min, max);
	}

	void UIRenderer::CreateColourEdit(const std::string &lable, float* var)
	{
		ImGui::ColorEdit3(lable.c_str(), var);
	}

	void UIRenderer::CreateColumns(const std::string &name, uint32 number)
	{
		ImGui::Columns(number, name.c_str());
	}

	void UIRenderer::NextColumn()
	{
		ImGui::NextColumn();
	}

	void UIRenderer::SetNextWindowPosition(float x, float y, bool repositionable)
	{
		ImVec2 pos(x, y);
		ImGui::SetNextWindowPos(pos, repositionable ? ImGuiCond_FirstUseEver : 0);
	}

	void UIRenderer::FocusWindow()
	{
		ImGui::SetWindowFocus();
	}

	void UIRenderer::IsMouseOnAnyWindow(bool &state)
	{
		state = ImGui::GetIO().WantCaptureMouse;			
	}

	void UIRenderer::AnyWindowFocus(bool &state)
	{
		state = ImGui::IsAnyWindowFocused();
	}

	void UIRenderer::CreateImage(uint32 id, int32 width, int32 height)
	{
		ImGui::Image(reinterpret_cast<void*>(id), ImVec2(static_cast<float>(width), static_cast<float>(height)));
	}

	void UIRenderer::CreateVec3Slider(const std::string &label, Vec3* var)
	{
		ImGui::DragFloat3(label.c_str(), var->arr, 0);
	}

	void UIRenderer::SameLineNext()
	{
		ImGui::SameLine();
	}

	void UIRenderer::Divide()
	{
		ImGui::Separator();
	}

	void UIRenderer::GetWindowSize(Vec2 *size)
	{
		ImVec2 temp = ImGui::GetWindowSize();
		size->x = temp.x;
		size->y = temp.y;
	}

	void EditorConsole::Log(const std::string &msg)
	{
		current << msg << '\n';
	}
	static void  Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }
	void EditorConsole::UpdateAndDraw()
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		ImGui::Begin(title.c_str());

		if (ImGui::SmallButton("Clear")) { Clear(); } ImGui::SameLine();
		bool copy_to_clipboard = ImGui::SmallButton("Copy");

		ImGui::Separator();
		const real32 footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
		
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

		if (copy_to_clipboard)
		{
			ImGui::LogToClipboard();
		}

		std::string info = current.str();
		
		if (Filter.PassFilter(info.c_str()))
		{
			ImGui::TextUnformatted(info.c_str());
		}
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
			   
		if (copy_to_clipboard) 
		{
			ImGui::LogFinish();
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();

		ImGui::Separator();

		//// @NOTE: Commands
		if (ImGui::InputText("Input", input_buffer, IM_ARRAYSIZE(input_buffer), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			std::string cmd = input_buffer;			
			ExeCommand(cmd);
			strcpy_s(input_buffer, "");
		}
		
		ImGui::End();

	}

	void EditorConsole::Clear()
	{
		// @NOTE: C++ man.... you'd think the .clear() would do...
		//		: Yup this is how you clear a stringstream
		history.str(std::string());
		current.str(std::string());
		commands.str(std::string());
	}


	void EditorConsole::ExeCommand(const std::string &cmd)
	{
		if (cmd == "help")
		{
			Log("Middle mouse to free look");			
		}		
	}

	void EditorRender::UpdateAndDraw()
	{
		Assert(render_settings); 
		ImGui::Begin("Render Settings");

		if (ImGui::CollapsingHeader("Render Passes"))
		{
			ImGui::Checkbox("Shadow Pass", &render_settings->shadow_pass);
			ImGui::Checkbox("Defferd Pass", &render_settings->defferd_pass);
			ImGui::Checkbox("SSAO Pass", &render_settings->ssao);
			ImGui::Checkbox("SSR Pass", &render_settings->ssr);
		}
		
		if (ImGui::CollapsingHeader("Post Processing"))
		{
			
			if (ImGui::TreeNode("Tone Mapping"))
			{
				ImGui::DragFloat("Exposure", &render_settings->post_processing_exposure, 0.1f);	
				
				render_settings->tonemapping = ImGui::RadioButton("RienHard", render_settings->tonemapping == RenderSettings::Reinhard) ? RenderSettings::Reinhard : render_settings->tonemapping;
				render_settings->tonemapping = ImGui::RadioButton("Uncharted", render_settings->tonemapping == RenderSettings::Uncharted) ? RenderSettings::Uncharted : render_settings->tonemapping;
				render_settings->tonemapping = ImGui::RadioButton("Filmic", render_settings->tonemapping == RenderSettings::Filmic) ? RenderSettings::Filmic : render_settings->tonemapping;				

				ImGui::TreePop();
			}


			if (ImGui::TreeNode("Bloom"))
			{
				ImGui::Checkbox("Bloom ", &render_settings->bloom);

				ImGui::Text(" Filter and Blur dependance");
				int32 current = render_settings->bloom_dependance;
				if (ImGui::Combo(" ", &current, "Dependent\0Independent\0\0"))
				{
					render_settings->bloom_dependance = (RenderSettings::BloomBlurDependance)current;
				}
				ImGui::SliderFloat("Threshold", &render_settings->bloom_threshold, 0.0, 10.0);
				ImGui::SliderInt("Kernel Size", &render_settings->bloom_kernel_size, 1, 20);
				ImGui::SliderFloat("DS Multiplier", &render_settings->bloom_downsample_mul, 0.25, 1.0);
				ImGui::SliderInt("Iterations", &render_settings->bloom_blur_iterations, 1, 4);
				ImGui::TreePop();
			}
						
			if (ImGui::TreeNode("FXAA"))
			{
				ImGui::Checkbox("FXAA ", &render_settings->fxaa);				
				ImGui::SliderFloat("Span max", &render_settings->fxaa_span_max, 0.0f, 40.0f);
				ImGui::SliderFloat("Dir min", &render_settings->fxaa_dir_min, 0.0f, 0.5f);
				ImGui::SliderFloat("Dir reduction", &render_settings->fxaa_dir_reduc, 0.0f, 1.f);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Vignette"))
			{
				ImGui::Checkbox("Vignette ", &render_settings->vigentte);
				ImGui::SliderFloat("Outer raduis", &render_settings->vigentte_outer_raduis, 0.0f, 1.0f);
				ImGui::SliderFloat("Inner raduis", &render_settings->vigentte_inner_raduis, 0.0f, 1.0f);
				ImGui::SliderFloat("Intensity", &render_settings->vigentte_intensity, 0.0f, 1.0f);
				ImGui::TreePop();
			}			
		}

		if (ImGui::CollapsingHeader("Performance"))
		{
			std::string ss = "Time " + std::to_string(delta_time);
			ImGui::PlotLines(ss.c_str(), fps_graph, 100, 0, "", 0, 120, ImVec2(0, 50));
		}
		
		ImGui::End();
		for (uint32 i = 0; i < 99; i++)
		{
			fps_graph[i] = fps_graph[i + 1];
		}
		fps_graph[99] = delta_time * 1000.f;
	}

}