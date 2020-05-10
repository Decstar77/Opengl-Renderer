#include <glew.h>
#include <GLFW/glfw3.h>
#include "src/Core.h"
#include "src/OpenGL/RenderCommands.h"
#include "src/Math/CosmicPhysics.h"
#include "src/Math/CosmicNoise.h"
#include "src/OpenGL/OpenGL.h"
#include "src/Core/EditableMesh.h"
#include "src/Core/Editor.h"
#include "src/Core/Debug.h"
#include "src/Engine/AssetLoader.h"
#include "src/Engine/Input.h"
#include "src/Core/Sandbox.h"
#include "src/Core/Serialization.h"
#include "src/Core/Camera.h"
using namespace cm;


// @TODO: Make platform thingy. Check SSAO shader and Imgui frame they have hardcoded values
static const uint32 WINDOW_WIDTH = 1280;
static const uint32 WINDOW_HEIGHT = 720;
//static const uint32 WINDOW_WIDTH = 1920;
//static const uint32 WINDOW_HEIGHT = 1080;

static const float MOUSE_SENSITIVITY = 0.08f;

static CameraController camera_controller = {};
static MassAggregateEngine physics_engine;
static GLFWwindow *window = nullptr;
static float delta_time = 0;

static bool DRAW_LIGHT_POSITIONS = false;
static bool move_camera = false;

EditorContext current_context;

void MousePositionCallBack(GLFWwindow *widow, double xpos, double ypos)
{
	// @NOTE: GetLastMouse is now actually 2 frames behind. Thats why we use GetMouse. 
	Vec2f current_mouse = Vec2f(static_cast<real>(xpos), static_cast<real>(ypos));
	Vec2f last_mouse = Input::GetMousePosition();

	real xoffset = current_mouse.x - last_mouse.x;
	real yoffset = last_mouse.y - current_mouse.y; 

	xoffset *= MOUSE_SENSITIVITY;
	yoffset *= MOUSE_SENSITIVITY;
	if (move_camera)
	{
		camera_controller.CameraRotate(yoffset, xoffset);
	}
	
	Input::SetMousePosition(current_mouse.x, current_mouse.y);	
}

void MouseButtonCallBack(GLFWwindow* window, int32 button, int32 action, int32 mods)
{
	if (action == GLFW_PRESS)
	{
		Input::SetMouseKeyState(button, true);
	}
	else if (action == GLFW_RELEASE)
	{
		Input::SetMouseKeyState(button, false);
	}
}

void KeyCodeCallBack(GLFWwindow *window, int32 key, int32 scancode, int32 action, int32 mod)
{
	if (action == GLFW_PRESS)
	{
		Input::SetKeyState(key, true);
	}
	else if (action == GLFW_RELEASE)
	{
		Input::SetKeyState(key, false);
	}	
}

