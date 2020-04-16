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


	//ModeImport shpere_import;
	//shpere_import.import_animations = false;
	//shpere_import.model_paths.push_back("res/models/sphere.obj");
	//shpere_import.Load();
	//StandardMeshes::sphere = shpere_import.resulting_meshes[0].CreateMesh(false);
	

	//MeshExport mesh_export;
	//mesh_export.Create(StandardMeshes::sphere);
	//mesh_export.Write("sphere.txt");
	//mesh_export.Free();

	//tut::Model tut_model;
	//std::string path = "res/models/man.dae";
	//tut_model.loadModel(path);

	
	Actor floor_tile;
	floor_tile.mesh = StandardMeshes::plane;
	floor_tile.transform.scale = Vec3(20);
	floor_tile.transform.rotation = EulerToQuat(Vec3(90, 0, 0));
	floor_tile.transform.position = Vec3(-10, 0, 10);
	floor_tile.material.forward_shader = &forward_pbr_notext_shader;

	Actor wall_front;
	wall_front.mesh = StandardMeshes::plane;
	wall_front.transform.scale = Vec3(20);
	wall_front.transform.rotation = EulerToQuat(Vec3(0, 0, 0));
	wall_front.transform.position = Vec3(-10, 0, -10);
	

	Actor wall_left;
	wall_left.mesh = StandardMeshes::plane;
	wall_left.transform.scale = Vec3(20);
	wall_left.transform.rotation = EulerToQuat(Vec3(0, -90, 0));
	wall_left.transform.position = Vec3(-10, 0, 10);
	

	Actor wall_right;
	wall_right.mesh = StandardMeshes::plane;
	wall_right.transform.scale = Vec3(20);
	wall_right.transform.rotation = EulerToQuat(Vec3(0, 90, 0));
	wall_right.transform.position = Vec3(10, 0, -10);
	

	Actor spheres[6];
	for (real32 i = 0; i < 6; i++)
	{
		Actor sphere;
		sphere.mesh = StandardMeshes::sphere;
		sphere.transform.scale = Vec3(0.5);
		sphere.transform.position = Vec3(-4 + i * 2, 4, 0);
		
		sphere.material.roughness = Clamp( (i / 6.f) + 0.02, 0, 1);
		sphere.material.metalness = Clamp( (i / 6.f) + 0.02, 0, 1);
		//sphere.material.metalness = 0.85f;
		//sphere.material.roughness = 0.1f;

		sphere.material.forward_shader = &forward_pbr_notext_shader;
		sphere.material.shadow_shader = &forward_phong_notext_shader;
		sphere.material.gbuffer_shader= &forward_phong_notext_shader;
		sphere.material.instance_shader = &forward_phong_notext_shader;
		
		spheres[(uint32)i] = sphere;
	}

	TextureImport texture_import;
	texture_import.flip = true;
	texture_import.texture_paths.push_back("res/textures/studio_small_03_2k.hdr");
	texture_import.Load(); 

	Texture hdri;
	hdri.config = texture_import.texture_configs[0];
	CreateTexture(&hdri, texture_import.texture_data[0].data());

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
	enviroment_map.config.generate_mip_maps = true;
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
	prefilter_map.config.generate_mip_maps = true;
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

	texture_import.Free();
	//enviroment_map = prefilter_map;
	//enviroment_map = irradiance_map;

