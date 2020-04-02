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
#include "src/Core/Renderer.h"
#include "src/Debug.h"
#include "src/Engine/AssetLoader.h"
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
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Open-Gl Renderer", 0, 0);
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
	
	Shader pbr_nomaps_shader = CreateShader(ReadFile("shaders/forward_phong_notext_vert.glsl"), ReadFile("shaders/forward_phong_notext_frag.glsl"));
	pbr_nomaps_shader.name = "pbr_nomaps";

	Shader pbr_nomaps_batch_shader = CreateShader(ReadFile("shaders/forward_phong_notext_batch_vert.glsl"), ReadFile("shaders/forward_phong_notext_frag.glsl"));
	pbr_nomaps_batch_shader.name = "pbr_nomaps_batch_shader";

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
	// @HACK: Overriding this cause it has texture coords
	standard_meshes.cube = meshes[0];

	Actor floor_tile;
	floor_tile.mesh = standard_meshes.plane;
	floor_tile.transform.scale = Vec3(20);
	floor_tile.transform.rotation = EulerToQuat(Vec3(90, 0, 0));
	floor_tile.transform.position = Vec3(-10, 0, 10);

	Actor wall_front;
	wall_front.mesh = standard_meshes.plane;
	wall_front.transform.scale = Vec3(20);
	wall_front.transform.rotation = EulerToQuat(Vec3(0, 0, 0));
	wall_front.transform.position = Vec3(-10, 0, -10);

	Actor wall_left;
	wall_left.mesh = standard_meshes.plane;
	wall_left.transform.scale = Vec3(20);
	wall_left.transform.rotation = EulerToQuat(Vec3(0, -90, 0));
	wall_left.transform.position = Vec3(-10, 0, 10);

	Actor wall_right;
	wall_right.mesh = standard_meshes.plane;
	wall_right.transform.scale = Vec3(20);
	wall_right.transform.rotation = EulerToQuat(Vec3(0, 90, 0));
	wall_right.transform.position = Vec3(10, 0, -10);

	Actor test_cube_guy;
	test_cube_guy.mesh = meshes[2];
	test_cube_guy.transform.position = Vec3(0, 1, 0);
	World main_world;
	main_world.actors.push_back(floor_tile);
	main_world.actors.push_back(test_cube_guy);
	main_world.actors.push_back(wall_front);
	main_world.actors.push_back(wall_left);
	main_world.actors.push_back(wall_right);

	RenderCommands::ChangeViewPort(WINDOW_WIDTH, WINDOW_WIDTH);
	RenderCommands::EnableDepthBuffer();
	RenderCommands::EnableCubeMapSeamless();
	RenderCommands::DepthBufferFunction(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
	RenderCommands::ChangeViewPort(WINDOW_WIDTH, WINDOW_HEIGHT);

	camera_controller.main_camera.projection_matrix = Perspective(40, ((float)WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 250.0f);
	camera_controller.main_camera.target = Vec3(0);
	camera_controller.main_camera.transform.position = Vec3(0, 4, 5);
	camera_controller.main_camera.view_matrix = LookAt(camera_controller.main_camera.transform.position, camera_controller.main_camera.target, Vec3(0, 1, 0));

	Renderer renderer;
	renderer.render_shaders.skybox_shader = cubemap_shader;
	renderer.render_shaders.depth_test_shader = simple_shadow_map_shader;
	renderer.render_shaders.g_buffer_shader = g_buffer_shader;
	renderer.render_shaders.ssao_gbuffer_shader = ssao_gbuffer_shader;
	renderer.render_shaders.ssao_shader = ssao_shader;
	renderer.render_shaders.forward_render_shader = pbr_nomaps_shader;
	renderer.render_shaders.forward_render_batch_shader = pbr_nomaps_batch_shader;
	renderer.render_shaders.deferred_render_shader = deferred_shader;
	renderer.render_shaders.post_processing_shader = post_processing_shader;
	renderer.render_shaders.debug_shader = debug_shader;
	renderer.render_shaders.debug_mesh_shader = debug_mesh_shader;

	renderer.standard_shaders.simple_blur = simple_blur_shader;

	renderer.standard_meshes = standard_meshes;
	
	renderer.WINDOW_WIDTH = WINDOW_WIDTH;
	renderer.WINDOW_HEIGHT = WINDOW_HEIGHT;
	renderer.camera = &camera_controller;
	renderer.InitFrameBuffers();

	Informer informer;
	informer.CreateUBO("WorldMatrices", BUFFER_LAYOUT(ShaderDataType::Mat4, ShaderDataType::Mat4,  ShaderDataType::Mat4), 0);
	informer.CreateUBO("LightingData", BUFFER_LAYOUT(ShaderDataType::Float4, ShaderDataType::Float4, ShaderDataType::Float4,
		ShaderDataType::Mat4, ShaderDataType::Mat4, ShaderDataType::Mat4, ShaderDataType::Mat4, ShaderDataType::Mat4), 1);

	informer.LinkShader("WorldMatrices", debug_shader);
	informer.LinkShader("WorldMatrices", debug_mesh_shader);
	informer.LinkShader("WorldMatrices", pbr_nomaps_shader);
	informer.LinkShader("WorldMatrices", pbr_nomaps_batch_shader);	
	informer.LinkShader("WorldMatrices", deferred_shader);	
	informer.LinkShader("WorldMatrices", g_buffer_shader);

	informer.LinkShader("LightingData", deferred_shader);
	informer.LinkShader("LightingData", pbr_nomaps_shader);

	DirectionalLight sun_light;
	sun_light.direction = Normalize(Vec3(2, -4, 1));
	sun_light.light_colour = Vec3(.5);

	float fh = 1;

	EditableMesh tri;
	float z = 0.5;
	tri.AddTrianlge(Vec3(-0.5, -0.5, z), Vec3(0.5, -0.5, z), Vec3(0, 0.5, z));
	Mesh trimesh = tri.CreateMesh();

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
		//light_space_matrix = light_view * shadow_projections[0];

		//************************************
		// Update GPU Buffers
		//************************************

		DynaArray<Mat4> camera_data = { camera_controller.main_camera.projection_matrix, camera_controller.main_camera.view_matrix,
										light_space_matrix };

		DynaArray<Vec4> lighting_data = {
		Vec4(0,  1, 0, 0),  // Meta data for lighting, number of lights 
		Vec4(camera_controller.main_camera.transform.position, 0),
		Vec4(sun_light.direction, 0), Vec4(sun_light.light_colour, 0)
		};

		informer.UpdateUBO("WorldMatrices", camera_data);
		informer.UpdateUBO("LightingData", lighting_data);


		//************************************
		// Render The Current Frame
		//************************************

		renderer.Render(main_world);
	
		RenderCommands::DisableDepthBuffer();
		DebugDrawLines(&debug_shader);
		RenderCommands::EnableDepthBuffer();


		////////////////////
		// Post Debug Drawing
		////////////////////

		//BindShader(debug_mesh_shader);
		//glViewport(0, 0, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);
		//ShaderBindTexture(debug_mesh_shader, renderer.frame_shadow_map.depth_texture_attachment, 0, "mesh_texture");
		//ShaderSetMat4(debug_mesh_shader, "model", Mat4(1).arr);
		//RenderMesh(debug_mesh_shader, standard_meshes.quad);
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
		//std::cout << "Ms: " << time * 0.001 << std::endl;
		//std::cout << "Sc: " << time * 0.001f * 0.001f << std::endl;		
	}

	LOG("CLEAING GLFW");	
	glfwTerminate();
	LOG("FINISHING");
	return 0;
}