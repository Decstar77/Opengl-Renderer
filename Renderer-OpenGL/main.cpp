#include <glew.h>
#include <GLFW/glfw3.h>
#include <chrono>

#include "src/Core.h"
#include "src/OpenGL/RenderCommands.h"
#include "src/Math/CosmicPhysics.h"
#include "src/Math/CosmicNoise.h"
#include "src/OpenGL/OpenGL.h"
#include "src/OpenGL/OpenGlRenderer.h"
#include "src/Mesh.h"
#include "src/GPUCompute.h"
#include "src/Debug.h"
#include "src/AssetLoader.h"
#include "src/Engine/Input.h"

using namespace cm;

struct Voxel
{
	Actor actor;
	Transform transform;
	Aabb aabb;
};

static const uint32 WINDOW_WIDTH = 1280;
static const uint32 WINDOW_HEIGHT = 720;
static const float MOUSE_SENSITIVITY = 0.08f;

static CameraController camera_controller = {};
static StandardMeshes standard_meshes = {};
static MassAggregateEngine physics_engine;
static GLFWwindow *window = nullptr;
static float delta_time = 0;


static bool DRAW_LIGHT_POSITIONS = false;


void MousePositionCallBack(GLFWwindow *widow, double xpos, double ypos)
{
	// @NOTE: GetLastMouse is now actually 2 frames behind. Thats why we use GetMouse. 
	Vec2 current_mouse = Vec2(static_cast<real>(xpos), static_cast<real>(ypos));
	Vec2 last_mouse = Input::GetMousePosition();

	real xoffset = current_mouse.x - last_mouse.x;
	real yoffset = last_mouse.y - current_mouse.y; 

	xoffset *= MOUSE_SENSITIVITY;
	yoffset *= MOUSE_SENSITIVITY;

	camera_controller.CameraRotate(yoffset, xoffset);
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
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Test", 0, 0);
	if (!window)
	{
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		Assert(0);
	}
	

	//DESC: Callbacks
	glfwSetCursorPosCallback(window, MousePositionCallBack);
	glfwSetKeyCallback(window, KeyCodeCallBack);
	return window;
}

void CreateMeshes(DynaArray<Mesh> *meshes, DynaArray<std::string> directories)
{
	//SLOW!!!!!!!!!!!!
	for (uint32 i = 0; i < directories.size(); i++)
	{
		FileDataMesh data = LoadMesh(directories.at(i), true);		
	
		VertexBuffer vbo;
		vbo.lbo = PNTTB_VBO_LAYOUT;
		CreateBuffer<VertexBuffer>(&vbo, data.size_bytes, VertexFlags::READ_WRITE);
		WriteBufferData(vbo, data.data, 0);
		
		VertexArray vao;
		vao.vertex_buffers.push_back(vbo);

		CreateVertexArray(&vao);
		IndexBuffer ibo;
		CreateBuffer<IndexBuffer>(&ibo, data.indices.size() * sizeof(uint32), VertexFlags::READ_WRITE);
		ibo.index_count = data.indices.size();
		ibo.size_bytes = data.indices.size() * sizeof(float);
		WriteBufferData(ibo, data.indices, 0);

		Mesh m;
		m.vao = vao;
		m.ibo = ibo;		
		meshes->push_back(m);
	}
}

void CreateTextures(DynaArray<Texture> *textures, DynaArray<std::string> directories)
{
	//SLOW!!!!!!!!!!!!
	TextureConfig config;
	config.data_type = GL_UNSIGNED_BYTE;
	config.texture_format = GL_RGB;
	config.pixel_format = GL_RGB;
	config.width = 1;
	config.height = 1;
	DynaArray<uint8> id_data = { 255 };
	Texture id_text;
	id_text.config = config;
	CreateTexture(&id_text, id_data.data());
	textures->push_back(id_text);

	for (uint32 i = 0; i < directories.size(); i++)
	{
		FileDataTexture<uint8> td = LoadTexture(directories.at(i));

		config.width = td.width;
		config.height = td.height;

			
		Texture t;
		t.config = config;
		CreateTexture(&t, td.data.data());
		textures->push_back(t);
	}
}

