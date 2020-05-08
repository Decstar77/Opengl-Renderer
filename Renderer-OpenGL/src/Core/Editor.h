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

	class Widget3D
	{
	public:
		virtual Transform *GetTransform();
		virtual void SetTransform(const Transform &transform);
		
		virtual real32 GetSnappingAmount();
		virtual void SetSnappingAmount(const real32 &deg);

		virtual bool IsObjectAligning();
		virtual void ObjectAlign();
		virtual void WorldAlign();

		virtual void Create(const GLMesh &mesh) = 0;
		virtual bool Select(const Ray &camera_ray, const Transform &transform) = 0;
		virtual bool Update(const Ray &camera_ray, Transform *transform) = 0;
		virtual const GLMesh GetMeshForRender() const;

	protected:
		Transform transform;
		GLMesh mesh;

		TransformationMode transformation_mode = TransformationMode::none;
		bool object_aligning = true;
		real32 snapping_amount = 0;
	
	protected:
		virtual void CalculateBoundingBoxes() = 0;
	};

	class TranslationWidget : public Widget3D
	{
	public:		
		Plane translation_plane;
		OBB x_bounding_volume;
		OBB y_bounding_volume;
		OBB z_bounding_volume;
		bool is_selected = false;

	public:	
		virtual void Create(const GLMesh &mesh) override;
		virtual bool Select(const Ray &camera_ray, const Transform &transform) override;
		virtual bool Update(const Ray &camera_ray, Transform *transform) override;

	public:
		TranslationWidget();
		~TranslationWidget();

	private:		
		virtual void CalculateBoundingBoxes() override;
	};

	class RotationWidget : public Widget3D
	{
	public:
		Plane transform_plane;
		OBB x_bounding_volumes[4];
		OBB y_bounding_volumes[4];
		OBB z_bounding_volumes[4];
		bool is_selected = false;

	public:
		virtual void Create(const GLMesh &mesh) override;
		virtual bool Select(const Ray &camera_ray, const Transform &transform) override;
		virtual bool Update(const Ray &camera_ray, Transform *transform) override;
				
	public:
		RotationWidget();
		~RotationWidget();

	private:
		virtual void CalculateBoundingBoxes() override;	
	};

	class ScalingWidget : public Widget3D
	{
	public:
		Plane transformation_plane;
		OBB x_bounding_volume;
		OBB y_bounding_volume;
		OBB z_bounding_volume;
		bool is_selected = false;

	public:
		virtual void Create(const GLMesh &mesh) override;
		virtual bool Select(const Ray &camera_ray, const Transform &transform) override;
		virtual bool Update(const Ray &camera_ray, Transform *transform) override;

		// @NOTE: We don't support arbitary scaling directions
		//		: This stems from the fact that we store out scaling in a 
		//		: object space Vec3 and thus making it impossible to 
		//		: scale arbitary directions. At least I think.
		virtual void WorldAlign() override;
	public:
		ScalingWidget();
		~ScalingWidget();

	private:
		virtual void CalculateBoundingBoxes() override;
	};

	
	// @NOTE: I think we have mutlible instances for undos ???
	class EditorContext
	{
	public:
		WorldObject *selected_world_object = nullptr;
		Widget3D *current_widget = nullptr;
		bool draw_editor = true;
		bool mouse_input_for_editor_window = false;
	};



	class EditorConsole
	{		
	public:
		void LogInfo(const String &msg);
		void LogInfo(const bool msg);

		void LogWarning(const String &msg);
		void LogWarning(const bool msg);

		void UpdateAndDraw();
		void ClearLogger();
		void ExeCommand(const String &cmd);

	public:
		String title = "Console";

	private:
		char input_buffer[256];
		StringStream commands;
		StringStream log_info;
		StringStream log_warnings;
		StringStream history;


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

	public: 
		void SetWorldObject(WorldObject *world_object);
		bool UpdateAndDraw();
		
		

	public:
		EditorWorldObjectInspector();
		EditorWorldObjectInspector(WorldObject *actor);
		~EditorWorldObjectInspector();

	private:
		WorldObject *world_object = nullptr;


		// @NOTE: I do this because we loose the non-canonical euler rotation when we are 
		//		: converting from qaut to euler as it gives us the canonical form. This dummy var
		//		: then acts as out non canonical form which the user plays with and then we convert 
		//		: back into a quat. This has the down side landing in gimbal lock, but is much better
		//		: than having to rewrite the whole transform class inorder to store a euler rotation
		Vec3f euler_angles = Vec3f(0);
		Quatf quat_angle = Quatf();
	};
}