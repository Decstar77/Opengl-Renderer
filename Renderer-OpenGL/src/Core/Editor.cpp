#include "Editor.h"

namespace cm
{
	void InitializeEditor(GLFWwindow *window)
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
	}

	void EditorBeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorEndFrame()
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

	void FreeEditor()
	{
		ImGui::DestroyContext();
	}


	void EditorConsole::Log(const std::string &msg)
	{
		current << msg << '\n';
	}

	void EditorConsole::Log(const bool msg)
	{
		current << msg << '\n';
	}

	static void  Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }
	void EditorConsole::UpdateAndDraw()
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		ImGui::Begin(title.c_str());

		if (ImGui::SmallButton("Clear")) { ClearLogger(); } ImGui::SameLine();
		bool copy_to_clipboard = ImGui::SmallButton("Copy"); ImGui::SameLine();
		scroll_to_bottom = ImGui::SmallButton("Scroll to bottom"); ImGui::SameLine();
		ImGui::Checkbox("Auto scroll", &auto_scroll); 
		

		ImGui::Separator();
		const real32 footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
		
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

		if (copy_to_clipboard)
		{
			ImGui::LogToClipboard();
		}

		current << Debug::logging_stream.str();
		Debug::ClearLogger();
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

		if (auto_scroll || scroll_to_bottom)
		{
			ImGui::SetScrollHereY(1.0f);
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

	void EditorConsole::ClearLogger()
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

	void EditorRenderSettingsWindow::UpdateAndDraw()
	{
		Assert(render_settings); 
		ImGui::Begin("Render Settings");

		if (ImGui::CollapsingHeader("Render Passes"))
		{
			if (ImGui::TreeNode("Shadow Pass"))
			{
				ImGui::Text("On the todo: When I'm smart enough");
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SSR Pass"))
			{
				ImGui::Text("On the todo: When I'm smart enough");
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SSAO Pass"))
			{
				render_settings->ssao_changed = render_settings->ssao_changed | ImGui::Checkbox("SSAO Pass", &render_settings->ssao);
				render_settings->ssao_changed = render_settings->ssao_changed | ImGui::SliderFloat("Radius", &render_settings->ssao_radius, 0.1, 5.0);
				render_settings->ssao_changed = render_settings->ssao_changed | ImGui::SliderFloat("Strength", &render_settings->ssao_strength, 0.1, 5.0);
				render_settings->ssao_changed = render_settings->ssao_changed | ImGui::SliderFloat("Bias", &render_settings->ssao_bias, 0.1, 0.001);
				render_settings->ssao_changed = render_settings->ssao_changed | ImGui::SliderInt("Kernel Size", &render_settings->ssao_kernel_size, 1, 64);
				ImGui::TreePop();
			}
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