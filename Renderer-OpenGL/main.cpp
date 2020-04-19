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
#include "src/Core/UIRenderer.h"
#include "Tutorials.h"
#include "src/Debug.h"
#include "src/Engine/AssetLoader.h"
#include "src/Engine/Input.h"
#include "src/Core/Sandbox.h"
#include "src/Core/Serialization.h"
using namespace cm;



static const uint32 WINDOW_WIDTH = 1280;
static const uint32 WINDOW_HEIGHT = 720;
static const float MOUSE_SENSITIVITY = 0.08f;

static CameraController camera_controller = {};
static MassAggregateEngine physics_engine;
static GLFWwindow *window = nullptr;
static float delta_time = 0;


static bool DRAW_LIGHT_POSITIONS = false;
static bool move_camera = false;

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
	return window;
}

void InitializeStandardMeshes()
{
	EditableMesh plane;
	// CCW
	plane.AddTrianlge(Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, 1, 0));
	plane.SetColour(Vec3(0.7, 0.1, 0.1));
	plane.AddTrianlge(Vec3(0, 1, 0), Vec3(1, 0, 0), Vec3(1, 1, 0));
	plane.SetColour(Vec3(0.1, 0.7, 0.1));
	plane.RecaluclateNormals();
	//plane.FuseVertices();
	StandardMeshes::plane = plane.CreateMesh(false);


	
	

	EditableMesh quad; 
	// CCW
	quad.AddTrianlge(Vec3(-1, -1, 0), Vec3(1, -1, 0), Vec3(-1, 1, 0));
	quad.AddTextureCoords(Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, 1, 0));
	quad.AddTrianlge(Vec3(-1, 1, 0), Vec3(1, -1, 0), Vec3(1, 1, 0));
	quad.AddTextureCoords(Vec3(0, 1, 0), Vec3(1, 0, 0), Vec3(1, 1, 0));	
	// CW
	//quad.AddTrianlge(Vec3(-1, -1, 0), Vec3(-1, 1, 0), Vec3(1, -1, 0));
	//quad.AddTextureCoords(Vec3(0, 0, 0), Vec3(0, 1, 0), Vec3(01, 0, 0));
	//quad.AddTrianlge(Vec3(-1, 1, 0), Vec3(1, 1, 0), Vec3(1, -1, 0));
	//quad.AddTextureCoords(Vec3(0, 1, 0), Vec3(1, 1, 0), Vec3(1, 0, 0));
	quad.RecaluclateNormals();
	StandardMeshes::quad = quad.CreateMesh(false);
}

