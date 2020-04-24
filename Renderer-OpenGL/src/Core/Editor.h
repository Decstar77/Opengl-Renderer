#pragma once
#include "Core.h"
#include "Core/World.h"
#include "Math/CosmicMath.h"
#include "Math/CosmicGeometry.h"
#include "Core/Debug.h"
// @NOTE: Apparently this doesn't work, therefore I've defined it in the visual studio pre processor
//#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "../vendor/IMGUI/imgui.h"
#include "../vendor/IMGUI/imgui_impl_glfw.h"
#include "../vendor/IMGUI/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>



namespace cm
{
	void InitializeEditor(GLFWwindow *window);
	void EditorBeginFrame();
	void EditorEndFrame();
	void FreeEditor();
		

	class TranslationWidget 
	{
	public:
		Transform transform;
		WorldObject *object;
		GLMesh mesh;
		Aabb x_bounding_volume;
		Aabb y_bounding_volume;
		Aabb z_bounding_volume;
		Plane translation_plane;
			
		bool is_held = false;

		void CheckCollision();

		void Draw();
	};



	class EditorConsole
	{		
	public:
		void Log(const std::string &msg);
		void Log(const bool msg);
		void UpdateAndDraw();
		void ClearLogger();
		void ExeCommand(const std::string &cmd);

	public:
		std::string title = "Console";

	private:
		char input_buffer[256];
		std::stringstream commands;
		std::stringstream current;
		std::stringstream history;


		ImGuiTextFilter	Filter;
		bool auto_scroll = true;
		bool scroll_to_bottom = false;
	};

	class EditorRenderSettingsWindow
	{
	public: 
		RenderSettings *render_settings = nullptr;
		real32 delta_time = 0;
		real32 fps_graph[100] = {};

	public:
		void UpdateAndDraw();

	};

}