void InitializeStandardMeshes()
{
	RenderCommands::EnableFaceCulling();
	EditableMesh plane;
	// CCW
	plane.AddTrianlge(Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, 1, 0));
	plane.SetColour(Vec3(0.7, 0.1, 0.1));
	plane.AddTrianlge(Vec3(0, 1, 0), Vec3(1, 0, 0), Vec3(1, 1, 0));
	plane.SetColour(Vec3(0.1, 0.7, 0.1));
	plane.RecaluclateNormals();
	//plane.FuseVertices();
	standard_meshes.plane = plane.CreateMesh();

	EditableMesh cube;
	// CCW
	// Back
	cube.AddTrianlge(Vec3(0, 0, 0), Vec3(0, 1, 0), Vec3(1, 0, 0));
	cube.AddTrianlge(Vec3(0, 1, 0), Vec3(1, 1, 0), Vec3(1, 0, 0));
	// Front
	cube.AddTrianlge(Vec3(0, 0, 1),  Vec3(1, 0, 1), Vec3(0, 1, 1) );
	cube.AddTrianlge(Vec3(0, 1, 1), Vec3(1, 0, 1), Vec3(1, 1, 1) );
	// Right
	cube.AddTrianlge(Vec3(1, 0, 0), Vec3(1, 1, 0), Vec3(1, 0, 1));
	cube.AddTrianlge(Vec3(1, 0, 1), Vec3(1, 1, 0), Vec3(1, 1, 1));
	// Left
	cube.AddTrianlge(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(0, 1, 0));
	cube.AddTrianlge(Vec3(0, 0, 1), Vec3(0, 1, 1), Vec3(0, 1, 0));
	// Top
	cube.AddTrianlge(Vec3(0, 1, 0), Vec3(0, 1, 1), Vec3(1, 1, 0));
	cube.AddTrianlge(Vec3(0, 1, 1), Vec3(1, 1, 1), Vec3(1, 1, 0));
	// Bottom
	cube.AddTrianlge(Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, 0, 1));
	cube.AddTrianlge(Vec3(0, 0, 1), Vec3(1, 0, 0), Vec3(1, 0, 1));

	cube.RecaluclateNormals();
	//cube.FuseVertices();
	standard_meshes.cube = cube.CreateMesh();

	
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
	standard_meshes.quad = quad.CreateMesh();
}




