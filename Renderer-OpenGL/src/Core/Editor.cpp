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
	
	//---------------------------------------------------------------
	//---------------------------------------------------------------
	//---------------------------------------------------------------

	TranslationWidget::TranslationWidget()
	{

	}

	TranslationWidget::~TranslationWidget()
	{

	}

	void TranslationWidget::SetTransform(const Transform &transform)
	{
		this->transform.position = transform.position;
		this->transform.rotation = transform.rotation;
		this->mesh = mesh;
		CalculateBoundingBoxes();
	}

	void TranslationWidget::Create(const GLMesh &mesh)
	{
		transform.position = Vec3(0);
		transform.scale = Vec3(0.04);		
		this->mesh = mesh;
		CalculateBoundingBoxes();
	}

	bool TranslationWidget::Select(const Ray &camera_ray, const Transform &transform)
	{
		is_selected = false;

		// @NOTE: Check x-axis
		bool x_axis= x_bounding_volume.CheckCollision(camera_ray);
		if (x_axis)
		{
			// @NOTE: Create a plane where the origin is the intersection and normal point towards the camera
			translation_plane = Plane(transform.position, -1 * camera_ray.direction);

			this->transform.position = transform.position;
			this->transform.rotation = transform.rotation;

			CollisionInfo colinfo;
			translation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3 p0 = translation_plane.origin;
			Vec3 p1 = camera_ray.Travel(colinfo.dist);

			// @NOTE: Offset the origin to stop snapping
			translation_plane.origin += p1 - p0;

			translation_mode = TransformationMode::x_axis;

			is_selected = true;
			return is_selected;
		}

		// @NOTE: Check y-axis
		bool y_axis = y_bounding_volume.CheckCollision(camera_ray);
		if (y_axis)
		{
			// @NOTE: Create a plane where the origin is the intersection and normal point towards the camera
			translation_plane = Plane(transform.position, -1 * camera_ray.direction);

			this->transform.position = transform.position;
			this->transform.rotation = transform.rotation;
			
			CollisionInfo colinfo;
			translation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3 p0 = translation_plane.origin;
			Vec3 p1 = camera_ray.Travel(colinfo.dist);

			// @NOTE: Offset the origin to stop snapping
			translation_plane.origin += p1 - p0;
			
			translation_mode = TransformationMode::y_axis;
			
			is_selected = true;
			return is_selected;
		}

		// @NOTE: Check z-axis
		bool z_axis = z_bounding_volume.CheckCollision(camera_ray);
		if (z_axis)
		{
			// @NOTE: Create a plane where the origin is the intersection and normal point towards the camera
			translation_plane = Plane(transform.position, -1 * camera_ray.direction);

			this->transform.position = transform.position;
			this->transform.rotation = transform.rotation;

			CollisionInfo colinfo;
			translation_plane.CheckCollision(camera_ray, &colinfo);


			Vec3 p0 = translation_plane.origin;
			Vec3 p1 = camera_ray.Travel(colinfo.dist);

			// @NOTE: Offset the origin to stop snapping
			translation_plane.origin += p1 - p0;

			translation_mode = TransformationMode::z_axis;

			is_selected = true;
			return is_selected;
		}

		return is_selected;
	}

	bool TranslationWidget::Update(const Ray &camera_ray, Transform *transform)
	{
		if (is_selected) 
		{
			Assert(transform);
			CollisionInfo colinfo;
			translation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3 p0 = translation_plane.origin;
			Vec3 p1 = camera_ray.Travel(colinfo.dist);
			
			Vec3 delta = p1 - p0;

			switch (translation_mode)
			{
			case cm::TransformationMode::none: {
				LOG(" WARNING: Translating nothing ??");
				break;
			}
			case cm::TransformationMode::x_axis: {
				int32 index = AbsMaxIndex(x_bounding_volume.basis.right);
				translation_plane.origin.arr[index] = translation_plane.origin.arr[index] + delta.arr[index];
				Vec3 dir(delta.arr[index] * Sign(x_bounding_volume.basis.right.arr[index]),0, 0);
				dir = Rotate((this->transform.rotation), dir);
				this->transform.position += dir;
				break;
			}
			case cm::TransformationMode::y_axis: {
				int32 index = AbsMaxIndex(y_bounding_volume.basis.upward);
				translation_plane.origin.arr[index] = translation_plane.origin.arr[index] + delta.arr[index];
				Vec3 dir(0, delta.arr[index] * Sign(y_bounding_volume.basis.upward.arr[index]), 0);
				dir = Rotate((this->transform.rotation), dir);
				this->transform.position += dir;
				break;
			}
			case cm::TransformationMode::z_axis: {
				int32 index = AbsMaxIndex(z_bounding_volume.basis.forward);
				translation_plane.origin.arr[index] = translation_plane.origin.arr[index] + delta.arr[index];
				Vec3 dir(0, 0, delta.arr[index] * Sign(z_bounding_volume.basis.forward.arr[index]));
				dir = Rotate((this->transform.rotation), dir);
				this->transform.position += dir;
				break;
			}
			default: {
				LOG(" WARNING: Translating nothing ??");
				break;
			}
			}
			transform->position = this->transform.position;
			CalculateBoundingBoxes();
		}
		return is_selected;
	}

	void TranslationWidget::CalculateBoundingBoxes()
	{
		// @NOTE: We take the conjugate because of model's blender coords ??
		Mat3 rotation = QuatToMat3(Conjugate(transform.rotation));
		Basis local_basis(rotation);

		Vec3 x_origin = Vec3(0.36, 0, 0) * local_basis.mat;
		Vec3 y_origin = Vec3(0, 0.36, 0) * local_basis.mat;
		Vec3 z_origin = Vec3(0, 0, 0.36) * local_basis.mat;

		x_bounding_volume = OBB(transform.position + x_origin, Vec3(0.30, 0.05, 0.05), local_basis);
		y_bounding_volume = OBB(transform.position + y_origin, Vec3(0.05, 0.30, 0.05), local_basis);
		z_bounding_volume = OBB(transform.position + z_origin, Vec3(0.05, 0.05, 0.30), local_basis);
	}

	const GLMesh TranslationWidget::GetMeshForRender()
	{
		return mesh;
	}

	//---------------------------------------------------------------
	//---------------------------------------------------------------
	//---------------------------------------------------------------
	
	RotationWidget::RotationWidget()
	{

	}

	RotationWidget::~RotationWidget()
	{

	}

	void RotationWidget::SetTransform(const Transform &transform)
	{
		this->transform.position = transform.position;
		this->transform.rotation = transform.rotation;
		CalculateBoundingBoxes();
	}

	void RotationWidget::Create(const GLMesh &mesh)
	{
		transform.position = Vec3(0);
		transform.scale = Vec3(0.04);
		this->mesh = mesh;
		CalculateBoundingBoxes();
	}

	bool RotationWidget::Select(const Ray &camera_ray, const Transform &transform)
	{
		// @TODO: Make is select the axis closest to the ray
		is_selected = false;
		for (int32 i = 0; i < 4; i++)
		{
			if (x_bounding_volumes[i].CheckCollision(camera_ray))
			{
				transformation_mode = TransformationMode::x_axis;
				transform_plane = Plane(x_bounding_volumes[i].origin, camera_ray.direction);

				this->transform.position = transform.position;
				this->transform.rotation = transform.rotation;

				is_selected = true;
				return is_selected;
			}		
		}
		
		for (int32 i = 0; i < 4; i++)
		{
			if (y_bounding_volumes[i].CheckCollision(camera_ray))
			{
				transformation_mode = TransformationMode::y_axis;
				transform_plane = Plane(y_bounding_volumes[i].origin, camera_ray.direction);
				
				this->transform.position = transform.position;
				this->transform.rotation = transform.rotation;

				is_selected = true;
				return is_selected;
			}
		}

		for (int32 i = 0; i < 4; i++)
		{
			if (z_bounding_volumes[i].CheckCollision(camera_ray))
			{
				transformation_mode = TransformationMode::z_axis;
				transform_plane = Plane(z_bounding_volumes[i].origin, camera_ray.direction);
				
				this->transform.position = transform.position;
				this->transform.rotation = transform.rotation;

				is_selected = true;
				return is_selected;
			}
		}
		return is_selected;
	}

	bool RotationWidget::Update(const Ray &camera_ray, Transform *transform)
	{
		if (is_selected)
		{
			CollisionInfo colinfo;
			transform_plane.CheckCollision(camera_ray, &colinfo);

			Vec3 p0 = transform_plane.origin;
			Vec3 p1 = camera_ray.Travel(colinfo.dist);

			Vec3 d0 = Normalize(p0 - this->transform.position);
			Vec3 d1 = Normalize(p1 - this->transform.position);
			
			switch (transformation_mode)
			{
			case cm::TransformationMode::none: {
				LOG(" WARNING: Rotating nothing ??");
				break;
			}
			case cm::TransformationMode::x_axis: {
				// @NOTE: Get the axis of rotation
				Basis b = x_bounding_volumes[0].basis;

				// @NOTE: Normalize for safety
				Vec3 ncr = Normalize(camera_ray.direction);
				Vec3 nbr = Normalize(b.right);

				// @NOTE: Get the angle theta from d0 to d1. The clamp is for floating point errors 
				real32 theta = Clamp(Dot(d0, d1), -1, 1);

				// @NOTE: The first dot is determining which way the user is dragging the widget
				//		: The second is determining if the user is facing away or two the right basis vector
				//		: of the widget. This is to determine the correct rotation direction
				int32 dir = Sign( Dot(Cross(d0, d1), ncr) ) * Sign( Dot(ncr, nbr) ) * -1;

				// @NOTE: Conversions
				theta = RadToDeg(acos(theta) * dir);
				
				// @NOTE: Apply the rotation
				Quat r = Rotate(this->transform.rotation, theta, nbr);			
				this->transform.rotation = r * this->transform.rotation;

				// @NOTE: Put the origin where the ray intersected the plane
				transform_plane.origin = p1;
				break; 
			}
			case cm::TransformationMode::y_axis: {
				// @NOTE: Get the axis of rotation
				Basis b = y_bounding_volumes[0].basis;

				// @NOTE: Normalize for safety
				Vec3 ncr = Normalize(camera_ray.direction);
				Vec3 nbu = Normalize(b.upward);

				// @NOTE: Get the angle theta from d0 to d1. The clamp is for floating point errors 
				real32 theta = Clamp(Dot(d0, d1), -1, 1);

				// @NOTE: The first dot is determining which way the user is dragging the widget
				//		: The second is determining if the user is facing away or two the right basis vector
				//		: of the widget. This is to determine the correct rotation direction
				int32 dir = Sign(Dot(Cross(d0, d1), ncr)) * Sign(Dot(ncr, nbu)) * -1;

				// @NOTE: Conversions
				theta = RadToDeg(acos(theta) * dir);

				// @NOTE: Apply the rotation
				Quat r = Rotate(this->transform.rotation, theta, nbu);
				this->transform.rotation = r * this->transform.rotation;

				// @NOTE: Put the origin where the ray intersected the plane
				transform_plane.origin = p1;
				break;
			}
			case cm::TransformationMode::z_axis: {
				Basis b = z_bounding_volumes[0].basis;

				// @NOTE: Normalize for safety
				Vec3 ncr = Normalize(camera_ray.direction);
				Vec3 nbf = Normalize(b.forward);

				// @NOTE: Get the angle theta from d0 to d1. The clamp is for floating point errors 
				real32 theta = Clamp(Dot(d0, d1), -1, 1);

				// @NOTE: The first dot is determining which way the user is dragging the widget
				//		: The second is determining if the user is facing away or two the right basis vector
				//		: of the widget. This is to determine the correct rotation direction
				int32 dir = Sign(Dot(Cross(d0, d1), ncr)) * Sign(Dot(ncr, nbf)) * -1;

				// @NOTE: Conversions
				theta = RadToDeg(acos(theta) * dir);

				// @NOTE: Apply the rotation
				Quat r = Rotate(this->transform.rotation, theta, nbf);
				this->transform.rotation = r * this->transform.rotation;

				// @NOTE: Put the origin where the ray intersected the plane
				transform_plane.origin = p1;
				break;
			}
			default: {
				LOG(" WARNING: Rotating nothing ??");
				break;
			}
			}
			transform->rotation = this->transform.rotation;	
			CalculateBoundingBoxes();
		}
		return is_selected;
	}

	void RotationWidget::CalculateBoundingBoxes()
	{		
		Mat3 rotation = QuatToMat3(Conjugate(transform.rotation));
		Basis local_basis(rotation);

		// @NOTE: x-axis 
		Vec3 x_origin01 = Vec3(0, 0.4, 0.4)	  * local_basis.mat;
		Vec3 x_origin02 = Vec3(0, -0.4, -0.4) * local_basis.mat;
		Vec3 x_origin03 = Vec3(0, 0.4, -0.4)  * local_basis.mat;
		Vec3 x_origin04 = Vec3(0, -0.4, 0.4)  * local_basis.mat;

		x_bounding_volumes[0] = OBB(transform.position + x_origin01, Vec3(0.05, 0.2, 0.05), Rotate(local_basis.mat, -45, Vec3(1, 0, 0)));
		x_bounding_volumes[1] = OBB(transform.position + x_origin02, Vec3(0.05, 0.2, 0.05), Rotate(local_basis.mat, -45, Vec3(1, 0, 0)));
		x_bounding_volumes[2] = OBB(transform.position + x_origin03, Vec3(0.05, 0.2, 0.05), Rotate(local_basis.mat, 45, Vec3(1, 0, 0)));
		x_bounding_volumes[3] = OBB(transform.position + x_origin04, Vec3(0.05, 0.2, 0.05), Rotate(local_basis.mat, 45, Vec3(1, 0, 0)));

		// @NOTE: y-axis 
		Vec3 y_origin01 = Vec3(0.4, 0, 0.4)	  * local_basis.mat;
		Vec3 y_origin02 = Vec3(-0.4, 0, -0.4) * local_basis.mat;
		Vec3 y_origin03 = Vec3(0.4, 0, -0.4)  * local_basis.mat;
		Vec3 y_origin04 = Vec3(-0.4, 0, 0.4)  * local_basis.mat;

		y_bounding_volumes[0] = OBB(transform.position + y_origin01, Vec3(0.2, 0.05, 0.05), Rotate(local_basis.mat, 45, Vec3(0, 1, 0)));
		y_bounding_volumes[1] = OBB(transform.position + y_origin02, Vec3(0.2, 0.05, 0.05), Rotate(local_basis.mat, 45, Vec3(0, 1, 0)));
		y_bounding_volumes[2] = OBB(transform.position + y_origin03, Vec3(0.2, 0.05, 0.05), Rotate(local_basis.mat, -45, Vec3(0, 1, 0)));
		y_bounding_volumes[3] = OBB(transform.position + y_origin04, Vec3(0.2, 0.05, 0.05), Rotate(local_basis.mat, -45, Vec3(0, 1, 0)));

		// @NOTE: z-axis, 0.6 because of pythag.
		Vec3 z_origin01 = Vec3(0.6, 0, 0)	* local_basis.mat;
		Vec3 z_origin02 = Vec3(0, 0.6, 0)	* local_basis.mat;
		Vec3 z_origin03 = Vec3(0, -0.6, 0)	* local_basis.mat;
		Vec3 z_origin04 = Vec3(-0.6, 0, 0)	* local_basis.mat;

		z_bounding_volumes[0] = OBB(transform.position + z_origin01, Vec3(0.05, 0.2, 0.05), local_basis);
		z_bounding_volumes[1] = OBB(transform.position + z_origin02, Vec3(0.2, 0.05, 0.05), local_basis);
		z_bounding_volumes[2] = OBB(transform.position + z_origin03, Vec3(0.2, 0.05, 0.05), local_basis);
		z_bounding_volumes[3] = OBB(transform.position + z_origin04, Vec3(0.05, 0.2, 0.05), local_basis);
	}

	const cm::GLMesh RotationWidget::GetMeshForRender() const
	{
		return mesh;
	}

}