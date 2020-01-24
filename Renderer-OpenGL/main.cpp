#include <glew.h>
#include <GLFW/glfw3.h>
#include <chrono>

#include "src/Core.h"
#include "src/OpenGL/RenderCommands.h"
#include "src/Math/CosmicPhysics.h"
#include "src/OpenGL/OpenGL.h"
#include "src/OpenGL/OpenGlRenderer.h"
#include "src/Mesh.h"
#include "src/Debug.h"
#include "src/AssetLoader.h"

using namespace cm;

struct Voxel
{
	Actor actor;
	Transform transform;
	PointMass body;// Array of each collision shape
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

void WindowEvents()
{
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, 1);
	}	
}

GLFWwindow* CreateRenderingWindow()
{
	if (!glfwInit())
	{
		Assert(0);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

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
	FileDataMesh data = LoadMesh("models/voxel_cube.obj", false);

	VertexBuffer vbo;
	vbo.lbo = PNT_VBO_LAYOUT;
	CreateBuffer<VertexBuffer>(&vbo, data.size_bytes, VertexFlags::READ_WRITE);
	WriteBufferData(vbo, data.data, 0);

	VertexArray vao;
	vao.vertex_buffers.push_back(vbo);

	CreateVertexArray(&vao);
	
	IndexBuffer ibo;
	CreateBuffer<IndexBuffer>(&ibo, data.indices.size() * sizeof(uint32), VertexFlags::READ_WRITE);
	ibo.index_count = data.indices.size();
	WriteBufferData(ibo, data.indices, 0);
	   
	standard_meshes.cube.vao = vao;
	standard_meshes.cube.ibo = ibo;	
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
	CreateStandardMeshes(&standard_meshes);
	
	

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
	
	Shader pbr_nomaps = CreateShader(ReadFile("shaders/pbr_nomaps_vert.glsl"), ReadFile("shaders/pbr_nomaps_frag.glsl"));
	pbr_nomaps.name = "pbr_nomaps";



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
		cube.body.damping = 0.99;
		cube.body.mass = 2;
		cube.body.inverse_mass = 1.f / (float)(cube.body.mass);		
		voxels.push_back(cube);
	}
	voxels[0].transform.rotation = EulerToQuat(Vec3(0, 0, 25));
	voxels[0].transform.position.y += 0.2f;
	voxels[3].transform.rotation = EulerToQuat(Vec3(0, 0, -25));
	voxels[3].transform.position.y += 0.2f;


	DynaArray<Actor> spheres;
	int32 sphere_count = 5;
	for (int32 i = 0; i < 5; i++)
	{
		Actor a;
		a.mesh = meshes[2];
		a.transform.position = Vec3(-(sphere_count/2) + i, 3, -2);
		a.transform.scale = Vec3(0.25);
		a.transform.rotation = EulerToQuat(Vec3(45, 45, 0 ));
		spheres.push_back(a);
	}
	DragForceGenerator drag_force_gen;
	drag_force_gen.k1 = 2;
	drag_force_gen.k2 = 5;
	AttractionForceGenerator attrac_fgen;
	attrac_fgen.origin = Vec3(0, 3, 0);
	attrac_fgen.strength = 8;
	attrac_fgen.raduis = 5;
	PartricelSpringForce part_spring_force;
	part_spring_force.k = 10;
	part_spring_force.l = 0.3;
	part_spring_force.other = &voxels[0].body; //Care don't realloc
	part_spring_force.apply_to_other = false;
	OvercrowdingSpringForceGenerator over_spring_force;
	over_spring_force.bodies.push_back(&voxels[0].body);
	over_spring_force.bodies.push_back(&voxels[1].body);
	over_spring_force.bodies.push_back(&voxels[2].body);
	over_spring_force.k = 20;
	over_spring_force.collision_length = 0.5;
	over_spring_force.l = 2.5;
	ForceRegistery force_registery;
	//force_registery.RegisterForce(&part_spring_force, &voxels[1].body);
	DynaArray<AnchoredSpringForce> anc_spring;
	anc_spring.resize(4);
	for (int32 i = 0; i < voxels.size(); i++)
	{
		AnchoredSpringForce anc_spring_force;
		anc_spring_force.k = 50;
		anc_spring_force.l = 0.5;
		anc_spring_force.anchor = voxels[i].transform.position + Vec3(0, 1, 0);
		anc_spring[i] = (anc_spring_force);
		force_registery.RegisterForce(&anc_spring[i], &voxels[i].body);
		DebugAddPersistentPoint(anc_spring_force.anchor);
	}

	for (uint32 i = 0; i < voxels.size(); i++)
	{
		force_registery.RegisterForce(&drag_force_gen, &voxels[i].body);
		//force_registery.RegisterForce(&over_spring_force, &voxels[i].body);
		//force_registery.RegisterForce(&attrac_fgen, &voxels[i].body);
	}

	const int test_p = 2;
	int *test = (int*)(&test_p);
	*test = 5;
	std::cout << test_p << std::endl;

	DynaArray<PointMass> point_masses;
	PointMass ground;
	ground.SetMass(0);
	ground.damping = 0.99;
	ground.position = Vec3(-2, 1, 0);
	point_masses.push_back(ground);
	uint32 count = 2;
	for (int32 i = 1; i < count + 1; i++)
	{
		PointMass pm;
		pm.SetMass(3);
		pm.damping = 0.99;
		pm.position = point_masses[i - 1].position + Vec3(0, 0.1, 0);
		point_masses.push_back(pm);
	}


	Actor plane;
	plane.mesh = meshes[1];
	plane.transform.scale = Vec3(4);

	World world;
	world.camera = main_camera;
		
	//DrawLightPositions
	if (DRAW_LIGHT_POSITIONS)
	{
		DebugAddPersistentLine(Vec3(-4, 2.5, 4), Vec3(-4, 2.6, 4));
		DebugAddPersistentLine(Vec3(-4, 2.5, -4), Vec3(-4, 2.6, -4));
		DebugAddPersistentLine(Vec3(4, 2.5, 4), Vec3(4, 2.6, 4));
		DebugAddPersistentLine(Vec3(4, 2.5, -4), Vec3(4, 2.6, -4));
	}

	uint32 	envCubemap = CreateSkyBox();



	while (!glfwWindowShouldClose(window))
	{
		std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();
		
		glfwPollEvents();
		UpdateCamera();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_F5))
		{
			FreeShader(&cubemap_shader);
			cubemap_shader = CreateShader(ReadFile("shaders/skybox_vert.glsl"), ReadFile("shaders/skybox_frag.glsl"));
			
			FreeShader(&pbr_shader);
			pbr_shader = CreateShader(ReadFile("shaders/pbr_vert.glsl"), ReadFile("shaders/pbr_frag.glsl"));
			ShaderBindUniformBuffer(pbr_shader, 0, "Matrices");
			ShaderBindUniformBuffer(pbr_shader, 1, "Lighting");

			FreeShader(&pbr_texture_shader);
			pbr_texture_shader = CreateShader(ReadFile("shaders/pbr_vert.glsl"), ReadFile("shaders/pbr_texture_frag.glsl"));
			ShaderBindUniformBuffer(pbr_texture_shader, 0, "Matrices");
			ShaderBindUniformBuffer(pbr_texture_shader, 1, "Lighting");

		}

		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			Vec2 pos(x, y);		
			voxels[2].body.velocity.x = 3;
		}

		WindowEvents();						

		DynaArray<Mat4> camera_data = {main_camera.projection_matrix, main_camera.view_matrix };
		WriteBufferData(ubo_camera, camera_data, 0);
		//Update Physics

		for (int32 i = 0; i < voxels.size(); i++)
		{
			//@NOTE: Make sure we run the simulation on the updated positions that may have changed due to gameplay code
			voxels[i].body.position = voxels[i].transform.position;
		}

		//@REDO: I hate this move before integreate for each individual PointMass, that way calc is done on an accurate pos
		force_registery.UpdateForces(delta_time);
		for (int32 i = 0; i < voxels.size(); i++)
		{		
			if (voxels[i].body.position.y > 0.25f)
			{
				voxels[i].body.acceleration.y = -10;				
			}
			else
			{
				voxels[i].body.acceleration.y = 0;
				voxels[i].body.velocity.y = 0;
			}		

			voxels[i].body.Integrate(delta_time);			
		}

		

		for (int32 i = 0; i < voxels.size(); i++)
		{
			//@NOTE: Alright now apply the resulting simulation to the transforms and caclulate the transform matrix;
			voxels[i].transform.position = voxels[i].body.position;
			voxels[i].actor.transform = voxels[i].transform; //@TODO: Make actor have a transform instead on mat4 
			//DebugAddIrresolutePoint(voxels[i].transform.position);
		}


		PointMassContactResolver pmcr;
		DynaArray<PointMassContact> contacts;
		
		for (int32 i = 0; i < point_masses.size(); i++)
		{
			point_masses[i].acceleration.y = -1;
			point_masses[i].Integrate(delta_time);
		}
		for (int32 i = 1; i < point_masses.size(); i++)
		{
			Vec3 top_point = point_masses[i - 1].position + Vec3(0, 0.1, 0);
			if (point_masses[i].position.y <= top_point.y)
			{
				PointMassContact pmc;
				pmc.penetration = Mag(top_point - point_masses[i].position);
				pmc.pms[0] = &point_masses[i];
				pmc.pms[1] = &point_masses[i - 1];
				pmc.restitution = 0.1f;
				pmc.contant_normal = Vec3(0, 1, 0);
				contacts.push_back(pmc);
	//			pmc.resolve(delta_time);
			}
		}
		pmcr.iterations = point_masses.size();
		pmcr.Resolve(contacts, delta_time);
		for (int32 i = 0; i < point_masses.size(); i++)
		{
			//DebugAddIrresolutePoint(point_masses[i].position);
		}

		//PhysicsUpdate(delta_time, &cube.body);
	




		//Update World
		
		world.actors.clear();		
		world.actors.push_back(plane);
		
		for (uint32 i = 0; i < voxels.size(); i++)
		{
			//world.actors.push_back(voxels[i].actor);
		}
		
		for (uint32 i = 0; i < spheres.size(); i++)
		{
			world.actors.push_back(spheres[i]);
		}
		

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
		RenderCommands::Clear(Colour(0,1, 0,1));

		//Draw Skybox
		glDepthMask(GL_FALSE);

		//BindShader(cubemap_shader);
		//ShaderSetInt32(cubemap_shader, "remove_translation", true);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.object); // @Hack write shader function
		//RenderMesh(&cubemap_shader, meshes[0]);
		

		BindShader(cubemap_shader);
		ShaderSetMat4(cubemap_shader, "projection", main_camera.projection_matrix.arr);
		ShaderSetMat4(cubemap_shader, "view", main_camera.view_matrix.arr);
		ShaderSetInt32(cubemap_shader, "environmentMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		RenderMesh(cubemap_shader, meshes[0]);

		glDepthMask(GL_TRUE);


		//for (int32 i = 0; i < world.actors.size(); i++)
		//{
		//	Mat4 transform_matrix = world.actors[i].transform.CalcTransformMatrix();
		//	ShaderSetMat4(pbr_shader, "model", transform_matrix.arr);		

		//	BindVertexArray(world.actors[i].mesh.vao);
		//	BindBuffer<IndexBuffer>(world.actors[i].mesh.ibo);
		//	
		//	glDrawElements(GL_TRIANGLES, world.actors[i].mesh.ibo.index_count, GL_UNSIGNED_INT, 0);

		//	UnbindBuffer<IndexBuffer>(world.actors[i].mesh.ibo);
		//	UnbindVertexArray();
		//}


		RenderMesh(pbr_nomaps, standard_meshes.plane);


		//DebugDrawing
		RenderCommands::DisableDepthBuffer();
		DebugDrawLines(&debug_shader);
		RenderCommands::EnableDepthBuffer();


		glfwSwapBuffers(window);

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