#if 0
	// pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------

	Shader equirectangular_to_cubemap_shader = eqi_to_map.shader;
	Shader irradiance_shader = irradiance_calc.shader;
	uint32 hdrTexture = hdri.object;

	uint32 frame_buffer_resolution = 1024;
	uint32 irradiance_buffer_resolution = 32;
	uint32 floating_point_accuracy = GL_RGB32F;
	uint32 prefilter_resolution = 128 * 3;
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, frame_buffer_resolution, frame_buffer_resolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, floating_point_accuracy, frame_buffer_resolution, frame_buffer_resolution, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	Mat4 captureProjection = Perspective(90.0f, 1.0f, 0.1f, 10.0f);
	Mat4 captureViews[] =
	{
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f),Vec3(0.0f, -1.0f,  0.0f)),
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)),
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)),
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f), Vec3(0.0f, -1.0f,  0.0f)),
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f), Vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	BindShader(equirectangular_to_cubemap_shader);
	ShaderSetMat4(&equirectangular_to_cubemap_shader, "projection", captureProjection.arr);
	ShaderSetInt32(&equirectangular_to_cubemap_shader, "equirectangularMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, frame_buffer_resolution, frame_buffer_resolution); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		ShaderSetMat4(&equirectangular_to_cubemap_shader, "view", captureViews[i].arr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderMesh(equirectangular_to_cubemap_shader, StandardMeshes::cube);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);



	unsigned int irradianceMap;
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, floating_point_accuracy, irradiance_buffer_resolution, irradiance_buffer_resolution, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradiance_buffer_resolution, irradiance_buffer_resolution);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	BindShader(irradiance_shader);
	ShaderSetInt32(&irradiance_shader, "environmentMap", 0);
	ShaderSetMat4(&irradiance_shader, "projection", captureProjection.arr);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glViewport(0, 0, irradiance_buffer_resolution, irradiance_buffer_resolution); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		ShaderSetMat4(&irradiance_shader, "view", captureViews[i].arr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderMesh(irradiance_shader, StandardMeshes::cube);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	

#endif	
 
	//TextureImport pbr_timport;
	//pbr_timport.texture_paths.push_back("res/textures/bot1_rig_v01_Scene_Material_BaseColor.png");
	//pbr_timport.texture_paths.push_back("res/textures/bot1_rig_v01_Scene_Material_OcclusionRoughnessMetallic.png");
	//pbr_timport.Load();

	//Texture pbr_gun_albedo;
	//pbr_gun_albedo.config = pbr_timport.texture_configs[0];
	//CreateTexture(&pbr_gun_albedo, pbr_timport.texture_data[0].data());

	//Texture pbr_gun_orm;
	//pbr_gun_orm.config = pbr_timport.texture_configs[1];
	//CreateTexture(&pbr_gun_orm, pbr_timport.texture_data[1].data());

	//pbr_timport.Free();


	ModeImport pbr_model;
	pbr_model.model_paths.push_back("res/models/claud_bot.obj");
	pbr_model.Load();

	Actor pbr_gun;
	pbr_gun.material.forward_shader = &forward_pbr_notext_shader;
	pbr_gun.mesh = pbr_model.resulting_meshes[0].CreateMesh(false);
	pbr_gun.transform.rotation = EulerToQuat(Vec3(0, 90, 0));
	pbr_gun.transform.position = Vec3(0, 1, 1);
	pbr_gun.transform.scale = Vec3(1);

	pbr_model.Free();


	ModeImport model_import;
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





	World main_world;
	//main_world.objects.push_back(&test_cube_guy);
	main_world.objects.push_back(&floor_tile);
	//main_world.objects.push_back(&wall_front);
	//main_world.objects.push_back(&wall_left);
	//main_world.objects.push_back(&wall_right);

	//main_world.objects.push_back(&spheres[0]);
	//main_world.objects.push_back(&spheres[1]);
	//main_world.objects.push_back(&spheres[2]);
	//main_world.objects.push_back(&spheres[3]);
	//main_world.objects.push_back(&spheres[4]);
	//main_world.objects.push_back(&spheres[5]);
	main_world.objects.push_back(&pbr_gun);
	
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
	   
	WorldRenderer renderer;
	renderer.render_shaders.skybox_shader = cubemap_shader;
	renderer.render_shaders.depth_test_shader = simple_shadow_map_shader;
	renderer.render_shaders.g_buffer_shader = g_buffer_shader;
	renderer.render_shaders.ssao_gbuffer_shader = ssao_gbuffer_shader;
	renderer.render_shaders.ssao_shader = ssao_shader;
	renderer.render_shaders.forward_render_shader = forward_pbr_notext_shader;
	renderer.render_shaders.deferred_render_shader = deferred_shader;
	renderer.render_shaders.post_processing_shader = post_processing_shader;
	renderer.render_shaders.debug_shader = debug_shader;
	renderer.render_shaders.debug_mesh_shader = debug_mesh_shader;

	renderer.standard_shaders.simple_blur = simple_blur_shader;

	renderer.default_skybox = enviroment_map;

	renderer.WINDOW_WIDTH = WINDOW_WIDTH;
	renderer.WINDOW_HEIGHT = WINDOW_HEIGHT;
	renderer.camera = &camera_controller;
	renderer.InitFrameBuffers();
	renderer.InitIdentityTexture();

	Informer informer;
	informer.CreateUBO("WorldMatrices", BUFFER_LAYOUT(ShaderDataType::Mat4, ShaderDataType::Mat4,  ShaderDataType::Mat4), 0);
	informer.CreateUBO("LightingData", BUFFER_LAYOUT(ShaderDataType::Float4, ShaderDataType::Float4, ShaderDataType::Float4,
		ShaderDataType::Mat4, ShaderDataType::Mat4, ShaderDataType::Mat4, ShaderDataType::Mat4, ShaderDataType::Mat4), 1);

	informer.LinkShader("WorldMatrices", debug_shader);
	informer.LinkShader("WorldMatrices", debug_mesh_shader);
	informer.LinkShader("WorldMatrices", forward_phong_notext_shader);
	informer.LinkShader("WorldMatrices", deferred_shader);	
	informer.LinkShader("WorldMatrices", g_buffer_shader);

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

	EditorRender editor_render_window;
	editor_render_window.delta_time = 0;
	editor_render_window.render_settings = &renderer.render_settings;
	

	DirectionalLight sun_light;
	sun_light.direction = Normalize(Vec3(2, -4, 1));
	sun_light.light_colour = Vec3(.5);




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

			toggel = true;
		}
		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
		{
			
			

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
		Vec4(0,  1, 0, 0),  // Meta data for lighting, number of lights 
		Vec4(camera_controller.main_camera.transform.position, 0),
		Vec4(sun_light.direction, 0), Vec4(sun_light.light_colour, 0)
		};

		informer.UpdateUBO("WorldMatrices", camera_data);
		informer.UpdateUBO("LightingData", lighting_data);

		std::vector<Mat4> mats;

		DebugAddIrresolutePoint(Vec3(0, 5, 2));

		//************************************
		// Render The Current Frame
		//************************************
		
	
		