GLFWwindow* CreateRenderingWindow()
{
	if (!glfwInit())
	{
		Assert(0);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

#if 1
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	GLFWwindow* window;
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //No leagcy function
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Open-Gl Renderer", 0, 0);
	if (!window)
	{
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//DESC: Callbacks
	glfwSetCursorPosCallback(window, MousePositionCallBack);
	glfwSetKeyCallback(window, KeyCodeCallBack);
	glfwSetMouseButtonCallback(window, MouseButtonCallBack);
	return window;
}

int main()
{
	window = CreateRenderingWindow();
	InitializeOpenGl(WINDOW_WIDTH, WINDOW_HEIGHT);
	InitializeDebug(WINDOW_WIDTH, WINDOW_HEIGHT, 5000);
	InitializeEditor(window);
	
	
	OpenGLStatistics opengl_stats;
	GetOpenglStatistics(&opengl_stats);
	PrintOpenglStatistics(opengl_stats);

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
	GLint nTotalMemoryInKB = 0;
	GLint nCurAvailMemoryInKB = 0;

	glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
		&nTotalMemoryInKB);


	glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
		&nCurAvailMemoryInKB);
	std::cout << "Total memory: " << (float)nTotalMemoryInKB * 0.001f << std::endl;
	std::cout << "Ava memory: " << (float)nCurAvailMemoryInKB  * 0.001f << std::endl;

	int NumberOfExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
	for (int i = 0; i < NumberOfExtensions; i++) {
		const GLubyte *ccc = glGetStringi(GL_EXTENSIONS, i);
		//std::cout << ccc << std::endl;
	}

	if (GLEW_EXT_texture_filter_anisotropic)
	{
		float a = GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT;
		std::cout << "MAX ANTI  " << a << std::endl;
	}

	int32 min = Min(1, 2, 3, 4, 5);

	//************************************
	// Load shaders
	//************************************

	Shader worldspace_gbuffer_shader;
	worldspace_gbuffer_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/worldspace_gbuffer_vert.glsl");
	worldspace_gbuffer_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/worldspace_gbuffer_frag.glsl");
	worldspace_gbuffer_shader.name = "demo_bot_gbuffer_shader";
	CreateShader(&worldspace_gbuffer_shader);

	Shader worldspace_gbuffer_anim_shader;
	worldspace_gbuffer_anim_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/worldspace_gbuffer_anim_vert.glsl");
	worldspace_gbuffer_anim_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/worldspace_gbuffer_frag.glsl");
	worldspace_gbuffer_anim_shader.name = "demo_bot_gbuffer_anim_shader";
	CreateShader(&worldspace_gbuffer_anim_shader);

	Shader viewspace_gbuffer_shader;
	viewspace_gbuffer_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/viewspace_gbuffer_vert.glsl");
	viewspace_gbuffer_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/viewspace_gbuffer_frag.glsl");
	viewspace_gbuffer_shader.name = "viewspace_gbuffer_shader";
	CreateShader(&viewspace_gbuffer_shader);

	Shader ssao_shader;
	ssao_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/screenspace_ambient_occlusion_vert.glsl");
	ssao_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/screenspace_ambient_occlusion_frag.glsl");
	ssao_shader.name = "screenspace_ambient_occlusion";
	CreateShader(&ssao_shader);

	Shader depth_shader;
	depth_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/depth_vert.glsl");
	depth_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/depth_frag.glsl");
	depth_shader.name = "depth shader";
	CreateShader(&depth_shader);

	Shader pbr_deffered_shader;
	pbr_deffered_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/pbr_deffered_vert.glsl");
	pbr_deffered_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/pbr_deffered_frag.glsl");
	pbr_deffered_shader.name = "demo_01_deffered_pbr_shader";
	CreateShader(&pbr_deffered_shader);

	Shader pbr_forward_shader;
	pbr_forward_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/pbr_forward_vert.glsl");
	pbr_forward_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/pbr_forward_frag.glsl");
	pbr_forward_shader.name = "pbr_forward_shader";
	CreateShader(&pbr_forward_shader);

	Shader postprocessing_shader;
	postprocessing_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/postprocessing_vert.glsl");
	postprocessing_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/postprocessing_frag.glsl");
	postprocessing_shader.name = "demo_01_postprocessing_shader";
	CreateShader(&postprocessing_shader);
	
	Shader skybox_shader;
	skybox_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/skybox_vert.glsl");
	skybox_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/skybox_frag.glsl");
	skybox_shader.name = "cubemap_shader";
	CreateShader(&skybox_shader);

	Shader ssr_shader;
	ssr_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/screenspace_reflections_vert.glsl");
	ssr_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/screenspace_reflections_frag.glsl");
	ssr_shader.name = "ssr_shader ";
	CreateShader(&ssr_shader);
	



	//************************************
	// Load editor assets
	//************************************

	ModelImport model_import_engine;
	model_import_engine.import_animations = true;
	model_import_engine.import_vertex_binorms_tangents = true;
	model_import_engine.model_paths.push_back("res/engine/meshes/sphere.obj");
	model_import_engine.model_paths.push_back("res/engine/meshes/translation_widget.dae");
	model_import_engine.model_paths.push_back("res/engine/meshes/rotation_widget.dae");
	model_import_engine.model_paths.push_back("res/engine/meshes/scalling_widget.dae");
	model_import_engine.Load();
	StandardMeshes::sphere = model_import_engine.resulting_meshes[0].CreateMesh(true);
	
	TranslationWidget translation_widget;
	translation_widget.Create(model_import_engine.resulting_meshes[1].CreateMesh());

	RotationWidget rotation_widget;
	rotation_widget.Create(model_import_engine.resulting_meshes[2].CreateMesh());

	ScalingWidget scaling_widget;
	scaling_widget.Create(model_import_engine.resulting_meshes[3].CreateMesh());

	Shader transform_widget_shader;
	transform_widget_shader.config.src_vert = ReadTextFile("res/engine/shaders/editor_widget_vert.glsl");
	transform_widget_shader.config.src_frag = ReadTextFile("res/engine/shaders/editor_widget_frag.glsl");
	CreateShader(&transform_widget_shader);

	Shader editor_light_shader;
	editor_light_shader.config.src_vert = ReadTextFile("res/engine/shaders/editor_light_vert.glsl");
	editor_light_shader.config.src_frag = ReadTextFile("res/engine/shaders/editor_light_frag.glsl");
	CreateShader(&editor_light_shader);

	//************************************
	// Load texture assets
	//************************************

#if 0
	TextureImportMultiThread texture_importer_multi;
	texture_importer_multi.flip_image = true;
	texture_importer_multi.SetTexturePath("res/botdemo/textures/skyboxes/Sky.jpg");
	texture_importer_multi.Load();
	
	Texture demo_skybox_eqi;
	demo_skybox_eqi.config = *texture_importer_multi.GetConfig();	
	CreateTexture(&demo_skybox_eqi, texture_importer_multi.GetData()->data());
	
	CubeMap demo_skybox;
	demo_skybox.config.width = 512;
	demo_skybox.config.height = 512;
	CreateCubeMap(&demo_skybox, nullptr);
	
	CubeMapGenerator cubemap_generator;	
	cubemap_generator.Create();
	cubemap_generator.Convert(demo_skybox_eqi, &demo_skybox);
	cubemap_generator.Free();

	FreeTexture(&demo_skybox_eqi);
#endif
#if 1
	//************************************
	// Initialize imported textures
	//************************************
	TextureBank texture_bank;
	texture_bank.Create();
	LoadTextureFolder(&texture_bank, "res/botdemo/textures/lady", false, true);


#endif
		
	//************************************
	// Initialize imported meshes 
	//************************************


	//************************************
	// Create the frame buffers
	//************************************

	FrameBuffer worldspace_gbuffer;
	CreateFrameBufferGBuffer(&worldspace_gbuffer, WINDOW_WIDTH, WINDOW_HEIGHT);

	FrameBuffer viewspace_gbuffer;
	CreateFrameBufferGBuffer(&viewspace_gbuffer, WINDOW_WIDTH, WINDOW_HEIGHT);

	FrameBuffer ssao_buffer;	
	CreateFrameBufferColourOnly(&ssao_buffer, WINDOW_WIDTH, WINDOW_HEIGHT);

	FrameBuffer ssr_frame;
	CreateFrameBufferColourOnly(&ssr_frame, WINDOW_WIDTH, WINDOW_HEIGHT);

	FrameBuffer post_processing_buffer;
	CreateFrameBufferColourDepth(&post_processing_buffer, WINDOW_WIDTH, WINDOW_HEIGHT);

	FrameBuffer final_buffer;
	CreateFrameBufferColourDepth(&final_buffer, WINDOW_WIDTH, WINDOW_HEIGHT);
	   	
	FrameBuffer depth_buffer;
	depth_buffer.colour0_texture_attachment.config.width = WINDOW_WIDTH;
	depth_buffer.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
	depth_buffer.colour0_texture_attachment.config.texture_format = GL_RG32F;
	depth_buffer.colour0_texture_attachment.config.pixel_format = GL_RG;
	depth_buffer.colour0_texture_attachment.config.data_type = GL_FLOAT;
	depth_buffer.colour0_texture_attachment.config.min_filter = GL_LINEAR;
	depth_buffer.colour0_texture_attachment.config.mag_filter = GL_LINEAR;
	depth_buffer.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	depth_buffer.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;

	depth_buffer.depthstencil_attchment.width = WINDOW_WIDTH;
	depth_buffer.depthstencil_attchment.height = WINDOW_HEIGHT;
	depth_buffer.depthstencil_attchment.render_buffer_format = GL_DEPTH_COMPONENT32;
	depth_buffer.depthstencil_attchment.render_buffer_attachment_type = GL_DEPTH_ATTACHMENT;

	CreateTexture(&depth_buffer.colour0_texture_attachment, nullptr);
	CreateDepthStencilBuffer(&depth_buffer.depthstencil_attchment);
	CreateFrameBuffer(&depth_buffer);

	BindFrameBuffer(depth_buffer);

	FrameBufferBindColourAttachtments(&depth_buffer);
	FrameBufferBindRenderAttachtment(&depth_buffer);

	Assert(CheckFrameBuffer(depth_buffer));
	UnbindFrameBuffer();


	Vec3f test1 = Vec3f(1, 1, 1);
	Vec3f test2 = Vec3f(1, 1, 1);
	Vec3f test3 = test1 + test2;


	uint32 drawing_width = post_processing_buffer.colour0_texture_attachment.config.width;
	uint32 drawing_height = post_processing_buffer.colour0_texture_attachment.config.height;
	
	LuminanceFilter lumin_filter;
	lumin_filter.Create(drawing_width, drawing_height, 1);

	HemisphereKernel hemisphere_kernel;
	hemisphere_kernel.Create(32, 4);

	GaussianTextureBlur gblur;
	gblur.Create(drawing_width, drawing_height, 11, 1.0 / 2.0, 1);

	SimpleTextureBlur sblur;
	sblur.Create(drawing_width, drawing_height, 2);

	Texture ssao_map;
	ssao_map.config = post_processing_buffer.colour0_texture_attachment.config;
	CreateTexture(&ssao_map, nullptr);

	Texture bloom_map;
	bloom_map.config = post_processing_buffer.colour0_texture_attachment.config;
	CreateTexture(&bloom_map, nullptr);
	   

	//************************************
	// Set opengl state
	//************************************

	SetViewPort(WINDOW_WIDTH, WINDOW_WIDTH);
	
	EnableFaceCulling();
	CullBackFace();
	
	EnableDepthBuffer();
	DepthBufferFunction(GL_LEQUAL);
	
	EnableCubeMapSeamless();
	
#if EDITOR_WINDOW
	EnableStencilBuffer();
	StencilDiscardOnOne();
	StencilOperationKKR();
#endif
	SetViewPort(WINDOW_WIDTH, WINDOW_HEIGHT);



	World main_world;


	camera_controller.main_camera.projection_matrix = Perspective(40, ((float)WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 250.0f);
	camera_controller.main_camera.target = Vec3f(0);
	camera_controller.main_camera.transform.position = Vec3f(0, 4, 5);
	camera_controller.main_camera.view_matrix = LookAt(camera_controller.main_camera.transform.position, camera_controller.main_camera.target, Vec3f(0, 1, 0));
	
	
	UniformBuffer global_uniforms;
	global_uniforms.size_bytes = sizeof(Mat4) * 3 + sizeof(Vec4f) * 23;
	CreateUniformBuffer(&global_uniforms);
	UniformBufferSetBindPoint(global_uniforms, 0);
	ShaderBindUniformBuffer(worldspace_gbuffer_shader, 0, "WorldState");


	RenderSettings render_settings;
	
	EditorConsole console;
	console.LogInfo("*****WELCOME TO A BAD RENDERER*****");
	console.LogInfo("General Stats");
	console.LogInfo("Vendor: " + opengl_stats.vendor);
	console.LogInfo("Renderer: " + opengl_stats.renderer);
	console.LogInfo("Version: " + opengl_stats.version);
	console.LogInfo("Shader lang: " + opengl_stats.shading_lang);

	EditorRenderSettingsWindow editor_render_window;
	editor_render_window.delta_time = 0;
	editor_render_window.render_settings = &render_settings;

	EditorWorldObjectSpawner editor_worldobject_spawner;
	editor_worldobject_spawner.world = &main_world;

	EditorWorldObjectInspector editor_world_object_inspector;
	
	DirectionalLight sun_light;
	sun_light.direction = Normalize(Vec3f(2, -4, 1));
	sun_light.light_colour = Vec3f(.5);
	
	PointLight point_light;
	point_light.light_colour = Vec3f(23.47, 21.31, 20.79);
	point_light.light_position = Vec3f(0, 2, -1);

	
	StaticWorldObject *floor = CreateWorldObjectPlane();
	floor->transform.rotation = EulerToQuat(Vec3f(90, 0, 0));
	floor->transform.scale = Vec3f(10);
	floor->GetCollider()->Update(floor->transform);


	ModelImport model_import_animation;
	model_import_animation.import_vertex_binorms_tangents = true;
	model_import_animation.model_paths.push_back("res/models/Idle.dae");
	model_import_animation.Load();


	AnimatedActor test_cube_guy;
	EditableMesh emesh = model_import_animation.resulting_meshes[0];
	test_cube_guy.mesh = emesh.CreateAnimMesh();
	test_cube_guy.animation_controller = model_import_animation.resulting_animation_controllers[0];
	test_cube_guy.transform.scale = Vec3f(1);
	test_cube_guy.transform.rotation = EulerToQuat(Vec3f(0, 0, 0));
	test_cube_guy.transform.position = Vec3f(0, 0, 0);

	model_import_animation.Free();

	main_world.RegisterWorldObject(floor);

	float fh = 1;
	float run_time = 1;
	bool toggel = false;
	
	current_context.selected_world_object = &test_cube_guy;
	while (!glfwWindowShouldClose(window))
	{
		//************************************
		// Process window and input events
		//************************************
		
		std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();
		
		glfwPollEvents();

		//************************************
		// Draw and update the editor 
		//************************************			
#if EDITOR_WINDOW
		if (current_context.draw_editor)
		{
			// @NOTE: Unbind any shader that was set.
			UnbindShader();

			// @NOTE: Set the clear colour to black for stencil errors
			SetClearColour(Vec4f(0, 0, 0, 1));

			// @NOTE: Configure the stencil buffer to write 1's where the editor draws
			StencilWriteOnes();

			// @NOTE: Clear all the buffers
			ClearAllBuffers();

			// @NOTE: Render the current editor frame
			EditorBeginFrame();

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 pos = viewport->GetWorkPos();
			pos.x += WINDOW_WIDTH - 350;
			ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(ImVec2(350, 720));
			ImGui::SetNextWindowViewport(viewport->ID);

			bool p_open;
			ImGui::Begin("DockSpace Demo", &p_open, window_flags);

			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id);

			ImGui::End();

			console.UpdateAndDraw();

			editor_render_window.delta_time = delta_time;
			editor_render_window.UpdateAndDraw();

			editor_worldobject_spawner.UpdateAndDraw();

			editor_world_object_inspector.SetWorldObject(current_context.selected_world_object);
			editor_world_object_inspector.UpdateAndDraw();


			ImGui::Begin("Spawner");
			if (ImGui::Button("Shader reset"))
			{
				FreeShader(&ssr_shader);
				ssr_shader.config.src_vert = ReadTextFile("res/botdemo/shaders/screenspace_reflections_vert.glsl");
				ssr_shader.config.src_frag = ReadTextFile("res/botdemo/shaders/screenspace_reflections_frag.glsl");
				CreateShader(&ssr_shader);
			}
			ImGui::End();

			EditorEndFrame();



			// @NOTE: Get any input that was for one of the editor windows	
			current_context.mouse_input_for_editor_window = *ImGui::GetIO().MouseDownOwned;

			// @NOTE: Disable writing to the stencil buffer
			StencilZeroMask();

			// @NOTE: Reset the clear colour to a noticable
			SetClearColour(Vec4f(0, 1, 0, 1));
		}
		else
		{
			StencilWriteOnes();
			ClearAllBuffers();		
		}
#else 
		// @NOTE: Clear all the buffers
		ClearAllBuffers();
#endif
	
		//************************************
		// Process Custom Events
		//************************************		

		// @NOTE: Update the 3D control wigets. 
		if (current_context.selected_world_object)
		{
			scaling_widget.SetTransform(*current_context.selected_world_object->GetTransform());
			rotation_widget.SetTransform(*current_context.selected_world_object->GetTransform());
			translation_widget.SetTransform(*current_context.selected_world_object->GetTransform());

			if (Input::GetKeyJustDown(GLFW_KEY_T))
			{
				current_context.current_widget = (current_context.current_widget == &translation_widget) ? nullptr : &translation_widget;
			}

			if (Input::GetKeyJustDown(GLFW_KEY_R))
			{
				current_context.current_widget = (current_context.current_widget == &rotation_widget) ? nullptr : &rotation_widget;
			}

			if (Input::GetKeyJustDown(GLFW_KEY_F))
			{
				current_context.current_widget = (current_context.current_widget == &scaling_widget) ? nullptr : &scaling_widget;
			}

			if (Input::GetKeyJustDown(GLFW_KEY_GRAVE_ACCENT))
			{
				if (current_context.current_widget) {
					if (current_context.current_widget->IsObjectAligning()) {
						current_context.current_widget->WorldAlign();
					}
					else {
						current_context.current_widget->ObjectAlign();
					}
				}
			}
		}

		if (Input::GetKeyHeldDown(GLFW_KEY_LEFT_SHIFT) && Input::GetKeyJustDown(GLFW_KEY_B))
		{
			current_context.draw_editor = !current_context.draw_editor;
			current_context.mouse_input_for_editor_window = false;
		}

		if (!(current_context.mouse_input_for_editor_window)) 
		{
			Vec2f curr = Input::GetMousePosition();
			Vec2f last = Input::GetMouseLastPosition();
			Ray cam_ray = camera_controller.RayFromCamera(curr, Vec2f(WINDOW_WIDTH, WINDOW_HEIGHT));

			if (Input::IsMouseJustDown(MOUSE_BUTTON_1))
			{
				// @NOTE: Do we hit a control widget
				bool using_widget = false;
				if (current_context.selected_world_object)
				{
					if (current_context.current_widget)
					{
						Transform transform = *current_context.selected_world_object->GetTransform();
						using_widget = current_context.current_widget->Select(cam_ray, transform);
					}				
				}
				if (!using_widget)
				{
					// @NOTE: Assume we miss everything
					current_context.selected_world_object = nullptr;
					
					real32 closest_dist = 9999999;
					for (int32 i = 0; i < main_world.objects.size(); i++)
					{
						WorldObject *current = main_world.objects[i];
						GeometricCollider *collider = current->GetCollider();
						CollisionInfo collision_info;

						if (collider == nullptr)
						{
							continue;
						}
						
						if (collider->CheckCollision(cam_ray, &collision_info))
						{
							if (collision_info.dist < closest_dist)
							{						
								closest_dist = collision_info.dist;
								// @NOTE: Select our new object
								current_context.selected_world_object = current;

								// @NOTE: Snap the controll widgets to the newly selected object
								if (current_context.current_widget)
								{
									Transform transform = *current_context.selected_world_object->GetTransform();
									current_context.current_widget->SetTransform(transform);
								}
							}
						}
					}					
				}
			}
			if (Input::IsMouseJustUp(MOUSE_BUTTON_1))
			{
				//translation_widget.is_selected = false;
			}
			if (Input::IsMouseHeldDown(MOUSE_BUTTON_1))
			{
				if (current_context.selected_world_object)
				{
					if (current_context.current_widget)
					{
						if (Input::GetKeyHeldDown(GLFW_KEY_LEFT_CONTROL))
						{
							current_context.current_widget->SetSnappingAmount(15);
						}
						else
						{
							current_context.current_widget->SetSnappingAmount(0);
						}

						Transform *transform = current_context.selected_world_object->GetTransform();
						GeometricCollider *collider = current_context.selected_world_object->GetCollider();
						current_context.current_widget->Update(cam_ray, transform);
						if (collider)
						{
							collider->Update(transform);
						}
					}		
				}
			}
			else if (GLFW_RELEASE == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
			{

			}
		}

		
		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
		{

			//point_light.light_position.x -= 4 * delta_time;
			toggel = true;
			fh -= 0.4f * delta_time;

		}
		else if (GLFW_RELEASE == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
		{

			toggel = false;
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
		}

		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			camera_controller.UpdateCamera(delta_time);
			move_camera = true;
		}
		else if (GLFW_RELEASE == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			move_camera = false;
		}

		//************************************
		// Update GPU textures
		//************************************


		if (texture_bank.LockStagingArea())
		{
			while (texture_bank.PopTextureOnStagingArea())
			{
				LOG("Poping textures");
			}
			texture_bank.UnlockStagingArea();
		}

		//************************************
		// Update GPU Buffers
		//************************************

		std::vector<Mat4> camera_data = { 
			camera_controller.main_camera.projection_matrix, 
			camera_controller.main_camera.view_matrix, 
			Mat4(1)
		};

		std::vector<Vec4f> lighting_data = {
			Vec4f(1,  1, 0, 0), 
			Vec4f(camera_controller.main_camera.transform.position, 0),
			Vec4f(sun_light.direction, 0),
			Vec4f(point_light.light_position, 0), Vec4f(1), Vec4f(1), Vec4f(1),
			Vec4f(point_light.light_colour, 0), Vec4f(1), Vec4f(1), Vec4f(1)
		};

		
		WriteBufferData(&global_uniforms, camera_data, 0);
		// @HACK: Should write in one go, defeats the purpose !
		WriteBufferData(&global_uniforms, lighting_data, sizeof(Mat4) * 3);

		Debug::Update(camera_controller.main_camera.projection_matrix, camera_controller.main_camera.view_matrix);

#pragma region RenderingLogic
		//************************************
		// Render The Current Frame
		//************************************

		//************************************
		// Render depth buffers 
		//************************************

		BindFrameBuffer(depth_buffer);
		BindShader(depth_shader);

		ClearColourBuffer();
		ClearDepthBuffer();

		ShaderSetMat4(&depth_shader, "projection", camera_controller.main_camera.projection_matrix.ptr);
		ShaderSetMat4(&depth_shader, "view", camera_controller.main_camera.view_matrix.ptr);

		for (int32 i = 0; i < main_world.objects.size(); i++)
		{
			WorldObject *obj	= main_world.objects[i];
			Transform transform = *obj->GetTransform();
			Material mat		= *obj->GetMaterial();
			Mat4 transform_matrix = transform.CalcTransformMatrix();

			ShaderSetMat4(&depth_shader, "model", transform_matrix.ptr);

			RenderMesh(depth_shader, obj->GetMeshForRender());
		}

		//************************************
		// Bind and clear the gbuffer 
		//************************************
		BindFrameBuffer(worldspace_gbuffer);

		ClearColourBuffer();
		ClearDepthBuffer();

		//************************************
		// World gbuffer pass for static objects
		//************************************

		BindShader(worldspace_gbuffer_shader);

		// @DANGER: This loop is not safe, we assume next is the last element
		for (uint32 i = 0; i < main_world.defferd_next; i++)
		{
			WorldObject *obj	= main_world.defferd_objects[i];
			Transform transform = *obj->GetTransform();
			Material mat		= *obj->GetMaterial();
			Mat4 transform_matrix = transform.CalcTransformMatrix();
				
			// @TODO: Some sort of sorting (TextureSet) ?? For this non instanced draw call
			if (mat.HasTextures())
			{
				// @TODO: Now that we know stuff about branching we can use Static-Flow controll booleans
				ShaderSetVec3(&worldspace_gbuffer_shader, "colour_set", Vec3f(1).ptr);
				ShaderSetInt32(&worldspace_gbuffer_shader, "normal_set", 1);
				ShaderSetVec3(&worldspace_gbuffer_shader, "orm_set", Vec3f(1).ptr);


				mat.SetTextures(&worldspace_gbuffer_shader);
			}
			else
			{
				// @TODO: Now that we know stuff about branching we can use Static-Flow controll booleans
				ShaderBindTexture(worldspace_gbuffer_shader, StandardTextures::GetOneTexture(), 0, "colour_map");
				ShaderBindTexture(worldspace_gbuffer_shader, StandardTextures::GetOneTexture(), 2, "orme_map");
				ShaderSetInt32(&worldspace_gbuffer_shader, "normal_set", 0);

				mat.SetValues(&worldspace_gbuffer_shader);
			}

			ShaderSetMat4(&worldspace_gbuffer_shader, "model", transform_matrix.ptr);
			RenderMesh(worldspace_gbuffer_shader, obj->GetMeshForRender());
		}

		//************************************
		// World gbuffer pass for animated objects
		//************************************
#if 1
		BindShader(worldspace_gbuffer_anim_shader);

		ShaderSetVec3(&worldspace_gbuffer_anim_shader, "colour_set", Vec3f(1).ptr);
		ShaderSetInt32(&worldspace_gbuffer_anim_shader, "normal_set", 1);
		ShaderSetVec3(&worldspace_gbuffer_anim_shader, "orm_set", Vec3f(1).ptr);
	
		Texture lady_diffuse;
		Texture lady_normal;
		Texture lady_orme;
		
		texture_bank.Get("Arissa_DIFF_diffuse", &lady_diffuse);
		texture_bank.Get("Arissa_NM_normal", &lady_normal);
		texture_bank.Get("Arissa_SPEC_specular", &lady_orme);

		ShaderBindTexture(worldspace_gbuffer_anim_shader, lady_diffuse, 0, "colour_map");
		ShaderBindTexture(worldspace_gbuffer_anim_shader, lady_normal, 1, "normal_map");
		ShaderBindTexture(worldspace_gbuffer_anim_shader, lady_orme, 2, "orme_map");

		test_cube_guy.animation_controller.Play(0);
		test_cube_guy.animation_controller.Update(delta_time);

		for (int32 i = 1; i < test_cube_guy.animation_controller.bones.size(); i++)
		{
			StringStream ss;
			ss << "gBones[" << (i - 1) << "]";
			ShaderSetMat4(&worldspace_gbuffer_anim_shader, ss.str(), test_cube_guy.animation_controller.bones.at(i).current_transform.ptr);
		}
		ShaderSetMat4(&worldspace_gbuffer_anim_shader, "model", test_cube_guy.transform.CalcTransformMatrix().ptr);
		
		RenderMesh(worldspace_gbuffer_anim_shader, test_cube_guy.GetMeshForRender());
				
		UnbindFrameBuffer();
#endif
		//************************************
		// Viewspace gbuffer pass for static objects
		//************************************

		BindFrameBuffer(viewspace_gbuffer);
		BindShader(viewspace_gbuffer_shader);	

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
			  
		// @DANGER: This loop is not safe, we assume next is the last element
		for (uint32 i = 0; i < main_world.defferd_next; i++)
		{
			WorldObject *obj = main_world.defferd_objects[i];
			Transform transform = *obj->GetTransform();
			Material mat = *obj->GetMaterial();
			Mat4 transform_matrix = transform.CalcTransformMatrix();
			RenderFlags flags = obj->GetRenderFlags();

			if (flags == RENDERFLAG_NOTHING)
			{
				if (mat.HasTextures())
				{
					// @TODO: Now that we know stuff about branching we can use Static-Flow controll booleans
					ShaderSetVec3(&viewspace_gbuffer_shader, "colour_set", Vec3f(1).ptr);
					ShaderSetInt32(&viewspace_gbuffer_shader, "normal_set", 1);
					ShaderSetVec3(&viewspace_gbuffer_shader, "orm_set", Vec3f(1).ptr);


					mat.SetTextures(&viewspace_gbuffer_shader);
				}
				else
				{
					// @TODO: Now that we know stuff about branching we can use Static-Flow controll booleans
					ShaderBindTexture(viewspace_gbuffer_shader, StandardTextures::GetOneTexture(), 0, "colour_map");
					ShaderBindTexture(viewspace_gbuffer_shader, StandardTextures::GetOneTexture(), 2, "orme_map");
					ShaderSetInt32(&viewspace_gbuffer_shader, "normal_set", 0);

					mat.SetValues(&viewspace_gbuffer_shader);
				}


				ShaderSetMat4(&viewspace_gbuffer_shader, "model", transform_matrix.ptr);
				RenderMesh(viewspace_gbuffer_shader, obj->GetMeshForRender());
			}
		}

		UnbindFrameBuffer();

		//************************************
		// Viewspace gbuffer pass for animated objects
		//************************************
		
		//************************************
		// Screen space reflections (SSR)
		//************************************

		// @NOTE: Not working currently, in development, ie still learning cause I suck
		if (render_settings.ssr)
		{
			BindFrameBuffer(ssr_frame);
			BindShader(ssr_shader);

			RenderCommands::ClearColourBuffer();
			RenderCommands::ClearDepthBuffer();

			ShaderBindTexture(ssr_shader, viewspace_gbuffer.colour0_texture_attachment, 0, "view_position_map");
			ShaderBindTexture(ssr_shader, viewspace_gbuffer.colour1_texture_attachment, 1, "view_normal_map");
			ShaderBindTexture(ssr_shader, viewspace_gbuffer.colour2_texture_attachment, 2, "view_colour_map");
			ShaderBindTexture(ssr_shader, post_processing_buffer.colour0_texture_attachment, 3, "last_frame_map");

			ShaderSetFloat(&ssr_shader, "ray_step_length", render_settings.ssr_ray_step_length);
			ShaderSetFloat(&ssr_shader, "ray_max_distance", render_settings.ssr_ray_max_distance);
			ShaderSetFloat(&ssr_shader, "ray_hit_tollerance", render_settings.ssr_ray_hit_tollerance);

			ShaderSetMat4(&ssr_shader, "proj", camera_controller.main_camera.projection_matrix.ptr);
			RenderMesh(ssr_shader, StandardMeshes::quad);

			UnbindFrameBuffer();

			gblur.Blur(ssr_frame.colour0_texture_attachment, &ssr_frame.colour0_texture_attachment);
		}

		//************************************
		// Screen space ambient occulsion (SSAO)
		//************************************
		
		if (render_settings.ssao)
		{
			BindFrameBuffer(ssao_buffer);
			BindShader(ssao_shader);

			ClearColourBuffer();

			ShaderBindTexture(ssao_shader, viewspace_gbuffer.colour0_texture_attachment, 0, "g_position");
			ShaderBindTexture(ssao_shader, viewspace_gbuffer.colour1_texture_attachment, 1, "g_normal");
			ShaderBindTexture(ssao_shader, hemisphere_kernel.noise_texture, 2, "noise_texture");			
			
			if (render_settings.ssao_changed)
			{
				// @TODO: Remove !
				ShaderSetMat4(&ssao_shader, "projection", camera_controller.main_camera.projection_matrix.ptr);
				
				ShaderSetFloat(&ssao_shader, "strength", render_settings.ssao_strength);
				ShaderSetFloat(&ssao_shader, "radius", render_settings.ssao_radius);
				ShaderSetFloat(&ssao_shader, "bias", render_settings.ssao_bias);
				
				hemisphere_kernel.SetKernelSize(render_settings.ssao_kernel_size);
				
				for (uint32 i = 0; i < hemisphere_kernel.GetKernelSize(); i++)
				{
					ShaderSetVec3(&ssao_shader, "samples[" + std::to_string(i) + "]", hemisphere_kernel.GetKernelSample(i).ptr);
				}

				ShaderSetInt32(&ssao_shader, "kernel_size", hemisphere_kernel.GetKernelSize());

				render_settings.ssao_changed = false;
			}

			RenderMesh(ssao_shader, StandardMeshes::quad);

			UnbindFrameBuffer();

			sblur.Blur(ssao_buffer.colour0_texture_attachment, &ssao_map);
		}
		
		//************************************
		// Deffered pass
		//************************************

		BindFrameBuffer(post_processing_buffer);

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();

		BindShader(pbr_deffered_shader);

		ShaderBindTexture(pbr_deffered_shader, worldspace_gbuffer.colour0_texture_attachment, 0, "g_position");
		ShaderBindTexture(pbr_deffered_shader, worldspace_gbuffer.colour1_texture_attachment, 1, "g_normal");
		ShaderBindTexture(pbr_deffered_shader, worldspace_gbuffer.colour2_texture_attachment, 2, "g_colour");
		ShaderBindTexture(pbr_deffered_shader, render_settings.ssao ? ssao_map : StandardTextures::GetOneTexture(), 3, "g_ssao");
		
		RenderMesh(pbr_deffered_shader, StandardMeshes::quad);

		UnbindFrameBuffer();

		//************************************
		// Forward pass
		//************************************
			   
		FrameBufferCopyDepth(worldspace_gbuffer, &post_processing_buffer);
		BindFrameBuffer(post_processing_buffer);
		
		DisableFaceCulling();

		BindShader(skybox_shader);

		ShaderSetMat4(&skybox_shader, "projection", camera_controller.main_camera.projection_matrix.ptr);
		ShaderSetMat4(&skybox_shader, "view", camera_controller.main_camera.view_matrix.ptr);
		ShaderSetBool(&skybox_shader, "has_skybox", false);
		//ShaderBindCubeMap(&skybox_shader, demo_skybox, 0, "skybox");
		
		RenderMesh(skybox_shader, StandardMeshes::Cube());
		
		EnableFaceCulling();
		
		UnbindFrameBuffer();


		//************************************
		// Post processing setup
		//************************************

		if (render_settings.bloom)
		{
			lumin_filter.SetThreshold(render_settings.bloom_threshold);
			gblur.SetKernelSize(render_settings.bloom_kernel_size);
			if (render_settings.bloom_dependance == RenderSettings::BloomBlurDependance::Independent)
			{
				// @NOTE: In this case we need a much more accutrate, good blur
				//		: This way we don't have to do multi pass filtering				
				lumin_filter.Filter(post_processing_buffer.colour0_texture_attachment, &bloom_map);
				gblur.Blur(bloom_map, &bloom_map);
			}
			else if (render_settings.bloom_dependance == RenderSettings::BloomBlurDependance::Dependent)
			{
				// @NOTE: In this case we wan't more aggressive blur.
				//		: However the blur doesn't have to be good
				//		: The stronger the blur the greater the chance for flickering
				gblur.Blur(post_processing_buffer.colour0_texture_attachment, &bloom_map);
				lumin_filter.Filter(bloom_map, &bloom_map);
				gblur.Blur(bloom_map, &bloom_map);
			}
		}

		//************************************
		// Final render pass
		//************************************

		BindFrameBuffer(final_buffer);

		ClearColourBuffer();
		ClearDepthBuffer();

		DisableDepthBuffer();

		BindShader(postprocessing_shader);

		ShaderBindTexture(postprocessing_shader, post_processing_buffer.colour0_texture_attachment, 0, "scene_texture");
		ShaderBindTexture(postprocessing_shader, bloom_map, 1, "bloom_texture");

		ShaderSetInt32(&postprocessing_shader, "FXAA", render_settings.fxaa);
		ShaderSetFloat(&postprocessing_shader, "FXAA_SPAN_MAX", render_settings.fxaa_span_max);
		ShaderSetFloat(&postprocessing_shader, "FXAA_DIR_MIN", render_settings.fxaa_dir_min);
		ShaderSetFloat(&postprocessing_shader, "FXAA_DIR_REDUC", render_settings.fxaa_dir_reduc);
		
		ShaderSetInt32(&postprocessing_shader, "bloom", render_settings.bloom);

		ShaderSetFloat(&postprocessing_shader, "vigentte_outer_radius", render_settings.vigentte_outer_raduis * render_settings.vigentte);
		ShaderSetFloat(&postprocessing_shader, "vigentte_inner_radius", render_settings.vigentte_inner_raduis * render_settings.vigentte);
		ShaderSetFloat(&postprocessing_shader, "vignette_intensity", render_settings.vigentte_intensity * render_settings.vigentte);

		ShaderSetFloat(&postprocessing_shader, "exposure", render_settings.post_processing_exposure);
		ShaderSetInt32(&postprocessing_shader, "tonemapping_method", static_cast<uint32>(render_settings.tonemapping));
			   
		RenderMesh(postprocessing_shader, StandardMeshes::quad);
		
		EnableDepthBuffer();
				
#pragma endregion
		
	
		//************************************
		// Editor/Debug pass
		//************************************	
	
#if EDITOR_WINDOW

		if (current_context.selected_world_object)
		{
			if (current_context.current_widget)
			{
				BindShader(transform_widget_shader);

				ShaderSetMat4(&transform_widget_shader, "model", current_context.current_widget->GetTransform()->CalcTransformMatrix().ptr);
				RenderMesh(transform_widget_shader, current_context.current_widget->GetMeshForRender());
			}
		}

		UnbindFrameBuffer();

		FrameBufferCopyDepth(post_processing_buffer, &final_buffer);
			   
		BindFrameBuffer(final_buffer);

		BindShader(editor_light_shader);
		RenderMesh(editor_light_shader, StandardMeshes::Cube());		

		if (current_context.selected_world_object)
		{		

			
			GeometricCollider* collider = current_context.selected_world_object->GetCollider();
			if (collider)
			{
				switch (collider->GetColliderType())
				{
				case cm::GeometricColliderType::plane: {
					Plane *plane = reinterpret_cast<Plane*>(collider);
					Debug::AddIrresolutePlane(plane->origin, plane->normal);
					break;
				}
				case cm::GeometricColliderType::axis_aligned_bounding_box: {
					Aabb *aabb = reinterpret_cast<Aabb*>(collider);
					Debug::AddIrresoluteAABBMinMax(aabb->min, aabb->max);
					break;
				}
				case cm::GeometricColliderType::object_bounding_box: {
					OBB *obb = reinterpret_cast<OBB*>(collider);
					Debug::AddIrresoluteOBB(obb->origin, obb->GetExtents(), obb->basis);
					break;
				}
				default:
					break;
				}
			}
		}		
		
		Debug::DrawLines();
		
		UnbindFrameBuffer();

		// @NOTE: Setup stencil buffer such that we discard any pixels that were drawn by the editor
		StencilDiscardOnOne();
#endif

		// @NOTE: This disables the final buffer in the event we don't build with the editor
		UnbindFrameBuffer();

		BindShader(Debug::texture_shader);

		ShaderBindTexture(Debug::texture_shader, final_buffer.colour0_texture_attachment, 0, "src_texture");
		RenderMesh(Debug::texture_shader, StandardMeshes::quad);

#if EDITOR_WINDOW

		SetViewPort(0, 0, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);
		Debug::DrawTexture(ssr_frame.colour0_texture_attachment);		
		SetViewPort(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

#endif
		//************************************
		// Process update input state
		//************************************

		Input::Update();

		//************************************
		// Swap the buffer into the screen
		//************************************
		
		glfwSwapBuffers(window);
			   		 
		//************************************
		// Calculate the frame time
		//************************************
		
		std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::steady_clock::now();
		std::chrono::microseconds elapsedTime(std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime));
		
		real32 time = static_cast<real32> (elapsedTime.count());
		delta_time = time * 0.001f * 0.001f;		
		run_time += delta_time;
	}


	FreeEditor();
	glfwTerminate();
	return 0;
}