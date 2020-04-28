#include <glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include "src/Core.h"
#include "src/OpenGL/RenderCommands.h"
#include "src/Math/CosmicPhysics.h"
#include "src/Math/CosmicNoise.h"
#include "src/OpenGL/OpenGL.h"

#include "src/Core/EditableMesh.h"
#include "src/Core/Renderer.h"
#include "src/Core/Editor.h"
#include "Tutorials.h"
#include "src/Core/Debug.h"
#include "src/Engine/AssetLoader.h"
#include "src/Engine/Input.h"
#include "src/Core/Sandbox.h"
#include "src/Core/Serialization.h"
using namespace cm;


// @TODO: Make platform thingy. Check SSAO shader and Imgui frame they have hardcoded values
static const uint32 WINDOW_WIDTH = 1280;
static const uint32 WINDOW_HEIGHT = 720;
static const float MOUSE_SENSITIVITY = 0.08f;

static CameraController camera_controller = {};
static MassAggregateEngine physics_engine;
static GLFWwindow *window = nullptr;
static float delta_time = 0;

static bool DRAW_LIGHT_POSITIONS = false;
static bool move_camera = false;

// @TODO: Make a context strct
ControlWidget selected_widget_mode = ControlWidget::none;
WorldObject *selected_world_object = nullptr;