#if 0
		BindShader(animation_test_shader);

		std::vector<aiMatrix4x4> tut_tran;
		
		tut_model.boneTransform(0.2, tut_tran);
		test_cube_guy.animation_controller.Play(0);
		test_cube_guy.animation_controller.Update(delta_time);

		if (false)
		{
			for (int32 i = 0; i < tut_tran.size(); i++)
			{
				std::stringstream ss;
				ss << "gBones[" << (i) << "]";
				//ShaderSetMat4(&animation_test_shader, ss.str(), ToMatrix4f(&tut_tran[i]).arr);
			}
		}
		else
		{
			for (int32 i = 1; i < test_cube_guy.animation_controller.bones.size(); i++)
			{
				std::stringstream ss;
				ss << "gBones[" << (i - 1) << "]";
				ShaderSetMat4(&animation_test_shader, ss.str(), test_cube_guy.animation_controller.bones.at(i).current_transform.arr);
			}
			
		}		
#endif


		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
		RenderCommands::Clear(Colour(0, 1, 0, 1));
		
		BindFrameBuffer(renderer.frame_g_buffer);
		BindShader(ssao_gbuffer_shader);

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();		

		for (int32 i = 0; i < main_world.objects.size(); i++)
		{
			WorldObject *obj = main_world.objects[i];
			Transform transform = obj->GetTransfrom();
			Material mat = obj->GetMaterial();
			Mat4 transform_matrix = obj->GetTransformMatrix();


			ShaderSetMat4(&ssao_gbuffer_shader, "model", transform_matrix.arr);
			RenderMesh(ssao_gbuffer_shader, obj->GetMeshForRender());
		}

		UnbindFrameBuffer();




		BindFrameBuffer(ssr_frame);
		BindShader(ssr_shader);

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();

		ShaderBindTexture(ssr_shader, renderer.frame_g_buffer.colour0_texture_attachment, 0, "view_position_map");
		ShaderBindTexture(ssr_shader, renderer.frame_g_buffer.colour1_texture_attachment, 1, "view_normal_map");
		ShaderSetMat4(&ssr_shader, "proj", camera_controller.main_camera.projection_matrix.arr);

		RenderMesh(ssr_shader, StandardMeshes::quad);

		UnbindFrameBuffer();




		BindFrameBuffer(renderer.frame_post_processing);

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
		
		BindShader(forward_pbr_notext_shader);

		ShaderBindCubeMap(&forward_pbr_notext_shader, irradiance_map, 0, "irradiance_map");
		ShaderBindCubeMap(&forward_pbr_notext_shader, prefilter_map, 1, "prefilter_map");
		ShaderBindTexture(forward_pbr_notext_shader, brdf_lookup_texture, 2, "brdf_map");
		//ShaderBindTexture(forward_pbr_notext_shader, brdf_lookup_texture, 3, "colour_map");
		//ShaderBindTexture(forward_pbr_notext_shader, brdf_lookup_texture, 4, "orm_map");

		renderer.ForwardPass(main_world);

		renderer.DrawSkyBox();

		UnbindFrameBuffer();
		
		renderer.PostProcessingPass(main_world);

		LOG("END");
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
			//DebugDrawTexture(&debug_mesh_shader, renderer.frame_g_buffer.colour1_texture_attachment);
			DebugDrawTexture(&debug_mesh_shader, ssr_frame.colour0_texture_attachment);
		}
		else
		{
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