int main()
{
	window = CreateRenderingWindow();
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

#if 0
	int32 tex_w = 1280, tex_h = 720;

	TextureConfig config;
	config.texture_format = GL_RGBA32F; 
	config.pixel_format = GL_RGBA;
	config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	config.width = tex_w;
	config.height = tex_h;

	//Texture new_texture = CreateTexture(config, nullptr);		
	//glBindImageTexture(0, new_texture.object, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);


	//Shader ray_= CreateComputeShader(ReadFile("shaders/first_compute.glsl"));
	//uint32 ray_program = ray_.shader_program;

	//
	//uint32 num_groups_x = tex_w;
	//uint32 num_groups_y = tex_h;
	//uint32 num_groups_z = 1;

	//uint32 num_groups_size_x = 1;
	//uint32 num_groups_size_y = 1;
	//uint32 num_groups_size_z = 1;

	//BindShader(ray_);

	//glDispatchComputeGroupSizeARB(num_groups_x, num_groups_y, num_groups_z, 
	//							  num_groups_size_x, num_groups_size_y, num_groups_size_z);
	//// make sure writing to image has finished before read
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
#endif


	//Shader qq = CreateShader(ReadFile("shaders/quad_vert.glsl"), ReadFile("shaders/quad_frag.glsl"));
	//qq.name = "quad";

	Shader pbr_shader = CreateShader(ReadFile("shaders/pbr_vert.glsl"), ReadFile("shaders/pbr_frag.glsl"));
	pbr_shader.name = "pbr_shader";

	Shader pbr_texture_shader = CreateShader(ReadFile("shaders/pbr_vert.glsl"), ReadFile("shaders/pbr_texture_frag.glsl"));
	pbr_texture_shader.name = "pbr_texture_shader";

	Shader cubemap_shader = CreateShader(ReadFile("shaders/skybox_vert.glsl"), ReadFile("shaders/skybox_frag.glsl"));
	cubemap_shader.name = "cubemap_shader";

	Shader debug_shader = CreateShader(ReadFile("shaders/debug_vert.glsl"), ReadFile("shaders/debug_frag.glsl"));
	debug_shader.name = "debug_shader";

	Shader debug_mesh_shader = CreateShader(ReadFile("shaders/debug_mesh_vert.glsl"), ReadFile("shaders/debug_mesh_frag.glsl"));
	debug_mesh_shader.name = "debug_quad_shader";
	
	Shader instance_shader_test = CreateShader(ReadFile("shaders/instance_test_vert.glsl"), ReadFile("shaders/instance_test_frag.glsl"));
	instance_shader_test.name = "instancing_test";
	
	Shader pbr_nomaps_shader = CreateShader(ReadFile("shaders/pbr_nomaps_vert.glsl"), ReadFile("shaders/pbr_nomaps_frag.glsl"));
	pbr_nomaps_shader.name = "pbr_nomaps";

	Shader pbr_nomaps_batch_shader = CreateShader(ReadFile("shaders/pbr_nomaps_batch_vert.glsl"), ReadFile("shaders/pbr_nomaps_frag.glsl"));
	pbr_nomaps_batch_shader.name = "pbr_nomaps_batch_shader";

	Shader post_processing_shader = CreateShader(ReadFile("shaders/post_processing_vert.glsl"), ReadFile("shaders/post_processing_frag.glsl"));
	post_processing_shader.name = "post_processing_shader";

	Shader simple_shadow_map_shader = CreateShader(ReadFile("shaders/simple_shadow_map_vert.glsl"), ReadFile("shaders/simple_shadow_map_frag.glsl"));
	simple_shadow_map_shader.name = "simple_shadow_map_vert";

	Shader gpu_gaussian_blur_shader = CreateShader(ReadFile("shaders/Compute/GPU_gaussian_blur_vert.glsl"), ReadFile("shaders/Compute/GPU_gaussian_blur_frag.glsl"));
	gpu_gaussian_blur_shader.name = "gaussian_blur";

	Shader g_buffer_shader = CreateShader(ReadFile("shaders/Compute/g_buffer_vert.glsl"), ReadFile("shaders/Compute/g_buffer_frag.glsl"));
	g_buffer_shader.name = "g_buffer_shader ";


	DynaArray<Shader> shaders;
	DynaArray<Texture> textures;
	DynaArray<Mesh> meshes;

	DynaArray<std::string> mesh_directories{  
		//"models/sponza.obj",
		"models/voxel_cube.obj",
		//"models/smooth_cube.obj",
		"models/plane.obj",
		"models/sphere.obj",
		"models/quad.obj",		
		//"models/claud_bot.obj",
	};
	DynaArray<std::string> text_directories{ 		
		//"textures/bot1_rig_v01_Scene_Material_BaseColor.png",
		//"textures/bot1_rig_v01_Scene_Material_Normal.png",
	    //"textures/bot1_rig_v01_Scene_Material_OcclusionRoughnessMetallic.png"
	};

	CreateTextures(&textures, text_directories);
	CreateMeshes(&meshes, mesh_directories);

		
	camera_controller.main_camera.projection_matrix = perspective(40, ((float)WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 250.0f);
	camera_controller.main_camera.target = Vec3(0);
	camera_controller.main_camera.transform.position = Vec3(0, 4, 5);
	camera_controller.main_camera.view_matrix = LookAt(camera_controller.main_camera.transform.position, camera_controller.main_camera.target, Vec3(0, 1, 0));
	
	Actor floor_tile;
	floor_tile.mesh = standard_meshes.plane;
	floor_tile.transform.scale = Vec3(20);
	floor_tile.transform.rotation = EulerToQuat(Vec3(90, 0, 0));
	floor_tile.transform.position = Vec3(-10, 0, 10);

	Actor test_cube_guy;
	test_cube_guy.mesh = standard_meshes.cube;
	World main_world;
	main_world.actors.push_back(floor_tile);
	main_world.actors.push_back(test_cube_guy);
	

	RenderCommands::ChangeViewPort(WINDOW_WIDTH, WINDOW_WIDTH);
	RenderCommands::EnableDepthBuffer();
	RenderCommands::EnableCubeMapSeamless();
	RenderCommands::DepthBufferFunction(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.

	UniformBuffer ubo_camera;
	ubo_camera.lbo = BUFFER_LAYOUT(ShaderDataType::Mat4, ShaderDataType::Mat4, ShaderDataType::Mat4);
	CreateUniformBuffer(&ubo_camera);

	UniformBuffer ubo_lighting;
	ubo_lighting.lbo = BUFFER_LAYOUT(ShaderDataType::Float4, ShaderDataType::Mat4, ShaderDataType::Mat4);
	CreateUniformBuffer(&ubo_lighting);


	DynaArray<Mat4> ubo_camera_data = { camera_controller.main_camera.projection_matrix, camera_controller.main_camera.view_matrix };

	DynaArray<Vec4> ubo_lighting_data = {
		Vec4(-4,  2.5, 4, 1), Vec4(-4, 2.5, -4, 1), Vec4(4, 2.5, 4, 1), Vec4(4, 2.5, -4, 1),
		//Vec4(0), Vec4(0), Vec4(0), Vec4(0)
		Vec4(100, 300, 100, 1), Vec4(300, 100, 100, 1), Vec4(100, 100, 300, 1), Vec4(300, 300, 300, 1)
	};
		
	UniformBufferSetBindPoint(ubo_camera, 0);
	UniformBufferSetBindPoint(ubo_lighting, 1);

	WriteBufferData(ubo_camera, ubo_camera_data, 0);
	WriteBufferData(ubo_lighting, ubo_lighting_data, 0);


	//@TODO: Rename the fricken ubo plz
	ShaderBindUniformBuffer(pbr_shader, 0, "Matrices");
	ShaderBindUniformBuffer(pbr_shader, 1, "Lighting");
	ShaderBindUniformBuffer(debug_shader, 0, "Matrices");
	ShaderBindUniformBuffer(pbr_texture_shader, 0, "Matrices");
	ShaderBindUniformBuffer(pbr_texture_shader, 1, "Lighting");	
	ShaderBindUniformBuffer(debug_mesh_shader, 0, "Matrices");	
	ShaderBindUniformBuffer(instance_shader_test, 0, "CameraMatrices");

	ShaderBindUniformBuffer(pbr_nomaps_shader, 0, "CameraMatrices");
	ShaderBindUniformBuffer(pbr_nomaps_batch_shader, 0, "CameraMatrices");

	ShaderBindUniformBuffer(g_buffer_shader, 0, "WorldMatrices");

	//ShaderBindUniformBuffer(pbr_nomaps_shader, 1, "LightingData");
	//ShaderBindUniformBuffer(pbr_nomaps_batch_shader, 1, "LightingData");
	
	

	CubeMap default_skybox;
	default_skybox.config.data_type = GL_FLOAT;
	default_skybox.config.texture_format = GL_RGBA16;
	default_skybox.config.min_filter = GL_LINEAR;
	default_skybox.config.mag_filter = GL_LINEAR;
	default_skybox.config.wrap_r_mode = GL_CLAMP_TO_EDGE;
	default_skybox.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	default_skybox.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	default_skybox.config.width = 512;
	default_skybox.config.height = 512;

	CreateCubeMap(&default_skybox, nullptr);

	FrameBuffer post_processing;
	CreateFrameBuffer(&post_processing);
	BindFrameBuffer(post_processing);

	TextureConfig post_processing_colour_buffers_config;
	post_processing_colour_buffers_config.height = WINDOW_HEIGHT;
	post_processing_colour_buffers_config.width = WINDOW_WIDTH;
	post_processing_colour_buffers_config.data_type = GL_FLOAT;
	post_processing_colour_buffers_config.texture_format = GL_RGBA16F;
	post_processing_colour_buffers_config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	post_processing_colour_buffers_config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	post_processing_colour_buffers_config.wrap_r_mode = GL_CLAMP_TO_EDGE;
	
	post_processing.colour0_texture_attachment.config = post_processing_colour_buffers_config;
	post_processing.colour1_texture_attachment.config = post_processing_colour_buffers_config;
	
	CreateTexture(&post_processing.colour0_texture_attachment, nullptr);
	CreateTexture(&post_processing.colour1_texture_attachment, nullptr);

	post_processing.render_attchment.width = WINDOW_WIDTH;
	post_processing.render_attchment.height = WINDOW_HEIGHT;

	FrameBufferAddColourAttachtments(&post_processing);
	FrameBufferAddColourAttachtments(&post_processing);
	FrameAddBufferRenderAttachtment(&post_processing);
	
	Assert(CheckFrameBuffer(post_processing));		
	
	FrameBuffer shadow_map0;	
	shadow_map0.depth_texture_attachment.config.texture_format = GL_DEPTH_COMPONENT32;
	shadow_map0.depth_texture_attachment.config.pixel_format = GL_DEPTH_COMPONENT;
	shadow_map0.depth_texture_attachment.config.min_filter = GL_NEAREST;
	shadow_map0.depth_texture_attachment.config.mag_filter = GL_NEAREST;
	shadow_map0.depth_texture_attachment.config.data_type = GL_FLOAT;
	shadow_map0.depth_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_BORDER;
	shadow_map0.depth_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_BORDER;
	shadow_map0.depth_texture_attachment.config.width = 1024;
	shadow_map0.depth_texture_attachment.config.height= 1024;
	
	CreateTexture(&shadow_map0.depth_texture_attachment, nullptr);	
	TextureSetBorder(&shadow_map0.depth_texture_attachment, Vec4(1).arr);
	
	CreateFrameBuffer(&shadow_map0);
	FrameBufferAddDepthAttachments(&shadow_map0);

	Assert(CheckFrameBuffer(shadow_map0));

	FrameBuffer shadow_map1;
	shadow_map1.depth_texture_attachment.config.texture_format = GL_DEPTH_COMPONENT32;
	shadow_map1.depth_texture_attachment.config.pixel_format = GL_DEPTH_COMPONENT;
	shadow_map1.depth_texture_attachment.config.min_filter = GL_NEAREST;
	shadow_map1.depth_texture_attachment.config.mag_filter = GL_NEAREST;
	shadow_map1.depth_texture_attachment.config.data_type = GL_FLOAT;
	shadow_map1.depth_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_BORDER;
	shadow_map1.depth_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_BORDER;
	shadow_map1.depth_texture_attachment.config.width = 1024;
	shadow_map1.depth_texture_attachment.config.height = 1024;

	CreateTexture(&shadow_map1.depth_texture_attachment, nullptr);
	TextureSetBorder(&shadow_map1.depth_texture_attachment, Vec4(1).arr);

	CreateFrameBuffer(&shadow_map1);
	FrameBufferAddDepthAttachments(&shadow_map1);

	Assert(CheckFrameBuffer(shadow_map1));

	FrameBuffer shadow_map2;
	shadow_map2.depth_texture_attachment.config.texture_format = GL_DEPTH_COMPONENT32;
	shadow_map2.depth_texture_attachment.config.pixel_format = GL_DEPTH_COMPONENT;
	shadow_map2.depth_texture_attachment.config.min_filter = GL_NEAREST;
	shadow_map2.depth_texture_attachment.config.mag_filter = GL_NEAREST;
	shadow_map2.depth_texture_attachment.config.data_type = GL_FLOAT;
	shadow_map2.depth_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_BORDER;
	shadow_map2.depth_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_BORDER;
	shadow_map2.depth_texture_attachment.config.width = 1024;
	shadow_map2.depth_texture_attachment.config.height = 1024;

	CreateTexture(&shadow_map2.depth_texture_attachment, nullptr);
	TextureSetBorder(&shadow_map2.depth_texture_attachment, Vec4(1).arr);

	CreateFrameBuffer(&shadow_map2);
	FrameBufferAddDepthAttachments(&shadow_map2);

	Assert(CheckFrameBuffer(shadow_map2));

	FrameBuffer g_buffer;
	
	g_buffer.colour0_texture_attachment.config.height = WINDOW_HEIGHT;
	g_buffer.colour0_texture_attachment.config.width = WINDOW_WIDTH;
	g_buffer.colour0_texture_attachment.config.data_type = GL_FLOAT;
	g_buffer.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
	g_buffer.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	g_buffer.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	g_buffer.colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	g_buffer.colour1_texture_attachment.config.height = WINDOW_HEIGHT;
	g_buffer.colour1_texture_attachment.config.width = WINDOW_WIDTH;
	g_buffer.colour1_texture_attachment.config.data_type = GL_FLOAT;
	g_buffer.colour1_texture_attachment.config.texture_format = GL_RGBA16F;
	g_buffer.colour1_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	g_buffer.colour1_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	g_buffer.colour1_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	g_buffer.colour2_texture_attachment.config.height = WINDOW_HEIGHT;
	g_buffer.colour2_texture_attachment.config.width = WINDOW_WIDTH;
	g_buffer.colour2_texture_attachment.config.data_type = GL_UNSIGNED_BYTE;
	g_buffer.colour2_texture_attachment.config.texture_format = GL_RGBA;
	g_buffer.colour2_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	g_buffer.colour2_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	g_buffer.colour2_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;

	g_buffer.render_attchment.width = WINDOW_WIDTH;
	g_buffer.render_attchment.height = WINDOW_HEIGHT;

	CreateTexture(&g_buffer.colour0_texture_attachment, nullptr);
	CreateTexture(&g_buffer.colour1_texture_attachment, nullptr);
	CreateTexture(&g_buffer.colour2_texture_attachment, nullptr);
	CreateFrameBuffer(&g_buffer);
	FrameBufferAddColourAttachtments(&g_buffer);
	FrameAddBufferRenderAttachtment(&g_buffer);

	Assert(CheckFrameBuffer(g_buffer));

	GPUGaussienCompute bloom_blur;
	bloom_blur.quad = &standard_meshes.quad;
	bloom_blur.blur = &gpu_gaussian_blur_shader;
	bloom_blur.texture_to_blur = &post_processing.colour1_texture_attachment;
	bloom_blur.CreateGPUGassien();


	DirectionalLight sun_light;
	sun_light.direction = Normalize(Vec3(2, -4, 1));
	sun_light.light_colour = Vec3(10);



	float fh = 1;
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
		camera_controller.UpdateCamera(delta_time);

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_F5))
		{
			FreeShader(&cubemap_shader);
			cubemap_shader = CreateShader(ReadFile("shaders/skybox_vert.glsl"), ReadFile("shaders/skybox_frag.glsl"));
			
			//FreeShader(&pbr_shader);
			//pbr_shader = CreateShader(ReadFile("shaders/pbr_vert.glsl"), ReadFile("shaders/pbr_frag.glsl"));
			//ShaderBindUniformBuffer(pbr_shader, 0, "Matrices");
			//ShaderBindUniformBuffer(pbr_shader, 1, "Lighting");

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
		}
		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
		{
			fh -= 0.4 * delta_time;
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
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



		/////////////////////////////////////////////////
		Mat4 cascades[4];
		float cascadeSplits[4] = { 0.0f, 0.1f, 0.5f, 1 };
		float cascade_ends[4] = {0,0,0,0};
		near_plane = 0.1f;
		far_plane = 250.f;

		float clip_range = far_plane - near_plane;

		float minZ = near_plane;
		float maxZ = near_plane + clip_range;

		float range = maxZ - minZ;
		float ratio = maxZ / minZ;


		// Calculate split depths based on view camera furstum
		// Based on method presentd in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
		for (uint32_t i = 0; i < 4; i++) {
			float p = (i + 1) / static_cast<float>(4);
			float log = minZ * std::pow(ratio, p);
			float uniform = minZ + range * p;
			float d = 0.95f * (log - uniform) + uniform;
			cascadeSplits[i] = (d - near_plane) / range;
			
		}
		
		Mat4 shadow_projections[4];

		float lastSplitDist = 0.0;	
		for (uint32 i = 0; i < 3; i++)
		{
			float splitDist = cascadeSplits[i];

			Vec3 frustumCorners[8] = {
				Vec3(-1.0f,  1.0f, -1.0f),
				Vec3(1.0f,  1.0f, -1.0f),
				Vec3(1.0f, -1.0f, -1.0f),
				Vec3(-1.0f, -1.0f, -1.0f),
				Vec3(-1.0f,  1.0f,  1.0f),
				Vec3(1.0f,  1.0f,  1.0f),
				Vec3(1.0f, -1.0f,  1.0f),
				Vec3(-1.0f, -1.0f,  1.0f)
			};

			// Project frustum corners into world space
			Mat4 invCam = Inverse(camera_controller.main_camera.view_matrix *
				camera_controller.main_camera.projection_matrix);
			for (uint32_t i = 0; i < 8; i++) {
				Vec4 invCorner = Vec4(frustumCorners[i], 1.0f) * invCam;
				invCorner = invCorner / invCorner.w;
				frustumCorners[i] = vec4tovec3(invCorner);
			}

			for (uint32_t i = 0; i < 4; i++) {
				Vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
				frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
				frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
			}
			cascade_ends[i]= (near_plane + splitDist * clip_range) * -1.0f;
			lastSplitDist = cascadeSplits[i];

			float minX = FLT_MAX;
			float minY = FLT_MAX;
			float minZ = FLT_MAX;
			float maxX = FLT_MIN;
			float maxY = FLT_MIN;
			float maxZ = FLT_MIN;

			for (uint32 j = 0; j < 8; j++)
			{	

				minX = std::min(minX, frustumCorners[j].x);
				maxX = std::max(maxX, frustumCorners[j].x);
				minY = std::min(minY, frustumCorners[j].y);
				maxY = std::max(maxY, frustumCorners[j].y);
				minZ = std::min(minZ, frustumCorners[j].z);
				maxZ = std::max(maxZ, frustumCorners[j].z);

			}
			shadow_projections[i] = light_view * Orthographic(minX, maxX, maxY, minY, minZ, maxZ);
			
		}
		//light_space_matrix = light_view * shadow_projections[0];




		//************************************
		// Update GPU Buffers
		//************************************

		DynaArray<Mat4> camera_data = { camera_controller.main_camera.projection_matrix, camera_controller.main_camera.view_matrix,
										shadow_projections[0]};
		WriteBufferData(ubo_camera, camera_data, 0);
			   

		//************************************
		// Render The Current Frame
		//************************************

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
		RenderCommands::Clear(Colour(0, 1, 0, 1));
		
		
		////////////////////
		// Shadow Pass
		////////////////////
		//Pass0
		BindFrameBuffer(shadow_map0);
		BindShader(simple_shadow_map_shader);
		RenderCommands::ChangeViewPort(1024, 1024);
		RenderCommands::ClearDepthBuffer();
		RenderCommands::CullFrontFace();
		ShaderSetMat4(simple_shadow_map_shader, "lightSpaceMatrix", shadow_projections[0].arr); // It's in ubo
		
		RenderWorld(&simple_shadow_map_shader, nullptr, main_world);

		UnbindFrameBuffer();
		RenderCommands::CullBackFace();
		RenderCommands::ChangeViewPort(WINDOW_WIDTH, WINDOW_HEIGHT);

		// Pass1
		BindFrameBuffer(shadow_map1);
		BindShader(simple_shadow_map_shader);
		RenderCommands::ChangeViewPort(1024, 1024);
		RenderCommands::ClearDepthBuffer();
		RenderCommands::CullFrontFace();
		ShaderSetMat4(simple_shadow_map_shader, "lightSpaceMatrix", shadow_projections[1].arr); // It's in ubo

		RenderWorld(&simple_shadow_map_shader, nullptr, main_world);

		UnbindFrameBuffer();
		RenderCommands::CullBackFace();
		RenderCommands::ChangeViewPort(WINDOW_WIDTH, WINDOW_HEIGHT);
		

		// Pass2
		BindFrameBuffer(shadow_map2);
		BindShader(simple_shadow_map_shader);
		RenderCommands::ChangeViewPort(1024, 1024);
		RenderCommands::ClearDepthBuffer();
		RenderCommands::CullFrontFace();
		ShaderSetMat4(simple_shadow_map_shader, "lightSpaceMatrix", shadow_projections[2].arr); // It's in ubo

		RenderWorld(&simple_shadow_map_shader, nullptr, main_world);

		UnbindFrameBuffer();
		RenderCommands::CullBackFace();
		RenderCommands::ChangeViewPort(WINDOW_WIDTH, WINDOW_HEIGHT);

			   
		////////////////////
		// G Buffer Pass
		////////////////////

		BindFrameBuffer(g_buffer);
		BindShader(g_buffer_shader);
		RenderCommands::ClearBuffers();

		RenderWorld(&g_buffer_shader, nullptr, main_world);

		UnbindFrameBuffer();
		////////////////////
		// Bind Post FrameBuffer
		////////////////////

		BindFrameBuffer(post_processing);
		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();

		////////////////////
		// Draw Skybox
		////////////////////

		RenderCommands::DisableFaceCulling();
		BindShader(cubemap_shader);
		ShaderSetMat4(cubemap_shader, "projection", camera_controller.main_camera.projection_matrix.arr);
		ShaderSetMat4(cubemap_shader, "view", camera_controller.main_camera.view_matrix.arr);
		RenderMesh(cubemap_shader, meshes[0]); // It has texture coords
		RenderCommands::EnableFaceCulling();

		////////////////////
		// Deffered Render
		////////////////////


		////////////////////
		// Forward Render
		////////////////////

		BindShader(pbr_nomaps_shader);
		//ShaderSetVec3(pbr_nomaps_shader, "light_pos", light_pos_test.arr);
		ShaderSetVec3(pbr_nomaps_shader, "light_colour", sun_light.light_colour.arr);
		ShaderSetVec3(pbr_nomaps_shader, "light_direction", sun_light.direction.arr);
		//ShaderSetFloat(pbr_nomaps_shader, "cascade_ends[0]", cascade_ends[0]);
		//ShaderSetFloat(pbr_nomaps_shader, "cascade_ends[1]", cascade_ends[1]);
		//ShaderSetFloat(pbr_nomaps_shader, "cascade_ends[2]", cascade_ends[2]);
		ShaderSetMat4(pbr_nomaps_shader, "light_space_matrices[0]", shadow_projections[0].arr);
		ShaderSetMat4(pbr_nomaps_shader, "light_space_matrices[1]", shadow_projections[1].arr);
		ShaderSetMat4(pbr_nomaps_shader, "light_space_matrices[2]", shadow_projections[2].arr);

		ShaderBindTexture(pbr_nomaps_shader, shadow_map0.depth_texture_attachment, 0, "shadow_map");
		DebugAddIrresolutePoint(Vec3(-2.0f, 4.0f, -1.0f));
		DebugAddIrresoluteLine(Vec3(-2.0f, 4.0f, -1.0f), Vec3(-2.0f, 4.0f, -1.0f) + sun_light.direction);


		RenderWorld(&pbr_nomaps_shader, &pbr_nomaps_batch_shader, main_world);
		RenderWorld(&pbr_nomaps_shader, &pbr_nomaps_batch_shader, debug_world);		
		
		////////////////////
		// Pre Debug Drawing
		////////////////////

		RenderCommands::DisableDepthBuffer();
		DebugDrawLines(&debug_shader);
		RenderCommands::EnableDepthBuffer();

		UnbindFrameBuffer();

		////////////////////
		// Draw Final Scence
		////////////////////

		bloom_blur.GPUGaussienBlur();
		BindShader(post_processing_shader);
		ShaderSetFloat(post_processing_shader, "exposure", 1.0f);
		ShaderBindTexture(post_processing_shader, post_processing.colour0_texture_attachment, 0, "scene_texture");
		ShaderBindTexture(post_processing_shader, *bloom_blur.texture_to_blur, 1, "bloom_texture");
		RenderMesh(post_processing_shader, standard_meshes.quad);

		////////////////////
		// Post Debug Drawing
		////////////////////

		// @TODO: Debug Draw Quad
		// @NOTE: Draw Shadow Map;
		BindShader(debug_mesh_shader);
		glViewport(0, 0, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);		
		ShaderBindTexture(debug_mesh_shader, shadow_map0.depth_texture_attachment, 0, "mesh_texture");
		ShaderSetMat4(debug_mesh_shader, "model", Mat4(1).arr);
		RenderMesh(debug_mesh_shader, standard_meshes.quad);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		BindShader(debug_mesh_shader);
		glViewport(WINDOW_WIDTH/3.8f, 0, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);
		ShaderBindTexture(debug_mesh_shader, shadow_map1.depth_texture_attachment, 0, "mesh_texture");
		ShaderSetMat4(debug_mesh_shader, "model", Mat4(1).arr);
		RenderMesh(debug_mesh_shader, standard_meshes.quad);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		BindShader(debug_mesh_shader);
		glViewport(WINDOW_WIDTH / 1.9f, 0, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);
		ShaderBindTexture(debug_mesh_shader, shadow_map2.depth_texture_attachment, 0, "mesh_texture");
		ShaderSetMat4(debug_mesh_shader, "model", Mat4(1).arr);
		RenderMesh(debug_mesh_shader, standard_meshes.quad);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

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
		//std::cout << "Ms: " << time * 0.001 << std::endl;
		//std::cout << "Sc: " << time * 0.001f * 0.001f << std::endl;		
	}

	LOG("CLEAING GLFW");	
	glfwTerminate();
	LOG("FINISHING");
	return 0;
}