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

using namespace cm;

struct Voxel
{
	Actor actor;
	Transform transform;
	Aabb aabb;	
};

struct MouseData
{
	Vec2 position = 0;
	uint32 state = 0;
	bool mouse_codes[MOUSE_KEY_AMOUNT];
};

static const uint32 WINDOW_WIDTH = 1280;
static const uint32 WINDOW_HEIGHT = 720;
static const float MOUSE_SENSITIVITY = 0.08f;

static Camera main_camera = {};
static StandardMeshes standard_meshes = {};
static MassAggregateEngine physics_engine;
static GLFWwindow *window = nullptr;
static float delta_time = 0;

static MouseData current_mouse = {};
static MouseData last_mouse = {};

static bool DRAW_LIGHT_POSITIONS = false;

void CameraMovement()
{	
	float cameraSpeed = 4 * delta_time;
	Vec3 cameraPos = main_camera.transform.position;
	Basis basis = main_camera.transform.basis;
	Vec3 cameraFront = basis.forward;
	Vec3 cameraUp = basis.upward;
	Vec3 cameraRight = basis.right;

	if (glfwGetKey(window,GLFW_KEY_Q))
	{
		cameraPos.y += cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		cameraPos.y -= cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		cameraPos -= cameraRight * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		cameraPos += cameraRight * cameraSpeed;
	}
	main_camera.transform.position = cameraPos;
}

void CameraRotate(float delta_pitch, float delta_yaw)
{
	main_camera.camera_yaw += delta_yaw;
	main_camera.camera_pitch += delta_pitch;
	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (main_camera.camera_pitch > 89.0f)
	{
		main_camera.camera_pitch = 89.0f;
	}
	else if (main_camera.camera_pitch < -89.0f)
	{
		main_camera.camera_pitch = -89.0f;
	}
	Vec3 front(0, 0, 0);
	front.x = cos(deg_to_rad(main_camera.camera_yaw)) * cos(deg_to_rad(main_camera.camera_pitch));
	front.y = sin(deg_to_rad(main_camera.camera_pitch));
	front.z = sin(deg_to_rad(main_camera.camera_yaw)) * cos(deg_to_rad(main_camera.camera_pitch));
	front = Normalize(front);

	Vec3 look = main_camera.transform.position + front;

	Vec3 up(0.0f, 1.0f, 0.0f); // World Space
	Vec3 camera_reverse_direction = Normalize((look - main_camera.transform.position));
	main_camera.transform.basis.right = Normalize(Cross(camera_reverse_direction, up));
	main_camera.transform.basis.upward = Cross(main_camera.transform.basis.right, camera_reverse_direction);
	main_camera.transform.basis.forward = Normalize(Cross(main_camera.transform.basis.upward, main_camera.transform.basis.right));
}

Ray RayFromCamera(const Vec2 &mouse_point)
{	
	Vec4 normal_coords = GetNormalisedDeviceCoordinates(WINDOW_WIDTH, WINDOW_HEIGHT, mouse_point.x, mouse_point.y);
	Vec4 view_coords = ToViewCoords(main_camera.projection_matrix, normal_coords);
	// This -1 ensure we a have something pointing in to the screen
	view_coords = Vec4(view_coords.x, view_coords.y, -1, 0);
	Vec3 world_coords = ToWorldCoords(main_camera.view_matrix, view_coords);
	
	Ray ray;
	ray.origin = main_camera.transform.position;
	ray.direction = Normalize(Vec3(world_coords.x, world_coords.y, world_coords.z));
	return ray;
}

void UpdateCamera()
{
	CameraMovement();
	Vec3 look = main_camera.transform.position + main_camera.transform.basis.forward;
	main_camera.view_matrix = LookAt(main_camera.transform.position, look, Vec3(0, 1, 0));	
}