int main()
{
	window = CreateRenderingWindow();
	InitializeOpenGl(WINDOW_WIDTH, WINDOW_HEIGHT);
	InitializeDebug();
	InitializeStandardMeshes();

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

	Shader ssao_shader = CreateShader(ReadFile("shaders/ssao_vert.glsl"), ReadFile("shaders/ssao_frag.glsl"));
	ssao_shader.name = "ssao_vert";

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

	ModeImport model_importer;
	model_importer.import_animations = true;
	model_importer.import_vertex_binorms_tangents = true;
	model_importer.model_paths.push_back("res/botdemo/FloorSet/Floor.fbx");
	model_importer.Load();

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
	floor_test.transform.scale = Vec3(0.01);

	Actor left_wall;
	left_wall.mesh = floor_test.mesh;
	left_wall.material.forward_shader = &forward_pbr_notext_shader;
	left_wall.transform.scale = Vec3(0.01);
	left_wall.transform.rotation = EulerToQuat(Vec3(0, 0, 90));
	left_wall.transform.position = Vec3(-2, 2, 0);

	Actor right_wall;
	right_wall.mesh = floor_test.mesh;
	right_wall.material.forward_shader = &forward_pbr_notext_shader;
	right_wall.transform.scale = Vec3(0.01);
	right_wall.transform.rotation = EulerToQuat(Vec3(0, 0, -90));
	right_wall.transform.position = Vec3(2, 2, 0);

	Actor back_wall;
	back_wall.mesh = floor_test.mesh;
	back_wall.material.forward_shader = &forward_pbr_notext_shader;
	back_wall.transform.scale = Vec3(0.01);
	back_wall.transform.rotation = EulerToQuat(Vec3(-90, 0, 0));
	back_wall.transform.position = Vec3(0, 2, -2);

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

	worldspace_gbuffer.render_attchment.width = WINDOW_WIDTH;
	worldspace_gbuffer.render_attchment.height = WINDOW_HEIGHT;

	CreateTexture(&worldspace_gbuffer.colour0_texture_attachment, nullptr);
	CreateTexture(&worldspace_gbuffer.colour1_texture_attachment, nullptr);
	CreateTexture(&worldspace_gbuffer.colour2_texture_attachment, nullptr);
	CreateFrameBuffer(&worldspace_gbuffer);

	BindFrameBuffer(worldspace_gbuffer);

	FrameBufferBindColourAttachtments(&worldspace_gbuffer);
	FrameAddBufferRenderAttachtment(&worldspace_gbuffer);

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

	viewspace_gbuffer.render_attchment.width = WINDOW_WIDTH;
	viewspace_gbuffer.render_attchment.height = WINDOW_HEIGHT;

	CreateTexture(&viewspace_gbuffer.colour0_texture_attachment, nullptr);
	CreateTexture(&viewspace_gbuffer.colour1_texture_attachment, nullptr);
	CreateTexture(&viewspace_gbuffer.colour2_texture_attachment, nullptr);
	CreateFrameBuffer(&viewspace_gbuffer);

	BindFrameBuffer(viewspace_gbuffer);

	FrameBufferBindColourAttachtments(&viewspace_gbuffer);
	FrameAddBufferRenderAttachtment(&viewspace_gbuffer);

	Assert(CheckFrameBuffer(viewspace_gbuffer));
	UnbindFrameBuffer();

	FrameBuffer post_processing;

	post_processing.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
	post_processing.colour0_texture_attachment.config.width = WINDOW_WIDTH;
	post_processing.colour0_texture_attachment.config.data_type = GL_FLOAT;
	post_processing.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
	post_processing.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	post_processing.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	post_processing.colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	post_processing.render_attchment.width = WINDOW_WIDTH;
	post_processing.render_attchment.height = WINDOW_HEIGHT;

	CreateTexture(&post_processing.colour0_texture_attachment, nullptr);
	CreateFrameBuffer(&post_processing);

	BindFrameBuffer(post_processing);

	FrameBufferBindColourAttachtments(&post_processing);
	FrameAddBufferRenderAttachtment(&post_processing);
	
	Assert(CheckFrameBuffer(post_processing));
	UnbindFrameBuffer();

	//************************************
	// Initialize pbr IBL 
	//************************************
	
	Texture hdri;
	hdri.config = texture_importer.texture_configs[0];
	CreateTexture(&hdri, texture_importer.texture_data[0].data());

	Texture brdf_lookup_texture;
	brdf_lookup_texture.config.texture_format = GL_RG32F;
	brdf_lookup_texture.config.pixel_format = GL_RG;
	brdf_lookup_texture.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	brdf_lookup_texture.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	brdf_lookup_texture.config.min_filter = GL_LINEAR;
	brdf_lookup_texture.config.mag_filter = GL_LINEAR;
	brdf_lookup_texture.config.width = 512;
	brdf_lookup_texture.config.height= 512;
	CreateTexture(&brdf_lookup_texture, nullptr);

	CubeMap enviroment_map;
	enviroment_map.config.texture_format = GL_RGB32F;
	enviroment_map.config.pixel_format = GL_RGB;
	enviroment_map.config.wrap_r_mode = GL_CLAMP_TO_EDGE;
	enviroment_map.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	enviroment_map.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	enviroment_map.config.min_filter = GL_LINEAR_MIPMAP_LINEAR;
	enviroment_map.config.mipmaps = true;
	enviroment_map.config.width	 = 512;
	enviroment_map.config.height = 512;
	CreateCubeMap(&enviroment_map, nullptr);

	CubeMap irradiance_map;
	irradiance_map.config.texture_format = GL_RGB32F;
	irradiance_map.config.pixel_format = GL_RGB;
	irradiance_map.config.wrap_r_mode = GL_CLAMP_TO_EDGE;
	irradiance_map.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	irradiance_map.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	irradiance_map.config.width = 32;
	irradiance_map.config.height = 32;
	CreateCubeMap(&irradiance_map, nullptr);

	CubeMap prefilter_map;
	prefilter_map.config.texture_format = GL_RGB32F;
	prefilter_map.config.pixel_format = GL_RGB;
	prefilter_map.config.width	= 128;
	prefilter_map.config.height = 128;
	prefilter_map.config.mipmaps = true;
	prefilter_map.config.min_filter = GL_LINEAR_MIPMAP_LINEAR;
	prefilter_map.config.mag_filter = GL_LINEAR;
	CreateCubeMap(&prefilter_map, nullptr);
	
	CubeMapGenerator eqi_to_map;
	eqi_to_map.Create();
	eqi_to_map.Convert(hdri, &enviroment_map);
	eqi_to_map.Free();	
	
	Texture map_to_eqi;
	map_to_eqi.config = hdri.config;
	CreateTexture(&map_to_eqi, nullptr);

	EquirectangularGenerator cube_to_eqi;
	cube_to_eqi.Create();
	cube_to_eqi.Convert(enviroment_map, &map_to_eqi);
	cube_to_eqi.Free();

	IrradianceGenerator irradiance_calc;
	irradiance_calc.Create();
	irradiance_calc.Convert(enviroment_map, &irradiance_map);
	irradiance_calc.Free();
	
	PrefilterGenerator gen_prefilter;
	gen_prefilter.Create();
	gen_prefilter.Convert(enviroment_map, &prefilter_map);
	gen_prefilter.Free();

	LookUpTextureGenerator brdf_lookup;
	brdf_lookup.Create();	
	brdf_lookup.Convert(&brdf_lookup_texture);
	brdf_lookup.Free();

	

	ModeImport pbr_model;
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


	ModeImport model_import;
	model_import.import_vertex_binorms_tangents = true;
	model_import.model_paths.push_back("res/models/man.dae");
	model_import.Load();
	
	AnimatedActor test_cube_guy;	
	EditableMesh emesh = model_import.resulting_meshes[0];
	test_cube_guy.mesh = emesh.CreateAnimMesh();
	test_cube_guy.animation_controller = model_import.resulting_animation_controllers[0];	
	test_cube_guy.transform.scale = Vec3(.5);
	test_cube_guy.transform.rotation = EulerToQuat(Vec3(0, 0, 0));
	test_cube_guy.transform.position = Vec3(0, 0, 0);

	model_import.Free();



	FrameBuffer ssr_frame;
	Texture srr_ctexture;

	srr_ctexture.config.texture_format = GL_RGBA16F;
	srr_ctexture.config.pixel_format = GL_RGBA;
	srr_ctexture.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	srr_ctexture.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	srr_ctexture.config.min_filter = GL_LINEAR;
	srr_ctexture.config.mag_filter = GL_LINEAR;
	srr_ctexture.config.width = WINDOW_WIDTH;
	srr_ctexture.config.height = WINDOW_HEIGHT;

	CreateFrameBuffer(&ssr_frame);
	CreateTexture(&srr_ctexture, nullptr);

	ssr_frame.colour0_texture_attachment = srr_ctexture;

	FrameBufferBindColourAttachtments(&ssr_frame);

	Assert(CheckFrameBuffer(ssr_frame));

	
	GaussianTextureBlur gblur;
	gblur.Create(post_processing.colour0_texture_attachment.config.width, post_processing.colour0_texture_attachment.config.height, 11, 1.0/2.0);
	
	LuminanceFilter lumin_filter;
	lumin_filter.Create(1);

	Texture luminance_texture;
	luminance_texture.config = post_processing.colour0_texture_attachment.config;
	CreateTexture(&luminance_texture, nullptr);

	//Shader testing_blur_shader;
	//testing_blur_shader.config.src_vert = ReadFile("shaders/downsampling_vert.glsl");
	//testing_blur_shader.config.src_frag = ReadFile("shaders/downsampling_frag.glsl");
	//CreateShader(&testing_blur_shader);
	//
	//SampleTextureBlur blurer;
	//blurer.shader = testing_blur_shader;
	//blurer.Create(post_processing.colour0_texture_attachment.config.width, post_processing.colour0_texture_attachment.config.height, 5);

	texture_importer.Free();

	World main_world;

	main_world.objects.push_back(&pbr_gun);
	main_world.objects.push_back(&floor_test);
	main_world.objects.push_back(&left_wall);
	main_world.objects.push_back(&right_wall);
	main_world.objects.push_back(&back_wall);

	RenderCommands::ChangeViewPort(WINDOW_WIDTH, WINDOW_WIDTH);
	RenderCommands::EnableFaceCulling();
	RenderCommands::CullBackFace();
	RenderCommands::EnableDepthBuffer();
	RenderCommands::EnableCubeMapSeamless();
	RenderCommands::DepthBufferFunction(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
	RenderCommands::ChangeViewPort(WINDOW_WIDTH, WINDOW_HEIGHT);

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
	
	UIRenderer ui_renderer;
	ui_renderer.Init(window);

	EditorConsole console;
	console.Log("*****WELCOME TO A BAD RENDERER*****");
	console.Log("General Stats");
	console.Log("Vendor: " + opengl_stats.vendor);
	console.Log("Renderer: " + opengl_stats.renderer);
	console.Log("Version: " + opengl_stats.version);
	console.Log("Shader lang: " + opengl_stats.shading_lang);

	RenderSettings render_settings;

	EditorRender editor_render_window;
	editor_render_window.delta_time = 0;
	editor_render_window.render_settings = &render_settings;
	

	DirectionalLight sun_light;
	sun_light.direction = Normalize(Vec3(2, -4, 1));
	sun_light.light_colour = Vec3(.5);


	PointLight point_light;
	point_light.light_colour = Vec3(23.47, 21.31, 20.79);
	point_light.light_position = Vec3(0, 2, -1);


	float fh = 1;
	DebugAddPersistentPoint(Vec3(0.0));
	float z = 0.5;
	
	
	float run_time = 1;
	bool toggel = false;
	while (!glfwWindowShouldClose(window))
	{
		//************************************
		// Process Windows Events
		//************************************
		std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();
		glfwPollEvents();
		

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
		
		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);					
			fh += 0.4 * delta_time;
			//point_light.light_position.x += 4 * delta_time;
			toggel = true;
		}
		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
		{
			
			//point_light.light_position.x -= 4 * delta_time;

			fh -= 0.4 * delta_time;
			
		}
		else if (GLFW_RELEASE == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
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
		

		
		static Vec3 distance_to_dir = Vec3(-2.0f, 4.0f, -1.0f) - camera_controller.main_camera.transform.position;
		static Vec3 distance_to_ori = Vec3(0) - Vec3(-2.0f, 4.0f, -1.0f);
		static Vec3 target = -1 * Vec3(-2.0f, 4.0f, -1.0f);
		Vec3 e_pos = camera_controller.main_camera.transform.position + distance_to_dir;
		Vec3 o_pos = e_pos + distance_to_ori;
		float near_plane = 1.0f, far_plane = 10.f;
		float rect = 10;
		Vec3 cpos = camera_controller.main_camera.transform.position;
		Mat4 lightProjection = Orthographic(-rect, rect, rect, -rect, near_plane, far_plane);
		//Mat4 light_view = LookAt(cpos, cpos + Normalize(target), Vec3(0, 1, 0));
		Mat4 light_view = LookAt(Vec3(-2.0f, 4.0f, -1.0f), Vec3(0), Vec3(0, 1, 0));
		Mat4 light_space_matrix = light_view * lightProjection;


		/////////////////////////////////////////////////
		//light_space_matrix = light_view * shadow_projections[0];

		//************************************
		// Update GPU Buffers
		//************************************

		std::vector<Mat4> camera_data = { camera_controller.main_camera.projection_matrix, camera_controller.main_camera.view_matrix,
										light_space_matrix };

		std::vector<Vec4> lighting_data = {
		Vec4(1,  1, 0, 0),  // Meta data for lighting, number of lights 
		Vec4(camera_controller.main_camera.transform.position, 0),
		Vec4(sun_light.direction, 0),
		Vec4(point_light.light_position, 0), Vec4(1), Vec4(1), Vec4(1),
		Vec4(point_light.light_colour, 0), Vec4(1), Vec4(1), Vec4(1)
		};

		informer.UpdateUBO("WorldMatrices", camera_data);
		informer.UpdateUBO("LightingData", lighting_data);

		std::vector<Mat4> mats;

		DebugAddIrresolutePoint(Vec3(0, 5, 2));

		//************************************
		// Render The Current Frame
		//************************************

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
		RenderCommands::Clear(Colour(0, 1, 0, 1));
		

		//************************************
		// World gbuffer pass for static objects
		//************************************

		BindFrameBuffer(worldspace_gbuffer);
		BindShader(worldspace_gbuffer_shader);

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();		
		

		ShaderBindTexture(worldspace_gbuffer_shader, demo_floor_colour_map, 0, "colour_map");
		ShaderBindTexture(worldspace_gbuffer_shader, demo_floor_normal_map, 1, "normal_map");
		ShaderBindTexture(worldspace_gbuffer_shader, demo_floor_orm_map, 2, "orme_map");

		for (int32 i = 0; i < main_world.objects.size(); i++)
		{
			WorldObject *obj = main_world.objects[i];
			Transform transform = obj->GetTransfrom();
			Material mat = obj->GetMaterial();
			Mat4 transform_matrix = obj->GetTransformMatrix();

			ShaderSetMat4(&worldspace_gbuffer_shader, "model", transform_matrix.arr);
			RenderMesh(worldspace_gbuffer_shader, obj->GetMeshForRender());
		}

		//************************************
		// Gbuffer pass for animated objects
		//************************************
#if 1
		BindShader(worldspace_gbuffer_anim_shader);

		ShaderBindTexture(worldspace_gbuffer_anim_shader, demo_floor_colour_map, 0, "colour_map");
		ShaderBindTexture(worldspace_gbuffer_anim_shader, demo_floor_normal_map, 1, "normal_map");
		ShaderBindTexture(worldspace_gbuffer_anim_shader, demo_floor_orm_map, 2, "orme_map");

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

		ShaderBindTexture(viewspace_gbuffer_shader, demo_floor_colour_map, 0, "colour_map");
		//ShaderBindTexture(viewspace_gbuffer_shader, demo_floor_normal_map, 1, "normal_map");
		ShaderBindTexture(viewspace_gbuffer_shader, demo_floor_orm_map, 2, "orme_map");

		for (int32 i = 0; i < main_world.objects.size(); i++)
		{
			WorldObject *obj = main_world.objects[i];
			Transform transform = obj->GetTransfrom();
			Material mat = obj->GetMaterial();
			Mat4 transform_matrix = obj->GetTransformMatrix();

			ShaderSetMat4(&viewspace_gbuffer_shader, "model", transform_matrix.arr);
			RenderMesh(viewspace_gbuffer_shader, obj->GetMeshForRender());
		}

		UnbindFrameBuffer();

		//************************************
		// Viewspace gbuffer pass for animated objects
		//************************************
		
		//************************************
		// Screen space ambient occulsion (SSAO)
		//************************************

		//************************************
		// Deffered pass
		//************************************

		BindFrameBuffer(post_processing);

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();

		BindShader(demo_01_deffered_pbr_shader);

		ShaderBindTexture(demo_01_deffered_pbr_shader, worldspace_gbuffer.colour0_texture_attachment, 0, "g_position");
		ShaderBindTexture(demo_01_deffered_pbr_shader, worldspace_gbuffer.colour1_texture_attachment, 1, "g_normal");
		ShaderBindTexture(demo_01_deffered_pbr_shader, worldspace_gbuffer.colour2_texture_attachment, 2, "g_colour");

		RenderMesh(demo_01_deffered_pbr_shader, StandardMeshes::quad);

		UnbindFrameBuffer();

		//************************************
		// Forward pass
		//************************************
#if 0
		// @NOTE: Da THIN BOII WAY
		// @NOTE: In this case we need a much more accutrate, good blur
		//		: This way we don't have to do multi pass filtering
		lumin_filter.Filter(post_processing.colour0_texture_attachment, &luminance_texture);
		BindFrameBuffer(post_processing);
		//DebugDrawTexture(&debug_mesh_shader, luminance_texture);
		UnbindFrameBuffer();

		gblur.Blur(luminance_texture, nullptr);
		BindFrameBuffer(post_processing);
		DebugDrawTexture(&debug_mesh_shader, gblur.vertical_frame.colour0_texture_attachment);
		UnbindFrameBuffer();

#else
#if 0
		// @NOTE: In this case we wan't more aggressive blur.
		//		: However the blur doesn't have to be good
		//		: The stronger the blur the greater the chance for flickering
		gblur.Blur(post_processing.colour0_texture_attachment, nullptr);
		BindFrameBuffer(post_processing);
		//DebugDrawTexture(&debug_mesh_shader, gblur.vertical_frame.colour0_texture_attachment);
		UnbindFrameBuffer();
				
		lumin_filter.Filter(gblur.vertical_frame.colour0_texture_attachment, &luminance_texture);
		BindFrameBuffer(post_processing);
		//DebugDrawTexture(&debug_mesh_shader, luminance_texture);
		UnbindFrameBuffer();

		gblur.Blur(luminance_texture, nullptr);
		BindFrameBuffer(post_processing);
		DebugDrawTexture(&debug_mesh_shader, gblur.vertical_frame.colour0_texture_attachment);
		UnbindFrameBuffer();
#endif
#endif
		//************************************
		// Post processing pass
		//************************************

		BindShader(demo_01_postprocessing_shader);

		ShaderSetFloat(&demo_01_postprocessing_shader, "exposure", render_settings.post_processing_exposure);
		ShaderSetInt32(&demo_01_postprocessing_shader, "tonemapping_method", static_cast<uint32>(render_settings.tonemapping));

		ShaderSetFloat(&demo_01_postprocessing_shader, "vigentte_outer_radius", render_settings.vigentte_outer_raduis * render_settings.vigentte);
		ShaderSetFloat(&demo_01_postprocessing_shader, "vigentte_inner_radius", render_settings.vigentte_inner_raduis * render_settings.vigentte);
		ShaderSetFloat(&demo_01_postprocessing_shader, "vignette_intensity", render_settings.vigentte_intensity * render_settings.vigentte);

		ShaderSetInt32(&demo_01_postprocessing_shader, "FXAA", render_settings.fxaa);
		ShaderSetFloat(&demo_01_postprocessing_shader, "FXAA_SPAN_MAX", render_settings.fxaa_span_max);
		ShaderSetFloat(&demo_01_postprocessing_shader, "FXAA_DIR_MIN", render_settings.fxaa_dir_min);
		ShaderSetFloat(&demo_01_postprocessing_shader, "FXAA_DIR_REDUC", render_settings.fxaa_dir_reduc);
		
		ShaderBindTexture(demo_01_postprocessing_shader, post_processing.colour0_texture_attachment, 0, "scene_texture");


		RenderMesh(demo_01_postprocessing_shader, StandardMeshes::quad);

		
		//************************************
		// DBUGING pass
		////************************************
		//BindFrameBuffer(renderer.frame_g_buffer);
		//BindShader(ssao_gbuffer_shader);

		//RenderCommands::ClearColourBuffer();
		//RenderCommands::ClearDepthBuffer();

		//for (int32 i = 0; i < main_world.objects.size(); i++)
		//{
		//	WorldObject *obj = main_world.objects[i];
		//	Transform transform = obj->GetTransfrom();
		//	Material mat = obj->GetMaterial();
		//	Mat4 transform_matrix = obj->GetTransformMatrix();


		//	ShaderSetMat4(&ssao_gbuffer_shader, "model", transform_matrix.arr);
		//	RenderMesh(ssao_gbuffer_shader, obj->GetMeshForRender());
		//}

		//UnbindFrameBuffer();
				
		//BindFrameBuffer(ssr_frame);
		//BindShader(ssr_shader);

		//RenderCommands::ClearColourBuffer();
		//RenderCommands::ClearDepthBuffer();

		//ShaderBindTexture(ssr_shader, renderer.frame_g_buffer.colour0_texture_attachment, 0, "view_position_map");
		//ShaderBindTexture(ssr_shader, renderer.frame_g_buffer.colour1_texture_attachment, 1, "view_normal_map");

		//RenderMesh(ssr_shader, StandardMeshes::quad);

		//UnbindFrameBuffer();


		

		//renderer.Render(main_world);
	
		RenderCommands::DisableDepthBuffer();
		DebugDrawLines(&debug_shader);
		RenderCommands::EnableDepthBuffer();


#if 1
		ui_renderer.BeginFrame();

		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 pos = viewport->GetWorkPos();
		pos.x += 1280 - 300;
		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize(ImVec2(300,720));
		ImGui::SetNextWindowViewport(viewport->ID);
		
		bool p_open;
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id);
		
		ImGui::End();

		
		

		console.UpdateAndDraw();

		editor_render_window.delta_time = delta_time;
		editor_render_window.UpdateAndDraw();
		
			
		ImGui::ShowDemoWindow();


		ui_renderer.EndFrame();
#endif
		
		////////////////////
		// Post Debug Drawing
		////////////////////

		if (toggel)
		{
			//DebugDrawTexture(&debug_mesh_shader, brdf_lookup_texture);
			//DebugDrawTexture(&debug_mesh_shader, map_to_eqi);
			//DebugDrawTexture(&debug_mesh_shader, demo_gbuffer.colour1_texture_attachment);
			//DebugDrawTexture(&debug_mesh_shader, viewspace_gbuffer.colour1_texture_attachment);
			//DebugDrawTexture(&debug_mesh_shader, blurer.sampling_textures[0]);
			//DebugDrawTexture(&debug_mesh_shader, testing_blur_down01);
			//DebugDrawTexture(&debug_mesh_shader, demo_floor_normal_map);
			//DebugDrawTexture(&debug_mesh_shader, ssr_frame.colour0_texture_attachment);
			//DebugDrawTexture(&debug_mesh_shader, gblur.vertical_frame.colour0_texture_attachment);

			
			//DebugDrawTexture(&debug_mesh_shader, gblur.vertical_frame.colour0_texture_attachment);
		}
		else
		{
			
			//DebugDrawTexture(&debug_mesh_shader, renderer.frame_g_buffer.colour1_texture_attachment);
		}


		//BindShader(debug_mesh_shader);
		//glViewport(0, 0, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);
		//ShaderBindTexture(debug_mesh_shader, ssr_frame.colour0_texture_attachment, 0, "mesh_texture");
		//ShaderSetMat4(&debug_mesh_shader, "model", Mat4(1).arr);
		//RenderMesh(debug_mesh_shader, StandardMeshes::quad);
		//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		//// @TODO: Debug Draw Quad
		//// @NOTE: Draw bloom buffer;
		//glViewport(0, 0, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
		//ShaderSetFloat(post_processing_shader, "exposure", 1.0f);
		//ShaderBindTexture(post_processing_shader, *bloom_blur.texture_to_blur, 0, "scene_texture");
		//RenderMesh(post_processing_shader, standard_meshes.quad);
		//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); 

		glfwSwapBuffers(window);

		//************************************
		// Collect Stats About Last Frame
		//************************************


		std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::steady_clock::now();
		std::chrono::microseconds elapsedTime(std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime));
		float time = elapsedTime.count();
		delta_time = time * 0.001f * 0.001f;
		run_time += delta_time;


		//std::cout << "Ms: " << time * 0.001 << std::endl;
		//std::cout << "Sc: " << time * 0.001f * 0.001f << std::endl;		
	}

	LOGC("CLEAING GLFW");	
	glfwTerminate();
	LOGC("FINISHING");
	return 0;
}