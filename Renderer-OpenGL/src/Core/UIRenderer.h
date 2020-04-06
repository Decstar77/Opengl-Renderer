#pragma once
#include "Core.h"
#include "../vendor/IMGUI/imgui.h"
#include "../vendor/IMGUI/imgui_impl_glfw.h"
#include "../vendor/IMGUI/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "Math/CosmicMath.h"

namespace cm
{
	class UIRenderer
	{
	public:
		UIRenderer();
		UIRenderer(GLFWwindow *window);
		~UIRenderer();

		void Init(GLFWwindow *window);
		void Free();

		void BeginFrame();
		void EndFrame();
		
		void CreateUIWindow(const std::string &title);
		void EndUIWindow();
		
		void GetWindowSize(Vec2 *size);
		void SetWindowSize(Vec2 size);
		void SetNextWindowPosition(float x, float y, bool repositionable = false);


		void FocusWindow();
		void IsMouseOnAnyWindow(bool &state);
		void AnyWindowFocus(bool &state);
		
		
		
		void CreateText(const std::string &text);
		void CreateText(const int32 &text);
		
		void CreateCheckBox(const std::string &label, bool* var);		

		bool CreateButton(const std::string &label);
		bool CreateButtonImage(uint32 id, int32 width, int32 height, const Vec4 &bg_col = Vec4(0),const Vec4 &tn_col = Vec4(1));

		void CreateFloatSlider(const std::string &label, float* var, float min, float max);
		void CreateVec3Slider(const std::string &label, Vec3* var);
		
		void CreateColourEdit(const std::string &lable, float* var);

		void CreateColumns(const std::string &name, uint32 number);

		void CreateImage(uint32 id, int32 width, int32 height);

		void NextColumn();

		void SameLineNext();
		void Divide();

	public:
		bool initialized = false;

	};



}