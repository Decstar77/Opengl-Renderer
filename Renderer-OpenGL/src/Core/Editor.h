#pragma once
#include "Core.h"
#include "Math/CosmicMath.h"
#include "Math/CosmicGeometry.h"

#include "Core/Debug.h"
#include "Core/World.h"
#include "Core/Sandbox.h"

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
		
	enum class TransformationMode
	{
		none = 0,
		x_axis = 1,
		y_axis = 2,
		z_axis = 3
	};

	enum class ControlWidget
	{
		none = 0,
		translation = 1,
		rotation = 2,
		scaling
	};

	class Widget3D
	{
	public:
		virtual void SetTransform(const Transform &transform) = 0;
		virtual void Create(const GLMesh &mesh) = 0;
		virtual bool Select(const Ray &camera_ray, const Transform &transform) = 0;
		virtual bool Update(const Ray &camera_ray, Transform *transform) = 0;
		virtual const GLMesh GetMeshForRender() const = 0;
	};

	class RotationWidget
	{
	private:
		void CalculateBoundingBoxes();

	private:
		GLMesh mesh;		
		TransformationMode transformation_mode;

	public:
		Transform transform;
		Plane transform_plane;
		OBB x_bounding_volumes[4];
		OBB y_bounding_volumes[4];
		OBB z_bounding_volumes[4];
		bool is_selected = false;
	
	public:		
		void SetTransform(const Transform &transform);
		void Create(const GLMesh &mesh);
		bool Select(const Ray &camera_ray, const Transform &transform);
		bool Update(const Ray &camera_ray, Transform *transform);
		const GLMesh GetMeshForRender() const;

	public:
		RotationWidget();
		~RotationWidget();

	};

	class TranslationWidget 
	{
	private:
		void CalculateBoundingBoxes();

	private:
		GLMesh mesh;
		TransformationMode translation_mode = TransformationMode::none;

	public:
		Transform transform;
		Plane translation_plane;
		OBB x_bounding_volume;
		OBB y_bounding_volume;
		OBB z_bounding_volume;
		bool is_selected = false;
		
	public:		
		void SetTransform(const Transform &transform);
		void Create(const GLMesh &mesh);
		bool Select(const Ray &camera_ray, const Transform &transform);
		bool Update(const Ray &camera_ray, Transform *transform);
		const GLMesh GetMeshForRender();

	public:
		TranslationWidget();
		~TranslationWidget();
		
	};

	class ScalingWidget
	{

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

	class EditorWorldObjectSpawner
	{
	private:
		void SpawnCube();
		void SpawnPlane();

	public:
		void UpdateAndDraw();
		
		World *world = nullptr;

	public:
		EditorWorldObjectSpawner();
		EditorWorldObjectSpawner(World *world);
		~EditorWorldObjectSpawner();
	};

	class EditorWorldObjectInspector
	{
	public: 
		void UpdateAndDraw();
		
		WorldObject *world_object = nullptr;

	public:
		EditorWorldObjectInspector();
		EditorWorldObjectInspector(WorldObject *actor);
		~EditorWorldObjectInspector();

	};
}