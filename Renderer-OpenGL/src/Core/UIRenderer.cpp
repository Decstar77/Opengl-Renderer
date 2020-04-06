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
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsClassic();

		bool window_res = ImGui_ImplGlfw_InitForOpenGL(window, true);
		bool opengl_res = ImGui_ImplOpenGL3_Init("#version 130");

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
			LOG("UIRENNDERER::FREE -> Tried to free something that was not created");
		}
	}

	void UIRenderer::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
}