void MousePositionCallBack(GLFWwindow *widow, double xpos, double ypos)
{
	// @NOTE: GetLastMouse is now actually 2 frames behind. Thats why we use GetMouse. 
	Vec2 current_mouse = Vec2(static_cast<real>(xpos), static_cast<real>(ypos));
	Vec2 last_mouse = Input::GetMousePosition();

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

	//Shader qq = CreateShader(ReadFile("shaders/quad_vert.glsl"), ReadFile("shaders/quad_frag.glsl"));
	//qq.name = "quad";

	//Shader pbr_shader = CreateShader(ReadFile("shaders/pbr_vert.glsl"), ReadFile("shaders/pbr_frag.glsl"));
	//pbr_shader.name = "pbr_shader";

	//Shader pbr_texture_shader = CreateShader(ReadFile("shaders/pbr_vert.glsl"), ReadFile("shaders/pbr_texture_frag.glsl"));
	//pbr_texture_shader.name = "pbr_texture_shader";

	Shader cubemap_shader = CreateShader(ReadFile("shaders/skybox_vert.glsl"), ReadFile("shaders/skybox_frag.glsl"));
	cubemap_shader.name = "cubemap_shader";

	Shader debug_shader = CreateShader(ReadFile("shaders/debug_vert.glsl"), ReadFile("shaders/debug_frag.glsl"));
	debug_shader.name = "debug_shader";

	Shader debug_mesh_shader = CreateShader(ReadFile("shaders/debug_mesh_vert.glsl"), ReadFile("shaders/debug_mesh_frag.glsl"));
	debug_mesh_shader.name = "debug_quad_shader";
	
	Shader deferred_shader = CreateShader(ReadFile("shaders/deffered_shader_vert.glsl"), ReadFile("shaders/deffered_shader_frag.glsl"));
	deferred_shader.name = "deffered_shader";

	Shader ssao_gbuffer_shader = CreateShader(ReadFile("shaders/ssao_gbuffer_vert.glsl"), ReadFile("shaders/ssao_gbuffer_frag.glsl"));
	ssao_gbuffer_shader.name = "ssao_gbuffer_shader";



	Shader simple_blur_shader = CreateShader(ReadFile("shaders/simple_blur_vert.glsl"), ReadFile("shaders/simple_blur_frag.glsl"));
	simple_blur_shader.name = "simple_blur";

	//Shader instance_shader_test = CreateShader(ReadFile("shaders/instance_test_vert.glsl"), ReadFile("shaders/instance_test_frag.glsl"));
	//instance_shader_test.name = "instancing_test";
	
	Shader forward_phong_notext_shader = CreateShader(ReadFile("shaders/forward_phong_notext_vert.glsl"), ReadFile("shaders/forward_phong_notext_frag.glsl"));
	forward_phong_notext_shader.name = "forward_phong_notext_shader";

	Shader forward_pbr_notext_shader = CreateShader(ReadFile("shaders/forward_pbr_notext_vert.glsl"), ReadFile("shaders/forward_pbr_notext_frag.glsl"));
	forward_pbr_notext_shader.name = "forward_pbr_notext_shader";

	Shader simple_phong_shader = CreateShader(ReadFile("shaders/simple_phong_vert.glsl"), ReadFile("shaders/simple_phong_frag.glsl"));
	simple_phong_shader.name = "simple_phong_shader";

	Shader animation_test_shader = CreateShader(ReadFile("shaders/test_anim_vert.glsl"), ReadFile("shaders/forward_pbr_notext_frag.glsl"));
	animation_test_shader.name = "animation_test_shader";


	Shader post_processing_shader = CreateShader(ReadFile("shaders/post_processing_vert.glsl"), ReadFile("shaders/post_processing_frag.glsl"));
	post_processing_shader.name = "post_processing_shader";

	Shader simple_shadow_map_shader = CreateShader(ReadFile("shaders/simple_shadow_map_vert.glsl"), ReadFile("shaders/simple_shadow_map_frag.glsl"));
	simple_shadow_map_shader.name = "simple_shadow_map_vert";

	Shader gpu_gaussian_blur_shader = CreateShader(ReadFile("shaders/Compute/GPU_gaussian_blur_vert.glsl"), ReadFile("shaders/Compute/GPU_gaussian_blur_frag.glsl"));
	gpu_gaussian_blur_shader.name = "gaussian_blur";

	Shader g_buffer_shader = CreateShader(ReadFile("shaders/Compute/g_buffer_vert.glsl"), ReadFile("shaders/Compute/g_buffer_frag.glsl"));
	g_buffer_shader.name = "g_buffer_shader ";

	Shader sanitycheck_shader = CreateShader(ReadFile("shaders/sanitycheck_vert.glsl"), ReadFile("shaders/sanitycheck_frag.glsl"));
	sanitycheck_shader.name = "sanitycheck_shader ";

	Shader ssr_shader = CreateShader(ReadFile("shaders/ssr_vert.glsl"), ReadFile("shaders/ssr_frag.glsl"));
	ssr_shader.name = "ssr_shader ";




	//************************************
	// Load shaders
	//************************************

	Shader worldspace_gbuffer_shader;
	worldspace_gbuffer_shader.config.src_vert = ReadFile("shaders/demo_01/worldspace_gbuffer_vert.glsl");
	worldspace_gbuffer_shader.config.src_frag = ReadFile("shaders/demo_01/worldspace_gbuffer_frag.glsl");
	worldspace_gbuffer_shader.name = "demo_bot_gbuffer_shader";
	CreateShader(&worldspace_gbuffer_shader);

	Shader worldspace_gbuffer_anim_shader;
	worldspace_gbuffer_anim_shader.config.src_vert = ReadFile("shaders/demo_01/worldspace_gbuffer_anim_vert.glsl");
	worldspace_gbuffer_anim_shader.config.src_frag = ReadFile("shaders/demo_01/worldspace_gbuffer_frag.glsl");
	worldspace_gbuffer_anim_shader.name = "demo_bot_gbuffer_anim_shader";
	CreateShader(&worldspace_gbuffer_anim_shader);

	Shader viewspace_gbuffer_shader;
	viewspace_gbuffer_shader.config.src_vert = ReadFile("shaders/demo_01/viewspace_gbuffer_vert.glsl");
	viewspace_gbuffer_shader.config.src_frag = ReadFile("shaders/demo_01/viewspace_gbuffer_frag.glsl");
	viewspace_gbuffer_shader.name = "viewspace_gbuffer_shader";
	CreateShader(&viewspace_gbuffer_shader);

	Shader ssao_shader;
	ssao_shader.config.src_vert = ReadFile("shaders/demo_01/screenspace_ambient_occlusion_vert.glsl");
	ssao_shader.config.src_frag = ReadFile("shaders/demo_01/screenspace_ambient_occlusion_frag.glsl");
	ssao_shader.name = "screenspace_ambient_occlusion";
	CreateShader(&ssao_shader);

	Shader depth_shader;
	depth_shader.config.src_vert = ReadFile("shaders/demo_01/depth_vert.glsl");
	depth_shader.config.src_frag = ReadFile("shaders/demo_01/depth_frag.glsl");
	depth_shader.name = "depth shader";
	CreateShader(&depth_shader);

	Shader demo_01_deffered_pbr_shader;
	demo_01_deffered_pbr_shader.config.src_vert = ReadFile("shaders/demo_01/pbr_deffered_vert.glsl");
	demo_01_deffered_pbr_shader.config.src_frag = ReadFile("shaders/demo_01/pbr_deffered_frag.glsl");
	demo_01_deffered_pbr_shader.name = "demo_01_deffered_pbr_shader";
	CreateShader(&demo_01_deffered_pbr_shader);

	Shader demo_01_postprocessing_shader;
	demo_01_postprocessing_shader.config.src_vert = ReadFile("shaders/demo_01/postprocessing_vert.glsl");
	demo_01_postprocessing_shader.config.src_frag = ReadFile("shaders/demo_01/postprocessing_frag.glsl");
	demo_01_postprocessing_shader.name = "demo_01_postprocessing_shader";
	CreateShader(&demo_01_postprocessing_shader);
	
	Shader transform_widget_shader;
	transform_widget_shader.config.src_vert = ReadFile("res/engine/shaders/editor_widget_vert.glsl");
	transform_widget_shader.config.src_frag = ReadFile("res/engine/shaders/editor_widget_frag.glsl");
	CreateShader(&transform_widget_shader);

	//************************************
	// Load texture assets
	//************************************
	// @TODO: Free is FAR down the file
	TextureImport texture_importer;
	texture_importer.flip = false;
	texture_importer.texture_paths.push_back("res/textures/studio_small_03_2k.hdr");
	texture_importer.texture_paths.push_back("res/botdemo/FloorSet/Textures/Floor2/Floor_BaseFloor_BaseColor.png");
	texture_importer.texture_paths.push_back("res/botdemo/FloorSet/Textures/Floor2/Floor_BaseFloor_Normal.png");
	texture_importer.texture_paths.push_back("res/botdemo/FloorSet/Textures/Floor2/Floor_BaseFloor_OcclusionRoughnessMetallic.png");
	//texture_importer.texture_paths.push_back("res/textures/bot1_rig_v01_Scene_Material_BaseColor.png");
	//texture_importer.texture_paths.push_back("res/textures/bot1_rig_v01_Scene_Material_Normal.png");
	//texture_importer.texture_paths.push_back("res/textures/bot1_rig_v01_Scene_Material_OcclusionRoughnessMetallic.png");	

	texture_importer.Load();

	//************************************
	// Load mesh assets
	//************************************

	// @TODO: Remove
	ModelImport model_importer;
	model_importer.import_animations = true;
	model_importer.import_vertex_binorms_tangents = true;
	model_importer.model_paths.push_back("res/botdemo/FloorSet/Floor.fbx");
	model_importer.model_paths.push_back("res/models/sphere.obj");
	model_importer.Load();
	StandardMeshes::sphere = model_importer.resulting_meshes[1].CreateMesh(true);



	ModelImport editor_mesh_importer;
	editor_mesh_importer.import_animations = true;
	editor_mesh_importer.import_vertex_binorms_tangents = true;
	editor_mesh_importer.model_paths.push_back("res/engine/meshes/translation_widget.dae");
	editor_mesh_importer.model_paths.push_back("res/engine/meshes/rotation_widget.dae");
	editor_mesh_importer.Load();
	
	TranslationWidget translation_widget;
	translation_widget.Create(editor_mesh_importer.resulting_meshes[0].CreateMesh());

	RotationWidget rotation_widget;
	rotation_widget.Create(editor_mesh_importer.resulting_meshes[1].CreateMesh());
	
	editor_mesh_importer.Free();

	//************************************
	// Initialize imported textures
	//************************************
	
	Texture demo_floor_colour_map;
	demo_floor_colour_map.config = texture_importer.texture_configs[1];
	demo_floor_colour_map.config.mipmaps = true;
	CreateTexture(&demo_floor_colour_map, texture_importer.texture_data[1].data());

	Texture demo_floor_normal_map;
	demo_floor_normal_map.config = texture_importer.texture_configs[2];
	demo_floor_normal_map.config.mipmaps = true;
	CreateTexture(&demo_floor_normal_map, texture_importer.texture_data[2].data());

	Texture demo_floor_orm_map;
	demo_floor_orm_map.config = texture_importer.texture_configs[3];
	demo_floor_orm_map.config.mipmaps = true;
	CreateTexture(&demo_floor_orm_map, texture_importer.texture_data[3].data());

	//************************************
	// Initialize imported meshes 
	//************************************

	Actor floor_test;	
	floor_test.mesh = model_importer.resulting_meshes[0].CreateMesh(true);
	floor_test.material.forward_shader = &forward_pbr_notext_shader;
	floor_test.transform.scale = Vec3(0.05);
	floor_test.material.diffuse = Vec3(0.86);
	floor_test.material.metalness = 0.6;
	floor_test.material.roughness = 0;
		
	Actor left_wall;
	left_wall.mesh = floor_test.mesh;
	left_wall.material.forward_shader = &forward_pbr_notext_shader;
	left_wall.transform.scale = Vec3(0.05);
	left_wall.transform.rotation = EulerToQuat(Vec3(0, 0, 90));
	left_wall.transform.position = Vec3(-10, 10, 0);
	left_wall.material.diffuse = Vec3(0.2);
	left_wall.material.metalness = 0.2;
	left_wall.material.roughness = 1;

	Actor right_wall;
	right_wall.mesh = floor_test.mesh;
	right_wall.material.forward_shader = &forward_pbr_notext_shader;
	right_wall.transform.scale = Vec3(0.01);
	right_wall.transform.rotation = EulerToQuat(Vec3(0, 0, -90));
	right_wall.transform.position = Vec3(2, 2, 0);
	right_wall.material.diffuse = Vec3(0.2);
	right_wall.material.metalness = 0.2;
	right_wall.material.roughness = 1;

	Actor back_wall;
	back_wall.mesh = floor_test.mesh;
	back_wall.material.forward_shader = &forward_pbr_notext_shader;
	back_wall.transform.scale = Vec3(0.01);
	back_wall.transform.rotation = EulerToQuat(Vec3(-90, 0, 0));
	back_wall.transform.position = Vec3(0, 2, -2);
	back_wall.material.diffuse = Vec3(0.2);
	back_wall.material.metalness = 0.2;
	back_wall.material.roughness = 1;


	//************************************
	// Create the frame buffers
	//************************************

	FrameBuffer worldspace_gbuffer;

	worldspace_gbuffer.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
	worldspace_gbuffer.colour0_texture_attachment.config.width = WINDOW_WIDTH;
	worldspace_gbuffer.colour0_texture_attachment.config.data_type = GL_FLOAT;
	worldspace_gbuffer.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
	worldspace_gbuffer.colour0_texture_attachment.config.pixel_format = GL_RGBA;
	worldspace_gbuffer.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	worldspace_gbuffer.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	worldspace_gbuffer.colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	worldspace_gbuffer.colour1_texture_attachment.config.height = WINDOW_HEIGHT;
	worldspace_gbuffer.colour1_texture_attachment.config.width = WINDOW_WIDTH;
	worldspace_gbuffer.colour1_texture_attachment.config.data_type = GL_FLOAT;
	worldspace_gbuffer.colour1_texture_attachment.config.texture_format = GL_RGBA16F;
	worldspace_gbuffer.colour1_texture_attachment.config.pixel_format = GL_RGBA;
	worldspace_gbuffer.colour1_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	worldspace_gbuffer.colour1_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	worldspace_gbuffer.colour1_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	worldspace_gbuffer.colour2_texture_attachment.config.height = WINDOW_HEIGHT;
	worldspace_gbuffer.colour2_texture_attachment.config.width = WINDOW_WIDTH;
	worldspace_gbuffer.colour2_texture_attachment.config.data_type = GL_FLOAT;
	worldspace_gbuffer.colour2_texture_attachment.config.texture_format = GL_RGBA16F;
	worldspace_gbuffer.colour2_texture_attachment.config.pixel_format = GL_RGBA;
	worldspace_gbuffer.colour2_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	worldspace_gbuffer.colour2_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	worldspace_gbuffer.colour2_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	worldspace_gbuffer.depthstencil_attchment.width = WINDOW_WIDTH;
	worldspace_gbuffer.depthstencil_attchment.height = WINDOW_HEIGHT;

	CreateTexture(&worldspace_gbuffer.colour0_texture_attachment, nullptr);
	CreateTexture(&worldspace_gbuffer.colour1_texture_attachment, nullptr);
	CreateTexture(&worldspace_gbuffer.colour2_texture_attachment, nullptr);
	CreateDepthStencilBuffer(&worldspace_gbuffer.depthstencil_attchment);
	CreateFrameBuffer(&worldspace_gbuffer);

	BindFrameBuffer(worldspace_gbuffer);

	FrameBufferBindColourAttachtments(&worldspace_gbuffer);
	FrameBufferBindRenderAttachtment(&worldspace_gbuffer);

	Assert(CheckFrameBuffer(worldspace_gbuffer));
	UnbindFrameBuffer();


	FrameBuffer viewspace_gbuffer;

	viewspace_gbuffer.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
	viewspace_gbuffer.colour0_texture_attachment.config.width = WINDOW_WIDTH;
	viewspace_gbuffer.colour0_texture_attachment.config.data_type = GL_FLOAT;
	viewspace_gbuffer.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
	viewspace_gbuffer.colour0_texture_attachment.config.pixel_format = GL_RGBA;
	viewspace_gbuffer.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	viewspace_gbuffer.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	viewspace_gbuffer.colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	viewspace_gbuffer.colour1_texture_attachment.config.height = WINDOW_HEIGHT;
	viewspace_gbuffer.colour1_texture_attachment.config.width = WINDOW_WIDTH;
	viewspace_gbuffer.colour1_texture_attachment.config.data_type = GL_FLOAT;
	viewspace_gbuffer.colour1_texture_attachment.config.texture_format = GL_RGBA16F;
	viewspace_gbuffer.colour1_texture_attachment.config.pixel_format = GL_RGBA;
	viewspace_gbuffer.colour1_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	viewspace_gbuffer.colour1_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	viewspace_gbuffer.colour1_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	viewspace_gbuffer.colour2_texture_attachment.config.height = WINDOW_HEIGHT;
	viewspace_gbuffer.colour2_texture_attachment.config.width = WINDOW_WIDTH;
	viewspace_gbuffer.colour2_texture_attachment.config.data_type = GL_FLOAT;
	viewspace_gbuffer.colour2_texture_attachment.config.texture_format = GL_RGBA16F;
	viewspace_gbuffer.colour2_texture_attachment.config.pixel_format = GL_RGBA;
	viewspace_gbuffer.colour2_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	viewspace_gbuffer.colour2_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	viewspace_gbuffer.colour2_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	viewspace_gbuffer.depthstencil_attchment.width = WINDOW_WIDTH;
	viewspace_gbuffer.depthstencil_attchment.height = WINDOW_HEIGHT;

	CreateTexture(&viewspace_gbuffer.colour0_texture_attachment, nullptr);
	CreateTexture(&viewspace_gbuffer.colour1_texture_attachment, nullptr);
	CreateTexture(&viewspace_gbuffer.colour2_texture_attachment, nullptr);
	CreateDepthStencilBuffer(&viewspace_gbuffer.depthstencil_attchment);
	CreateFrameBuffer(&viewspace_gbuffer);

	BindFrameBuffer(viewspace_gbuffer);

	FrameBufferBindColourAttachtments(&viewspace_gbuffer);
	FrameBufferBindRenderAttachtment(&viewspace_gbuffer);

	Assert(CheckFrameBuffer(viewspace_gbuffer));
	UnbindFrameBuffer();

	FrameBuffer ssao_buffer;
	
	ssao_buffer.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
	ssao_buffer.colour0_texture_attachment.config.width = WINDOW_WIDTH;
	ssao_buffer.colour0_texture_attachment.config.data_type = GL_FLOAT;
	ssao_buffer.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
	ssao_buffer.colour1_texture_attachment.config.pixel_format = GL_RGBA;
	ssao_buffer.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	ssao_buffer.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	ssao_buffer.colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	CreateTexture(&ssao_buffer.colour0_texture_attachment, nullptr);
	CreateFrameBuffer(&ssao_buffer);
	
	BindFrameBuffer(ssao_buffer);

	FrameBufferBindColourAttachtments(&ssao_buffer);

	Assert(CheckFrameBuffer(ssao_buffer));
	UnbindFrameBuffer();

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

	FrameBuffer post_processing_buffer;

	post_processing_buffer.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
	post_processing_buffer.colour0_texture_attachment.config.width = WINDOW_WIDTH;
	post_processing_buffer.colour0_texture_attachment.config.data_type = GL_FLOAT;
	post_processing_buffer.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
	post_processing_buffer.colour1_texture_attachment.config.pixel_format = GL_RGBA;
	post_processing_buffer.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	post_processing_buffer.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	post_processing_buffer.colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	post_processing_buffer.depthstencil_attchment.width = WINDOW_WIDTH;
	post_processing_buffer.depthstencil_attchment.height = WINDOW_HEIGHT;

	CreateTexture(&post_processing_buffer.colour0_texture_attachment, nullptr);
	CreateDepthStencilBuffer(&post_processing_buffer.depthstencil_attchment);
	CreateFrameBuffer(&post_processing_buffer);

	BindFrameBuffer(post_processing_buffer);

	FrameBufferBindColourAttachtments(&post_processing_buffer);
	FrameBufferBindRenderAttachtment(&post_processing_buffer);
	
	Assert(CheckFrameBuffer(post_processing_buffer));
	UnbindFrameBuffer();
	

	ModelImport pbr_model;
	pbr_model.model_paths.push_back("res/models/claud_bot.obj");
	pbr_model.import_vertex_binorms_tangents = true;
	pbr_model.Load();

	Actor pbr_gun;
	pbr_gun.material.forward_shader = &forward_pbr_notext_shader;
	pbr_gun.mesh = pbr_model.resulting_meshes[0].CreateMesh(true);
	pbr_gun.transform.rotation = EulerToQuat(Vec3(0, 90, 0));
	pbr_gun.transform.position = Vec3(0, 1, 1);
	pbr_gun.transform.scale = Vec3(1);

	pbr_model.Free();


	ModelImport model_import;
	model_import.import_vertex_binorms_tangents = true;
	model_import.model_paths.push_back("res/models/Idle.dae");
	model_import.Load();
	
	AnimatedActor test_cube_guy;	
	EditableMesh emesh = model_import.resulting_meshes[0];
	test_cube_guy.mesh = emesh.CreateAnimMesh();
	test_cube_guy.animation_controller = model_import.resulting_animation_controllers[0];	
	test_cube_guy.transform.scale = Vec3(1);
	test_cube_guy.transform.rotation = EulerToQuat(Vec3(0, 0, 0));
	test_cube_guy.transform.position = Vec3(0, 0, 0);

	model_import.Free();



	FrameBuffer ssr_frame;

	ssr_frame.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
	ssr_frame.colour0_texture_attachment.config.pixel_format = GL_RGBA;
	ssr_frame.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	ssr_frame.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	ssr_frame.colour0_texture_attachment.config.min_filter = GL_LINEAR;
	ssr_frame.colour0_texture_attachment.config.mag_filter = GL_LINEAR;
	ssr_frame.colour0_texture_attachment.config.width = WINDOW_WIDTH;
	ssr_frame.colour0_texture_attachment.config.height = WINDOW_HEIGHT;

	CreateFrameBuffer(&ssr_frame);
	CreateTexture(&ssr_frame.colour0_texture_attachment, nullptr);
	
	FrameBufferBindColourAttachtments(&ssr_frame);

	Assert(CheckFrameBuffer(ssr_frame));

	uint32 drawing_width = post_processing_buffer.colour0_texture_attachment.config.width;
	uint32 drawing_height = post_processing_buffer.colour0_texture_attachment.config.height;

	GaussianTextureBlur gblur;
	gblur.Create(drawing_width, drawing_height, 11, 1.0/2.0, 1);
	
	LuminanceFilter lumin_filter;
	lumin_filter.Create(drawing_width, drawing_height, 1);

	HemisphereKernel hemisphere_kernel;
	hemisphere_kernel.Create(32, 4);

	SimpleTextureBlur sblur;
	sblur.Create(drawing_width, drawing_height, 2);


	Texture ssao_map;
	ssao_map.config = post_processing_buffer.colour0_texture_attachment.config;
	CreateTexture(&ssao_map, nullptr);

	Texture bloom_map;
	bloom_map.config = post_processing_buffer.colour0_texture_attachment.config;
	CreateTexture(&bloom_map, nullptr);

	//Shader testing_blur_shader;
	//testing_blur_shader.config.src_vert = ReadFile("shaders/downsampling_vert.glsl");
	//testing_blur_shader.config.src_frag = ReadFile("shaders/downsampling_frag.glsl");
	//CreateShader(&testing_blur_shader);
	//
	//SampleTextureBlur blurer;
	//blurer.shader = testing_blur_shader;
	//blurer.Create(post_processing.colour0_texture_attachment.config.width, post_processing.colour0_texture_attachment.config.height, 5);

	texture_importer.Free();

	Actor test_cube = CreateActorCube();
	World main_world;

	main_world.RegisterWorldObject(&test_cube);

	main_world.objects.push_back(&pbr_gun);
	main_world.objects.push_back(&floor_test);
	main_world.objects.push_back(&left_wall);
	main_world.objects.push_back(&right_wall);
	main_world.objects.push_back(&back_wall);

	SetViewPort(WINDOW_WIDTH, WINDOW_WIDTH);
	EnableFaceCulling();
	CullBackFace();
	EnableDepthBuffer();
	EnableCubeMapSeamless();
	DepthBufferFunction(GL_LEQUAL);
	
#if EDITOR_WINDOW
	EnableStencilBuffer();
	StencilDiscardOnOne();
	StencilOperationKKR();
#endif
	SetViewPort(WINDOW_WIDTH, WINDOW_HEIGHT);

	camera_controller.main_camera.projection_matrix = Perspective(40, ((float)WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 250.0f);
	camera_controller.main_camera.target = Vec3(0);
	camera_controller.main_camera.transform.position = Vec3(0, 4, 5);
	camera_controller.main_camera.view_matrix = LookAt(camera_controller.main_camera.transform.position, camera_controller.main_camera.target, Vec3(0, 1, 0));
	

	Informer informer;
	informer.CreateUBO("WorldMatrices", BUFFER_LAYOUT(ShaderDataType::Mat4, ShaderDataType::Mat4,  ShaderDataType::Mat4), 0);
	informer.CreateUBO("LightingData", BUFFER_LAYOUT(ShaderDataType::Float4, ShaderDataType::Float4, ShaderDataType::Float4,
		ShaderDataType::Mat4, ShaderDataType::Mat4, ShaderDataType::Mat4, ShaderDataType::Mat4, ShaderDataType::Mat4), 1);

	informer.LinkShader("WorldMatrices", debug_shader);
	informer.LinkShader("WorldMatrices", debug_mesh_shader);
	informer.LinkShader("WorldMatrices", forward_phong_notext_shader);
	informer.LinkShader("WorldMatrices", deferred_shader);	
	informer.LinkShader("WorldMatrices", g_buffer_shader);

	informer.LinkShader("WorldMatrices", worldspace_gbuffer_shader);
	informer.LinkShader("LightingData", demo_01_deffered_pbr_shader);


	informer.LinkShader("LightingData", deferred_shader);
	informer.LinkShader("LightingData", forward_phong_notext_shader);
	

	RenderSettings render_settings;
	
	EditorConsole console;
	console.Log("*****WELCOME TO A BAD RENDERER*****");
	console.Log("General Stats");
	console.Log("Vendor: " + opengl_stats.vendor);
	console.Log("Renderer: " + opengl_stats.renderer);
	console.Log("Version: " + opengl_stats.version);
	console.Log("Shader lang: " + opengl_stats.shading_lang);

	EditorRenderSettingsWindow editor_render_window;
	editor_render_window.delta_time = 0;
	editor_render_window.render_settings = &render_settings;


		
	DirectionalLight sun_light;
	sun_light.direction = Normalize(Vec3(2, -4, 1));
	sun_light.light_colour = Vec3(.5);
	
	PointLight point_light;
	point_light.light_colour = Vec3(23.47, 21.31, 20.79);
	point_light.light_position = Vec3(0, 2, -1);


	float fh = 1;
	//DebugAddPersistentPoint(Vec3(0.0));
	float z = 0.5;

	

	

	float run_time = 1;
	bool toggel = false;

	bool mouse_input_for_editor_window = false;
	selected_world_object = &test_cube_guy;
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
		// @NOTE: Unbind any shader that was set.
		UnbindShader();

		// @NOTE: Set the clear colour to black for stencil errors
		SetClearColour(Vec4(0, 0, 0, 1));

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

		EditorEndFrame();


		// @NOTE: Get any input that was for one of the editor windows	
		mouse_input_for_editor_window = *ImGui::GetIO().MouseDownOwned;
	
		// @NOTE: Disable writing to the stencil buffer
		StencilZeroMask();

		// @NOTE: Reset the clear colour to a noticable
		SetClearColour(Vec4(0, 1, 0, 1));
#else 
		// @NOTE: Clear all the buffers
		ClearAllBuffers();
#endif

		//************************************
		// Process Custom Events
		//************************************		

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_F5))
		{
			FreeShader(&cubemap_shader);
			cubemap_shader = CreateShader(ReadFile("shaders/skybox_vert.glsl"), ReadFile("shaders/skybox_frag.glsl"));

			FreeShader(&forward_pbr_notext_shader);
			forward_pbr_notext_shader = CreateShader(ReadFile("shaders/forward_pbr_notext_vert.glsl"), ReadFile("shaders/forward_pbr_notext_frag.glsl"));

			//FreeShader(&pbr_texture_shader);
			//pbr_texture_shader = CreateShader(ReadFile("shaders/pbr_vert.glsl"), ReadFile("shaders/pbr_texture_frag.glsl"));
			//ShaderBindUniformBuffer(pbr_texture_shader, 0, "Matrices");
			//ShaderBindUniformBuffer(pbr_texture_shader, 1, "Lighting");

			FreeShader(&post_processing_shader);
			post_processing_shader = CreateShader(ReadFile("shaders/post_processing_vert.glsl"), ReadFile("shaders/post_processing_frag.glsl"));
			post_processing_shader.name = "post_processing_shader";
		}
		if (!(mouse_input_for_editor_window)) 
		{
			Vec2 curr = Input::GetMousePosition();
			Vec2 last = Input::GetMouseLastPosition();
			Ray cam_ray = camera_controller.RayFromCamera(curr, Vec2(WINDOW_WIDTH, WINDOW_HEIGHT));

			if (Input::IsMouseJustDown(MOUSE_BUTTON_1))
			{
				// @NOTE: Do we hit a control widget
				bool using_widget = false;
				if (selected_world_object)
				{
					switch (selected_widget_mode)
					{
					case cm::ControlWidget::none:
						break;
					case cm::ControlWidget::translation: {
						Transform transform = *selected_world_object->GetTransform();
						using_widget = translation_widget.Select(cam_ray, transform);
						break;
					}
					case cm::ControlWidget::rotation: {
						Transform transform = *selected_world_object->GetTransform();
						using_widget = rotation_widget.Select(cam_ray, transform);
						break;
					}
					case cm::ControlWidget::scaling:
						break;
					default:
						break;
					}
				}
				if (!using_widget)
				{
					// @NOTE: Assume we miss everything
					selected_world_object = nullptr;
					for (int32 i = 0; i < main_world.objects.size(); i++)
					{
						WorldObject *current = main_world.objects[i];
						GeometricCollider *collider = current->GetCollider();
						if (collider == nullptr)
						{
							continue;
						}
						// TODO: Sorting 
						if (collider->CheckCollision(cam_ray))
						{
							selected_world_object = current;
							break;
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
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				fh += 0.4f * delta_time;
				//point_light.light_position.x += 4 * delta_time;
				if (selected_world_object)
				{
					switch (selected_widget_mode)
					{
					case cm::ControlWidget::none:
						break;
					case cm::ControlWidget::translation: {
						Transform *transform = selected_world_object->GetTransform();
						GeometricCollider *collider = selected_world_object->GetCollider();
						translation_widget.Update(cam_ray, transform);
						if (collider)
						{
							collider->Update(transform);
						}
						break;
					}
					case cm::ControlWidget::rotation: {
						Transform *transform = selected_world_object->GetTransform();
						GeometricCollider *collider = selected_world_object->GetCollider();
						rotation_widget.Update(cam_ray, transform);
						if (collider)
						{
							collider->Update(transform);
						}
						break;
					}
					case cm::ControlWidget::scaling:
						break;
					default:
						break;
					}					
				}
			}
			else if (GLFW_RELEASE == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
			{


			}
		}

		if (Input::GetKeyJustDown(GLFW_KEY_T))
		{			
			if (selected_world_object)
			{
				selected_widget_mode = selected_widget_mode == ControlWidget::translation ? ControlWidget::none : ControlWidget::translation;
				Transform transform = *selected_world_object->GetTransform();
				translation_widget.SetTransform(transform);
			}
		}

		if (Input::GetKeyJustDown(GLFW_KEY_R))
		{	
			if (selected_world_object)
			{
				selected_widget_mode = selected_widget_mode == ControlWidget::rotation ? ControlWidget::none : ControlWidget::rotation ;
				Transform transform = *selected_world_object->GetTransform();
				rotation_widget.SetTransform(transform);
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
		// Update GPU Buffers
		//************************************

		std::vector<Mat4> camera_data = { camera_controller.main_camera.projection_matrix, camera_controller.main_camera.view_matrix,
										Mat4(1)};

		std::vector<Vec4> lighting_data = {
		Vec4(1,  1, 0, 0), 
		Vec4(camera_controller.main_camera.transform.position, 0),
		Vec4(sun_light.direction, 0),
		Vec4(point_light.light_position, 0), Vec4(1), Vec4(1), Vec4(1),
		Vec4(point_light.light_colour, 0), Vec4(1), Vec4(1), Vec4(1)
		};

		informer.UpdateUBO("WorldMatrices", camera_data);
		informer.UpdateUBO("LightingData", lighting_data);

		std::vector<Mat4> mats;


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

		ShaderSetMat4(&depth_shader, "projection", camera_controller.main_camera.projection_matrix.arr);
		ShaderSetMat4(&depth_shader, "view", camera_controller.main_camera.view_matrix.arr);

		for (int32 i = 0; i < main_world.objects.size(); i++)
		{
			WorldObject *obj	= main_world.objects[i];
			Transform transform = *obj->GetTransform();
			Material mat		= *obj->GetMaterial();
			Mat4 transform_matrix = obj->GetTransformMatrix();

			ShaderSetMat4(&depth_shader, "model", transform_matrix.arr);

			RenderMesh(depth_shader, obj->GetMeshForRender());
		}
		//************************************
		// World gbuffer pass for static objects
		//************************************

		BindFrameBuffer(worldspace_gbuffer);
		BindShader(worldspace_gbuffer_shader);

		ClearColourBuffer();
		ClearDepthBuffer();		
		
		for (int32 i = 0; i < main_world.objects.size(); i++)
		{
			WorldObject *obj	= main_world.objects[i];
			Transform transform = *obj->GetTransform();
			Material mat		= *obj->GetMaterial();
			Mat4 transform_matrix = obj->GetTransformMatrix();
					   
			if (mat.diffuse_texture != nullptr)
			{
				ShaderBindTexture(worldspace_gbuffer_shader, demo_floor_colour_map, 0, "colour_map");
				ShaderSetVec3(&worldspace_gbuffer_shader, "colour_set", Vec3(1).arr);
			}
			else
			{
				ShaderBindTexture(worldspace_gbuffer_shader, StandardTextures::GetOneTexture(), 0, "colour_map");
				ShaderSetVec3(&worldspace_gbuffer_shader, "colour_set", mat.diffuse.arr);
			}

			if (mat.normal_texture != nullptr)
			{
				ShaderBindTexture(worldspace_gbuffer_shader, demo_floor_normal_map, 1, "normal_map");
				ShaderSetInt32(&worldspace_gbuffer_shader, "normal_set", 1);
			}
			else
			{
				ShaderSetInt32(&worldspace_gbuffer_shader, "normal_set", 0);
			}

			if (mat.occlusion_roughness_metallic != nullptr)
			{
				ShaderBindTexture(worldspace_gbuffer_shader, demo_floor_orm_map, 2, "orme_map");
				ShaderSetVec3(&worldspace_gbuffer_shader, "orm_set", Vec3(1).arr);
			}
			else
			{
				ShaderBindTexture(worldspace_gbuffer_shader, StandardTextures::GetOneTexture(), 2, "orme_map");
				ShaderSetVec3(&worldspace_gbuffer_shader, "orm_set", Vec3(1, mat.roughness, mat.metalness).arr);
			}

			ShaderSetMat4(&worldspace_gbuffer_shader, "model", transform_matrix.arr);
			RenderMesh(worldspace_gbuffer_shader, obj->GetMeshForRender());
		}

		//************************************
		// Gbuffer pass for animated objects
		//************************************
#if 1
		BindShader(worldspace_gbuffer_anim_shader);

		ShaderSetVec3(&worldspace_gbuffer_anim_shader, "colour_set", Vec3(0.7).arr);
		ShaderSetInt32(&worldspace_gbuffer_anim_shader, "normal_set", 0);
		ShaderSetVec3(&worldspace_gbuffer_anim_shader, "orm_set", Vec3(1, 0.5, 0.1).arr);

		test_cube_guy.animation_controller.Play(0);
		test_cube_guy.animation_controller.Update(delta_time);

		for (int32 i = 1; i < test_cube_guy.animation_controller.bones.size(); i++)
		{
			std::stringstream ss;
			ss << "gBones[" << (i - 1) << "]";
			ShaderSetMat4(&worldspace_gbuffer_anim_shader, ss.str(), test_cube_guy.animation_controller.bones.at(i).current_transform.arr);
		}
		ShaderSetMat4(&worldspace_gbuffer_anim_shader, "model", test_cube_guy.transform.CalcTransformMatrix().arr);
		
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
			   
		for (int32 i = 0; i < main_world.objects.size(); i++)
		{
			WorldObject *obj = main_world.objects[i];
			Transform transform = *obj->GetTransform();
			Material mat = *obj->GetMaterial();
			Mat4 transform_matrix = obj->GetTransformMatrix();

			if (mat.diffuse_texture != nullptr)
			{
				ShaderBindTexture(viewspace_gbuffer_shader, demo_floor_colour_map, 0, "colour_map");
				ShaderSetVec3(&viewspace_gbuffer_shader, "colour_set", Vec3(1).arr);
			}
			else
			{
				ShaderBindTexture(viewspace_gbuffer_shader, StandardTextures::GetOneTexture(), 0, "colour_map");
				ShaderSetVec3(&viewspace_gbuffer_shader, "colour_set", mat.diffuse.arr);
			}

			if (mat.normal_texture != nullptr)
			{
				ShaderBindTexture(viewspace_gbuffer_shader, demo_floor_normal_map, 1, "normal_map");
				ShaderSetInt32(&viewspace_gbuffer_shader, "normal_set", 1);
			}
			else
			{
				ShaderSetInt32(&viewspace_gbuffer_shader, "normal_set", 0);
			}
			
			if (mat.occlusion_roughness_metallic != nullptr)
			{
				ShaderBindTexture(viewspace_gbuffer_shader, demo_floor_orm_map, 2, "orme_map");
				ShaderSetVec3(&viewspace_gbuffer_shader, "orm_set", Vec3(1).arr);
			}
			else
			{
				ShaderBindTexture(viewspace_gbuffer_shader, StandardTextures::GetOneTexture(), 2, "orme_map");
				ShaderSetVec3(&viewspace_gbuffer_shader, "orm_set", Vec3(1, mat.roughness, mat.metalness).arr);			
			}


			ShaderSetMat4(&viewspace_gbuffer_shader, "model", transform_matrix.arr);
			RenderMesh(viewspace_gbuffer_shader, obj->GetMeshForRender());
		}

		UnbindFrameBuffer();

		//************************************
		// Viewspace gbuffer pass for animated objects
		//************************************
		
		//************************************
		// Screen space reflections (SSR)
		//************************************

		// @NOTE: Not working currently, in development, ie still learning cause I suck
#if 0
		BindFrameBuffer(ssr_frame);
		BindShader(ssr_shader);

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();

		ShaderBindTexture(ssr_shader, viewspace_gbuffer.colour0_texture_attachment, 0, "view_position_map");
		ShaderBindTexture(ssr_shader, viewspace_gbuffer.colour1_texture_attachment, 1, "view_normal_map");
		ShaderBindTexture(ssr_shader, viewspace_gbuffer.colour2_texture_attachment, 2, "view_colour_map");
		ShaderSetMat4(&ssr_shader, "proj", camera_controller.main_camera.projection_matrix.arr);
		RenderMesh(ssr_shader, StandardMeshes::quad);

		UnbindFrameBuffer();
#endif

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
				ShaderSetMat4(&ssao_shader, "projection", camera_controller.main_camera.projection_matrix.arr);
				
				ShaderSetFloat(&ssao_shader, "strength", render_settings.ssao_strength);
				ShaderSetFloat(&ssao_shader, "radius", render_settings.ssao_radius);
				ShaderSetFloat(&ssao_shader, "bias", render_settings.ssao_bias);
				
				hemisphere_kernel.SetKernelSize(render_settings.ssao_kernel_size);
				
				for (uint32 i = 0; i < hemisphere_kernel.GetKernelSize(); i++)
				{
					ShaderSetVec3(&ssao_shader, "samples[" + std::to_string(i) + "]", hemisphere_kernel.GetKernelSample(i).arr);
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

		BindShader(demo_01_deffered_pbr_shader);

		ShaderBindTexture(demo_01_deffered_pbr_shader, worldspace_gbuffer.colour0_texture_attachment, 0, "g_position");
		ShaderBindTexture(demo_01_deffered_pbr_shader, worldspace_gbuffer.colour1_texture_attachment, 1, "g_normal");
		ShaderBindTexture(demo_01_deffered_pbr_shader, worldspace_gbuffer.colour2_texture_attachment, 2, "g_colour");
		ShaderBindTexture(demo_01_deffered_pbr_shader, render_settings.ssao ? ssao_map : StandardTextures::GetOneTexture(), 3, "g_ssao");
		RenderMesh(demo_01_deffered_pbr_shader, StandardMeshes::quad);

		UnbindFrameBuffer();

		//************************************
		// Forward pass
		//************************************
		

		//************************************
		// Post processing pass
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

#if EDITOR_WINDOW
		// @NOTE: Setup stencil buffer such that we discard any pixels that were drawn by the editor
		StencilDiscardOnOne();
#endif
		DisableDepthBuffer();
		BindShader(demo_01_postprocessing_shader);

		ShaderBindTexture(demo_01_postprocessing_shader, post_processing_buffer.colour0_texture_attachment, 0, "scene_texture");
		ShaderBindTexture(demo_01_postprocessing_shader, bloom_map, 1, "bloom_texture");

		ShaderSetInt32(&demo_01_postprocessing_shader, "FXAA", render_settings.fxaa);
		ShaderSetFloat(&demo_01_postprocessing_shader, "FXAA_SPAN_MAX", render_settings.fxaa_span_max);
		ShaderSetFloat(&demo_01_postprocessing_shader, "FXAA_DIR_MIN", render_settings.fxaa_dir_min);
		ShaderSetFloat(&demo_01_postprocessing_shader, "FXAA_DIR_REDUC", render_settings.fxaa_dir_reduc);
		
		ShaderSetInt32(&demo_01_postprocessing_shader, "bloom", render_settings.bloom);

		ShaderSetFloat(&demo_01_postprocessing_shader, "vigentte_outer_radius", render_settings.vigentte_outer_raduis * render_settings.vigentte);
		ShaderSetFloat(&demo_01_postprocessing_shader, "vigentte_inner_radius", render_settings.vigentte_inner_raduis * render_settings.vigentte);
		ShaderSetFloat(&demo_01_postprocessing_shader, "vignette_intensity", render_settings.vigentte_intensity * render_settings.vigentte);

		ShaderSetFloat(&demo_01_postprocessing_shader, "exposure", render_settings.post_processing_exposure);
		ShaderSetInt32(&demo_01_postprocessing_shader, "tonemapping_method", static_cast<uint32>(render_settings.tonemapping));
			   
		RenderMesh(demo_01_postprocessing_shader, StandardMeshes::quad);
		
		EnableDepthBuffer();

		//************************************
		// DBUGING pass
		//************************************	
	
		// @NOTE: Because we disable the depth buffer when drawing the screen space quad. We can now draw anything with depth on top of it
		//		: for debug reasons

		Debug::AddIrresoluteOBB(test_cube.obb.origin, test_cube.obb.extents, test_cube.obb.basis);

		BindShader(transform_widget_shader);
		if (selected_world_object)
		{			
			switch (selected_widget_mode)
			{
			case cm::ControlWidget::none:
				break;
			case cm::ControlWidget::translation: {
				ShaderSetMat4(&transform_widget_shader, "model", translation_widget.transform.CalcTransformMatrix().arr);
				RenderMesh(transform_widget_shader, translation_widget.GetMeshForRender());
				break;
			}
			case cm::ControlWidget::rotation: {
				ShaderSetMat4(&transform_widget_shader, "model", rotation_widget.transform.CalcTransformMatrix().arr);
				RenderMesh(transform_widget_shader, rotation_widget.GetMeshForRender());
				break;
			}
			case cm::ControlWidget::scaling:
				break;
			default:
				break;
			}
		}




		Debug::Draw(&debug_shader);
		
		////////////////////
		// Post Debug Drawing
		////////////////////
		
		if (toggel)
		{
			//DebugDrawTexture(&debug_mesh_shader, brdf_lookup_texture);
			//DebugDrawTexture(&debug_mesh_shader, map_to_eqi);
			//DebugDrawTexture(&debug_mesh_shader, demo_gbuffer.colour1_texture_attachment);
			//DebugDrawTexture(&debug_mesh_shader, blurer.sampling_textures[0]);
			//DebugDrawTexture(&debug_mesh_shader, testing_blur_down01);
			//DebugDrawTexture(&debug_mesh_shader, worldspace_gbuffer.colour2_texture_attachment);
		
			//Debug::DrawTexture(&debug_mesh_shader, bloom_map);
			//Debug::DrawTexture(&debug_mesh_shader, depth_buffer.colour0_texture_attachment);
			//DebugDrawTexture(&debug_mesh_shader, test_renderer.frame_ssao_blured.colour0_texture_attachment);
			//DebugDrawTexture(&debug_mesh_shader, ssao_buffer.colour0_texture_attachment); 
			//DebugDrawTexture(&debug_mesh_shader, gblur.vertical_frame.colour0_texture_attachment);
		}
		else
		{
			//DebugDrawTexture(&debug_mesh_shader, renderer.frame_g_buffer.colour1_texture_attachment);
		}


		//BindShader(debug_mesh_shader);
		glViewport(0, 0, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);
		Debug::DrawTexture(&debug_mesh_shader, viewspace_gbuffer.colour1_texture_attachment);
		//ShaderBindTexture(debug_mesh_shader, ssr_frame.colour0_texture_attachment, 0, "mesh_texture");
		//ShaderSetMat4(&debug_mesh_shader, "model", Mat4(1).arr);
		//RenderMesh(debug_mesh_shader, StandardMeshes::quad);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		//// @TODO: Debug Draw Quad
		//// @NOTE: Draw bloom buffer;
		//glViewport(0, 0, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
		//ShaderSetFloat(post_processing_shader, "exposure", 1.0f);
		//ShaderBindTexture(post_processing_shader, *bloom_blur.texture_to_blur, 0, "scene_texture");
		//RenderMesh(post_processing_shader, standard_meshes.quad);
		//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); 


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
		
		float time = static_cast<float> (elapsedTime.count());
		delta_time = time * 0.001f * 0.001f;		

		run_time += delta_time;

	}


	FreeEditor();
	glfwTerminate();
	return 0;
}