void MousePositionCallBack(GLFWwindow *widow, double xpos, double ypos)
{
	current_mouse.position = Vec2(static_cast<float>(xpos), static_cast<float>(ypos));
	   	 
	float xoffset = current_mouse.position.x - last_mouse.position.x;
	float yoffset = last_mouse.position.y - current_mouse.position.y; //Reverse cause screen's
		
	xoffset *= MOUSE_SENSITIVITY;
	yoffset *= MOUSE_SENSITIVITY;
	CameraRotate(yoffset, xoffset);

	last_mouse.position = current_mouse.position;
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
	//DESC: ViewPort
	int32 width, height;
	glfwGetFramebufferSize(window, &width, &height );
	glViewport(0, 0, width, height); //YEET
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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

//Should be a function in opgl.h
void CreateCubeMapFrom6(CubeMap *cubemap, DynaArray<std::string> cubemap_faces_directories)
{
	DynaArray<uint8> cubemap_data[6] = {};
	cubemap->config.type = GL_TEXTURE_CUBE_MAP;
	cubemap->config.texture_format = GL_RGB;
	cubemap->config.pixel_format = GL_RGB;
	cubemap->config.data_type = GL_UNSIGNED_BYTE;
	
	for (int32 i = 0; i < 6; i++)
	{
		FileDataTexture<uint8> texture_data = LoadTexture(cubemap_faces_directories[i]);
		cubemap->config.width = texture_data.width;
		cubemap->config.height = texture_data.height;
		cubemap_data[i] = texture_data.data;
	}
	CreateCubeMap(cubemap, cubemap_data);
}

void PhysicsUpdate(float delta, PointMass *body)
{

}

void InitializeStandardMeshes()
{
	EditableMesh plane;
	plane.AddTrianlge(Vec3(0, 0, 0), Vec3(0, 1, 0), Vec3(1, 0, 0));
	plane.SetColour(Vec3(0.7, 0.1, 0.1));
	plane.AddTrianlge(Vec3(0, 1, 0), Vec3(1, 1, 0), Vec3(1, 0, 0));
	plane.SetColour(Vec3(0.1, 0.7, 0.1));
	//plane.AddTrianlge(Vec3(1, 1, 0), Vec3(1, 2, 0), Vec3(2, 1, 0));
	//plane.AddTrianlge(Vec3(1, 0, 0), Vec3(2, 0, 0), Vec3(2, -1, 0));
	//plane.AddTrianlge(Vec3(0, 0, 0), Vec3(-1, 0, 0), Vec3(0, -1, 0));
	//plane.RecaluclateNormals();
	plane.FuseVertices();
	standard_meshes.plane = plane.CreateMesh();

	EditableMesh cube;
	// Back
	cube.AddTrianlge(Vec3(0, 0, 0), Vec3(0, 1, 0), Vec3(1, 0, 0));
	cube.AddTrianlge(Vec3(0, 1, 0), Vec3(1, 1, 0), Vec3(1, 0, 0));
	// Front
	cube.AddTrianlge(Vec3(0, 0, 1), Vec3(0, 1, 1), Vec3(1, 0, 1));
	cube.AddTrianlge(Vec3(0, 1, 1), Vec3(1, 1, 1), Vec3(1, 0, 1));
	// Right
	cube.AddTrianlge(Vec3(1, 0, 0), Vec3(1, 0, 1), Vec3(1, 1, 0));
	cube.AddTrianlge(Vec3(1, 0, 1), Vec3(1, 1, 1), Vec3(1, 1, 0));
	// Left
	cube.AddTrianlge(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(0, 1, 0));
	cube.AddTrianlge(Vec3(0, 0, 1), Vec3(0, 1, 1), Vec3(0, 1, 0));
	// Top
	cube.AddTrianlge(Vec3(0, 1, 0), Vec3(0, 1, 1), Vec3(1, 1, 0));
	cube.AddTrianlge(Vec3(0, 1, 1), Vec3(1, 1, 1), Vec3(1, 1, 0));
	// Bottom
	cube.AddTrianlge(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(1, 0, 0));
	cube.AddTrianlge(Vec3(0, 0, 1), Vec3(1, 0, 1), Vec3(1, 0, 0));

	cube.RecaluclateNormals();
	//cube.FuseVertices();

	standard_meshes.cube = cube.CreateMesh();

	EditableMesh quad; 
	quad.AddTrianlge(Vec3(-1, -1, 0), Vec3(-1, 1, 0), Vec3(1, -1, 0));
	quad.AddTextureCoords(Vec3(0, 0, 0), Vec3(0, 1, 0), Vec3(01, 0, 0));
	quad.AddTrianlge(Vec3(-1, 1, 0), Vec3(1, 1, 0), Vec3(1, -1, 0));
	quad.AddTextureCoords(Vec3(0, 1, 0), Vec3(1, 1, 0), Vec3(1, 0, 0));
	quad.RecaluclateNormals();
	standard_meshes.quad = quad.CreateMesh();
}

int CreateSkyBox()
{ 
	//@Hack
	Shader equirectangular_to_cubemap_shader = CreateShader(ReadFile("shaders/cubemap_vert.glsl"), ReadFile("shaders/equirectangular_to_cubemap.glsl"));
	equirectangular_to_cubemap_shader.name = "equirect";

	Shader irradiance_shader = CreateShader(ReadFile("shaders/cubemap_vert.glsl"), ReadFile("shaders/irradince_convolution.glsl"));
	irradiance_shader.name = "irradice";

	Shader prefilter_shader = CreateShader(ReadFile("shaders/cubemap_vert.glsl"), ReadFile("shaders/prefilter.glsl"));
	prefilter_shader.name = "Prefiler";

	Shader brdf_shader = CreateShader(ReadFile("shaders/brdf_vert.glsl"), ReadFile("shaders/brdf_frag.glsl"));
	brdf_shader.name = "brdf_shader";


	uint32 frame_buffer_resolution = 512;
	uint32 irradiance_buffer_resolution = 32;
	uint32 floating_point_accuracy = GL_RGB16F;
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

	// pbr: load the HDR environment map
	// ---------------------------------
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf("textures/Milkyway_small.hdr", &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, floating_point_accuracy, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}

	// pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
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
	Mat4 captureProjection = perspective(90.0f, 1.0f, 0.1f, 10.0f);
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
	ShaderSetMat4(equirectangular_to_cubemap_shader, "projection", captureProjection.arr);
	ShaderSetInt32(equirectangular_to_cubemap_shader, "equirectangularMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, frame_buffer_resolution, frame_buffer_resolution); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		ShaderSetMat4(equirectangular_to_cubemap_shader, "view", captureViews[i].arr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderMesh(equirectangular_to_cubemap_shader, standard_meshes.cube);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glDeleteFramebuffers(1, &captureFBO);

	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	FreeShader(&equirectangular_to_cubemap_shader);
	FreeShader(&irradiance_shader);
	FreeShader(&prefilter_shader);
	FreeShader(&brdf_shader);

	return envCubemap;
}

int main()
{
	window = CreateRenderingWindow();
	InitDebug();
	InitializeStandardMeshes();

	OpenGLStatistics opengl_stats;
	GetOpenglStatistics(&opengl_stats);
	PrintOpenglStatistics(opengl_stats);

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


	Shader ray_= CreateComputeShader(ReadFile("shaders/first_compute.glsl"));
	uint32 ray_program = ray_.shader_program;

	
	uint32 num_groups_x = tex_w;
	uint32 num_groups_y = tex_h;
	uint32 num_groups_z = 1;

	uint32 num_groups_size_x = 1;
	uint32 num_groups_size_y = 1;
	uint32 num_groups_size_z = 1;

	BindShader(ray_);

	glDispatchComputeGroupSizeARB(num_groups_x, num_groups_y, num_groups_z, 
								  num_groups_size_x, num_groups_size_y, num_groups_size_z);
	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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

	Shader gpu_gaussian_blur_shader = CreateShader(ReadFile("shaders/Compute/GPU_gaussian_blur_vert.glsl"), ReadFile("shaders/Compute/GPU_gaussian_blur_frag.glsl"));
	gpu_gaussian_blur_shader.name = "gaussian_blur";

	DynaArray<float> quad_vertices =
	{
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	VertexBuffer vbo;
	vbo.lbo = BufferLayout({ ShaderDataType::Float2, ShaderDataType::Float2 });
	CreateBuffer<VertexBuffer>(&vbo, quad_vertices.size() * sizeof(float), VertexFlags::READ_WRITE);
	WriteBufferData(vbo, quad_vertices, 0);

	VertexArray vao;
	vao.vertex_buffers.push_back(vbo);
	CreateVertexArray(&vao);
	

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
	
	DynaArray<std::string> cubemap_faces_directories
	{
		//"textures/mp_orbital/orbital-element_bk.tga",// 1
		//"textures/mp_orbital/orbital-element_ft.tga",// 2
		//"textures/mp_orbital/orbital-element_up.tga",
		//"textures/mp_orbital/orbital-element_dn.tga",
		//"textures/mp_orbital/orbital-element_lf.tga",// 5
		//"textures/mp_orbital/orbital-element_rt.tga",// 6	
	};	
	
	DynaArray<Shader> shaders;
	DynaArray<Texture> textures;
	DynaArray<Mesh> meshes;
	
	CreateTextures(&textures, text_directories);
	
	CreateMeshes(&meshes, mesh_directories);	

	main_camera.projection_matrix = perspective(40, ((float)WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 250.0f);
	main_camera.target = Vec3(0);
	main_camera.transform.position = Vec3(0, 4, 5);
	main_camera.view_matrix = LookAt(main_camera.transform.position, main_camera.target, Vec3(0, 1, 0));

	uint32 pointlight_count = 4;
	UniformBuffer ubo_camera;
	CreateBuffer<UniformBuffer>(&ubo_camera ,sizeof(Mat4) * 2, VertexFlags::READ_WRITE);	
	ubo_camera.binding_location = 0;
	ubo_camera.lbo = BUFFER_LAYOUT(ShaderDataType::Mat4, ShaderDataType::Mat4);
	UniformBuffer ubo_lighting;
	CreateBuffer<UniformBuffer>(&ubo_lighting,sizeof(Vec4) * (pointlight_count * 2), VertexFlags::READ_WRITE);
	ubo_lighting.binding_location = 1;
	ubo_lighting.lbo = BUFFER_LAYOUT(ShaderDataType::Float4, ShaderDataType::Mat4, ShaderDataType::Mat4);


	DynaArray<Mat4> ubo_camera_data = { main_camera.projection_matrix, main_camera.view_matrix };

	DynaArray<Vec4> ubo_lighting_data = {
		Vec4(-4,  2.5, 4, 1), Vec4(-4, 2.5, -4, 1), Vec4(4, 2.5, 4, 1), Vec4(4, 2.5, -4, 1),
		//Vec4(0), Vec4(0), Vec4(0), Vec4(0)
		Vec4(100, 300, 100, 1), Vec4(300, 100, 100, 1), Vec4(100, 100, 300, 1), Vec4(300, 300, 300, 1)
	};
		
	SetBufferBindingPoint(ubo_camera);
	SetBufferBindingPoint(ubo_lighting);

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
	ShaderBindUniformBuffer(pbr_nomaps_batch_shader, 0, "CameraMatrices");
	//CubeMap cubemap;
	//CreateCubeMapFrom6(&cubemap, cubemap_faces_directories);

	RenderCommands::EnableDepthBuffer();

	DynaArray<Voxel> voxels;
	for (int32 i = 0; i < 4; i++)
	{
		int32 index = 0;
		Voxel cube;
		cube.actor.mesh = meshes[index];
		
		
		cube.aabb.center = (cube.aabb.max + cube.aabb.min) / 2;
		cube.aabb.raduis = (cube.aabb.max - cube.aabb.center);

		cube.transform.position = Vec3(i - 1, 1, 0);
		cube.transform.scale = Vec3(0.25);
		cube.transform.scale.x = 0.4;
		cube.transform.scale.y = .1;
		cube.transform.scale.z = 1;
		//cube.body.mass = i * 3+ 1;
		//cube.body.damping = 0.99;
		//cube.body.mass = 2;
		//cube.body.inverse_mass = 1.f / (float)(cube.body.mass);		
		voxels.push_back(cube);
	}
	voxels[0].transform.rotation = EulerToQuat(Vec3(0, 0, 25));
	voxels[0].transform.position.y += 0.2f;
	voxels[3].transform.rotation = EulerToQuat(Vec3(0, 0, -25));
	voxels[3].transform.position.y += 0.2f;





	World main_world;
	main_world.camera = main_camera;
		
	//DrawLightPositions
	if (DRAW_LIGHT_POSITIONS)
	{
		DebugAddPersistentLine(Vec3(-4, 2.5, 4), Vec3(-4, 2.6, 4));
		DebugAddPersistentLine(Vec3(-4, 2.5, -4), Vec3(-4, 2.6, -4));
		DebugAddPersistentLine(Vec3(4, 2.5, 4), Vec3(4, 2.6, 4));
		DebugAddPersistentLine(Vec3(4, 2.5, -4), Vec3(4, 2.6, -4));
	}

	uint32 	envCubemap = CreateSkyBox();
	
	DynaArray<PointMass> pms;
	for (int32 i = 0; i < 2; i++)
	{
		PointMass temp;
		temp.position = Vec3(0, 2 * (i + 1), 0);//Vec3(-3 * (i + 1), 4, 0);
		temp.SetMass(3);
		temp.damping = 0.98;
		pms.push_back(temp);
	}

	AnchoredSpringForce asft;
	asft.k = 2;
	asft.l = 1;
	asft.anchor = Vec3(-3, 5, 0);
		
	pms[0].position = Vec3(0, 4, 0);
	pms[1].position = Vec3(-2, 4, 0);
	PointMassRod joint;
	joint.max_length = 1;
	joint.pms[0] = &pms[0];
	joint.pms[1] = &pms[1];

	PointMass ground;
	ground.SetMass(0);
	ground.damping = 1;
	ground.position = Vec3(0);

	//@NOTE Why not have this other way round ? have the register point mass return a point mass pointer.
	//physics_registery.RegisterForceGenerator(&asft, &pm_test);
	//const uint32 dime = 10;
	//bool cube_field[dime][dime][dime] = {};
	//Batch cube_batch = {};
	//
	//PerlinNoise pn;
	//Vec3 vector_field[dime][dime][dime] = {};
	//PointMass vector_particle;
	//vector_particle.damping = 0.99;
	//vector_particle.SetMass(3);
	//vector_particle.position = Vec3(5.5,5.5,0);

	//for (int32 x = 0; x < dime; x++)
	//{
	//	for (int32 y = 0; y < dime; y++)
	//	{
	//		for (int32 z = 0; z < dime; z++)
	//		{
	//			uint32 frq = 1;
	//			float xx = ((float)x / (float)dime) * frq;
	//			float yy = ((float)y / (float)dime) * frq;
	//			float zz = ((float)z / (float)dime) * frq;
	//			real pno = pn.Sample(xx, yy, zz);


	//			Vec3 base = Vec3(1, 0, 0);

	//			float angle = pno * 2 * PI;

	//			//Vec3 dir = rotate(angle, base, Vec3(0, 0, 1));


	//			Vec3 dir = Vec3(cos(angle), sin(angle), cos(angle));

	//			//vector_field[x][y][z] = dir;
	//			bool r = pno > 0.5;
	//			cube_field[x][y][z] = r;
	//			if (r)
	//			{
	//				Transform transform;
	//				transform.position = Vec3(x, y, z);
	//				transform.scale = Vec3(0.98);
	//				//cube_batch.transforms.push_back(transform.CalcTransformMatrix());					
	//			}

	//			Vec3 pos = Vec3(x, y, z);
	//			//DebugAddPersistentPoint(pos);
	//			//DebugAddPersistentLine(pos, pos + dir);				
	//		}			
	//	}
	//}

	//Transform t1;
	//t1.position = Vec3(0);
	//Transform t2;
	//t2.position = Vec3(2);
	//cube_batch.transforms.push_back(t1.CalcTransformMatrix());
	//cube_batch.transforms.push_back(t2.CalcTransformMatrix());
	//CreateBatch(&cube_batch, standard_meshes.cube.vao.vertex_buffers[0], standard_meshes.cube.ibo);

	//main_world.batches.push_back(cube_batch);

	FrameBuffer post_processing;
	CreateFrameBuffer(&post_processing);
	BindFrameBuffer(post_processing);

	TextureConfig colour_buffers_config;
	colour_buffers_config.height = WINDOW_HEIGHT;
	colour_buffers_config.width = WINDOW_WIDTH;
	colour_buffers_config.data_type = GL_FLOAT;
	colour_buffers_config.texture_format = GL_RGBA16F;
	colour_buffers_config.wrap_s_mode = GL_CLAMP_TO_EDGE;
	colour_buffers_config.wrap_t_mode = GL_CLAMP_TO_EDGE;
	colour_buffers_config.wrap_r_mode = GL_CLAMP_TO_EDGE;
	
	post_processing.colour0_texture_attachment.config = colour_buffers_config;
	post_processing.colour1_texture_attachment.config = colour_buffers_config;
	
	CreateTexture(&post_processing.colour0_texture_attachment, nullptr);
	CreateTexture(&post_processing.colour1_texture_attachment, nullptr);

	post_processing.render_attchment.width = WINDOW_WIDTH;
	post_processing.render_attchment.height = WINDOW_HEIGHT;

	FrameBufferAddColourAttachtments(&post_processing);
	FrameAddBufferRenderAttachtment(&post_processing);

	bool check_frame_buffer = CheckFrameBuffer(post_processing);
	Assert(check_frame_buffer);

	GPUGaussienCompute bloom_blur;
	bloom_blur.quad = &standard_meshes.quad;
	bloom_blur.blur = &gpu_gaussian_blur_shader;
	bloom_blur.texture_to_blur = &post_processing.colour1_texture_attachment;
	bloom_blur.CreateGPUGassien();
	
	//bloom_blur.texture_to_blur = &post_processing.colour1_texture_attachment;
	//CreateGPUGassien(&bloom_blur);

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
		UpdateCamera();

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
			pms[1].velocity.x = 2;
		}
		
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
		}


		//Update Physics

		//@NOTE: Make sure we run the simulation on the updated positions that may have changed due to gameplay code					

		//int vx = (int)vector_particle.position.x;
		//int vy = (int)vector_particle.position.y;
		//if (vx > dime || vx < 0 || vy > dime || vy < 0)
		//{
		//	vector_particle.position.x = RandomUInt(1, dime - 1);
		//	vector_particle.position.y = RandomUInt(1, dime - 1);
		//}
		//else
		//{
		//	
		//	
		//	vector_particle.force_accum += 10 * vector_field[vx][vy];
		//	if (Mag(vector_particle.velocity) > 1)
		//	{
		//		std::cout << Mag(vector_particle.velocity) << std::endl;
		//		vector_particle.velocity = Normalize(vector_particle.velocity );
		//	}
		//	vector_particle.Integrate(delta_time);
		//}


		//DebugAddIrresolutePoint(vector_particle.position);

		for (int32 i = 0; i < pms.size(); i++)
		{
			//asft.ApplyForce(&pms[i], delta_time);
			if (pms[i].position.y > 0.25f)
			{
				pms[i].acceleration.y = -1;
			}
			//else
			//{
			//	pms[i].acceleration = 0;
			//	pms[i].velocity = 0;
			//}
			pms[i].Integrate(delta_time);
		}


		//physics_engine.Update(delta_time);
		//
		for (int32 i = 0; i < pms.size(); i++)
		{
			DebugAddIrresolutePoint(pms[i].position);
		}
		

		//@NOTE: Alright now apply the resulting simulation to the transforms and caclulate the transform matrix;


		PointMassContactResolver pmcr;
		pmcr.iterations = 10;
		DynaArray<PointMassContact> contacts;
		
		PointMassContact pmc = joint.CheckContact();
		if (pmc.penetration != 0)
		{
			//contacts.push_back(pmc);		
		}
		
		Vec3 side = pms[0].position + Vec3(-0.1, 0, 0);
		if (pms[1].position.x >= side.x)
		{
			PointMassContact hit_contact;
			hit_contact.penetration = Mag(side - pms[0].position);
			hit_contact.pms[0] = &pms[0];
			hit_contact.pms[1] = &pms[1];
			hit_contact.restitution = 0.4;
			hit_contact.contant_normal = Vec3(1, 0, 0);
			contacts.push_back(hit_contact);
		}

		for (int32 i = 0; i < pms.size(); i++)
		{
			if (pms[i].position.y < ground.position.y)
			{
				PointMassContact ground_contact;
				ground_contact.contant_normal = Vec3(0, 1, 0);
				ground_contact.penetration = abs(pms[i].position.y);
				ground_contact.restitution = 0.01;

				ground_contact.pms[0] = &pms[i];
				ground_contact.pms[1] = &ground;

				contacts.push_back(ground_contact);
			}
		}

		pmcr.Resolve(contacts, delta_time);

	//	for (int32 i = 0; i < point_masses.size(); i++)
	//	{
	//		point_masses[i].acceleration.y = -1;
	//		point_masses[i].Integrate(delta_time);
	//	}
	//	for (int32 i = 1; i < point_masses.size(); i++)
	//	{
	//		Vec3 top_point = point_masses[i - 1].position + Vec3(0, 0.1, 0);
	//		if (point_masses[i].position.y <= top_point.y)
	//		{
	//			PointMassContact pmc;
	//			pmc.penetration = Mag(top_point - point_masses[i].position);
	//			pmc.pms[0] = &point_masses[i];
	//			pmc.pms[1] = &point_masses[i - 1];
	//			pmc.restitution = 0.1f;
	//			pmc.contant_normal = Vec3(0, 1, 0);
	//			contacts.push_back(pmc);
	////			pmc.resolve(delta_time);
	//		}
	//	}
	//	pmcr.iterations = point_masses.size();
	//	pmcr.Resolve(contacts, delta_time);
	//	for (int32 i = 0; i < point_masses.size(); i++)
	//	{
	//		//DebugAddIrresolutePoint(point_masses[i].position);
	//	}

		//PhysicsUpdate(delta_time, &cube.body);
	
		
		//************************************
		// Update GPU Buffers
		//************************************

		DynaArray<Mat4> camera_data = { main_camera.projection_matrix, main_camera.view_matrix };
		WriteBufferData(ubo_camera, camera_data, 0);


		//************************************
		// Render The Current Frame
		//************************************

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
		RenderCommands::Clear(Colour(0, 1, 0, 1));


		BindFrameBuffer(post_processing);
		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
	

		//Draw Skybox
		BindShader(cubemap_shader);
		ShaderSetMat4(cubemap_shader, "projection", main_camera.projection_matrix.arr);
		ShaderSetMat4(cubemap_shader, "view", main_camera.view_matrix.arr);
		RenderMesh(cubemap_shader, meshes[0]);

		BindShader(pbr_nomaps_shader);
		ShaderSetMat4(pbr_nomaps_shader, "model", Mat4(1).arr);
		RenderMesh(pbr_nomaps_shader, standard_meshes.plane);
		
		RenderWorld(&pbr_nomaps_shader, &pbr_nomaps_batch_shader, main_world);
		RenderWorld(&pbr_nomaps_shader, &pbr_nomaps_batch_shader, debug_world);

		//DebugDrawing
		RenderCommands::DisableDepthBuffer();
		DebugDrawLines(&debug_shader);
		RenderCommands::EnableDepthBuffer();

		UnbindFrameBuffer();

		bloom_blur.GPUGaussienBlur();

		BindShader(post_processing_shader);
		ShaderSetFloat(post_processing_shader, "exposure", 1.0f);
		ShaderBindTexture(post_processing_shader, post_processing.colour0_texture_attachment, 0, "scene_texture");
		ShaderBindTexture(post_processing_shader, *bloom_blur.texture_to_blur, 1, "bloom_texture");
		RenderMesh(post_processing_shader, standard_meshes.quad);

		//glViewport(0, 0, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
		//ShaderSetFloat(post_processing_shader, "exposure", 1.0f);
		//ShaderBindTexture(post_processing_shader, blur_t, 0, "scene_texture");
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