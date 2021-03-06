#include "Debug.h"
namespace cm
{
	StringStream Debug::log_info_stream;

	StringStream Debug::log_warnings_stream;

	cm::DebugQueue Debug::debug_queue;

	bool Debug::created = false;

	cm::Shader Debug::line_shader;

	cm::Shader Debug::texture_shader;

	cm::Mat4f Debug::projection;

	cm::Mat4f Debug::view;

	uint32 Debug::window_height = 0;

	uint32 Debug::window_width = 0;

	void Debug::Log(const String &msg)
	{
		log_info_stream << msg << '\n';
	}

	void Debug::Log(const bool &msg)
	{
		log_info_stream << msg << '\n';
	}

	void Debug::Log(const uint32 &msg)
	{
		log_info_stream << msg << '\n';
	}

	void Debug::Log(const int32 &msg)
	{
		log_info_stream << msg << '\n';
	}

	void Debug::Log(const real32 &msg)
	{
		log_info_stream << msg << '\n';
	}

	void Debug::Log(const Vec3f &msg)
	{
		log_info_stream << ToString(msg) << '\n';
	}

	void Debug::Log(const Vec4f &msg)
	{
		log_info_stream << ToString(msg) << '\n';
	}

	void Debug::Log(const Mat3f &msg)
	{
		log_info_stream << ToString(msg) << '\n';
	}

	void Debug::Log(const Mat4f &msg)
	{
		log_info_stream << ToString(msg) << '\n';
	}

	void Debug::ClearLogger()
	{
		log_info_stream.str(String());
		log_warnings_stream.str(String());
	}

	void Debug::ClearPersistent()
	{

	}
	   
	void Debug::LogWarning(const String &msg)
	{
		log_warnings_stream << "WARNING: " << msg << '\n';
	}

	void Debug::LogWarning(const uint32 &msg)
	{
		log_info_stream << "WARNING: " << msg << '\n';
	}

	void Debug::LogWarning(const int32 &msg)
	{
		log_info_stream << "WARNING: " << msg << '\n';
	}

	void Debug::LogWarning(const real32 &msg)
	{
		log_info_stream << "WARNING: " << msg << '\n';
	}

	void Debug::LogWarning(const Vec3f &msg)
	{
		log_info_stream << "WARNING: " << ToString(msg) << '\n';
	}

	void Debug::LogWarning(const Vec4f &msg)
	{
		log_info_stream << "WARNING: " << ToString(msg) << '\n';
	}

	void Debug::LogWarning(const Mat3f &msg)
	{
		log_info_stream << "WARNING: " << ToString(msg) << '\n';
	}

	void Debug::LogWarning(const Mat4f &msg)
	{
		log_info_stream << "WARNING: " << ToString(msg) << '\n';
	}

	void Debug::AddPersistentLine(const Vec3f &a, const Vec3f &b)
	{
		Assert(debug_queue.persistent_vertices.size() + 2 < debug_queue.MAX_VERTICES_SIZE);
		debug_queue.persistent_vertices.push_back(a);
		debug_queue.persistent_vertices.push_back(b);
	}

	void Debug::AddIrresoluteLine(const Vec3f &a, const Vec3f &b)
	{
		debug_queue.irresolute_vertices.push_back(a);
		debug_queue.irresolute_vertices.push_back(b);
	}

	void Debug::AddPersistentAABBMinMax(const Vec3f &min, const Vec3f &max)
	{
		Vec3f v2 = Vec3f(max.x, min.y, min.z);
		Vec3f v3 = Vec3f(max.x, max.y, min.z);
		Vec3f v4 = Vec3f(min.x, max.y, min.z);

		Vec3f v6 = Vec3f(max.x, min.y, max.z);
		Vec3f v7 = Vec3f(min.x, min.y, max.z);
		Vec3f v8 = Vec3f(min.x, max.y, max.z);

		AddPersistentLine(min, v2);
		AddPersistentLine(min, v4);
		AddPersistentLine(min, v7);

		AddPersistentLine(max, v6);
		AddPersistentLine(max, v8);
		AddPersistentLine(max, v3);

		AddPersistentLine(v3, v2);
		AddPersistentLine(v3, v4);

		AddPersistentLine(v2, v6);
		AddPersistentLine(v6, v7);

		AddPersistentLine(v8, v7);
		AddPersistentLine(v8, v4);
	}

	void Debug::AddIrresoluteAABBMinMax(const Vec3f &min, const Vec3f &max)
	{
		Vec3f v2 = Vec3f(max.x, min.y, min.z);
		Vec3f v3 = Vec3f(max.x, max.y, min.z);
		Vec3f v4 = Vec3f(min.x, max.y, min.z);

		Vec3f v6 = Vec3f(max.x, min.y, max.z);
		Vec3f v7 = Vec3f(min.x, min.y, max.z);
		Vec3f v8 = Vec3f(min.x, max.y, max.z);

		AddIrresoluteLine(min, v2);
		AddIrresoluteLine(min, v4);
		AddIrresoluteLine(min, v7);

		AddIrresoluteLine(max, v6);
		AddIrresoluteLine(max, v8);
		AddIrresoluteLine(max, v3);

		AddIrresoluteLine(v3, v2);
		AddIrresoluteLine(v3, v4);

		AddIrresoluteLine(v2, v6);
		AddIrresoluteLine(v6, v7);

		AddIrresoluteLine(v8, v7);
		AddIrresoluteLine(v8, v4);
	}

	void Debug::AddPersistentAABBCenterRaduis(const Vec3f &center, const Vec3f &extents)
	{
		AddPersistentAABBMinMax(center - extents, center + extents);
	}

	void Debug::AddIrresoluteAABBCenterRaduis(const Vec3f &center, const Vec3f &extents)
	{
		AddIrresoluteAABBMinMax(center - extents, center + extents);
	}

	void Debug::AddPersistentPlane(const Vec3f &origin, const Vec3f &normal)
	{
		AddPersistentPoint(origin);
		AddPersistentLine(origin, origin + normal);
	}

	void Debug::AddIrresolutePlane(const Vec3f &origin, const Vec3f &normal)
	{
		AddIrresolutePoint(origin);
		AddIrresoluteLine(origin, origin + normal);
	}

	void Debug::AddPersistentPoint(const Vec3f &center)
	{
		AddPersistentAABBCenterRaduis(center, Vec3f(0.05f));
	}

	void Debug::AddIrresolutePoint(const Vec3f &center)
	{
		AddIrresoluteAABBCenterRaduis(center, Vec3f(0.05f));
	}

	void Debug::AddPersistentOBB(const Vec3f &origin, const Vec3f &extents, const Basis &basis)
	{	
		Mat4f mat(basis.mat, origin);

		Vec4f v0 = Vec4f(extents, 1) * mat;
		Vec4f v1 = Vec4f(extents * -1.0f, 1) * mat;
		
		Vec4f v2 = (Vec4f(-extents.x,  extents.y, extents.z, 1)) * mat;
		Vec4f v3 = (Vec4f(extents.x, -extents.y, extents.z, 1)) * mat;
		Vec4f v4 = (Vec4f(extents.x, extents.y, -extents.z, 1)) * mat;

		Vec4f v5 = (Vec4f(-extents.x, -extents.y, extents.z, 1)) * mat;
		Vec4f v6 = (Vec4f(extents.x, -extents.y, -extents.z, 1)) * mat;
		Vec4f v7 = (Vec4f(-extents.x, extents.y, -extents.z, 1)) * mat;
	
		AddPersistentLine(Vec3f(v0), Vec3f(v2));
		AddPersistentLine(Vec3f(v0), Vec3f(v4));
		AddPersistentLine(Vec3f(v0), Vec3f(v3));
		AddPersistentLine(Vec3f(v1), Vec3f(v5));
		AddPersistentLine(Vec3f(v1), Vec3f(v7));
		AddPersistentLine(Vec3f(v1), Vec3f(v6));
		AddPersistentLine(Vec3f(v3), Vec3f(v6));
		AddPersistentLine(Vec3f(v3), Vec3f(v5));
		AddPersistentLine(Vec3f(v2), Vec3f(v5));
		AddPersistentLine(Vec3f(v2), Vec3f(v7));
		AddPersistentLine(Vec3f(v4), Vec3f(v7));
		AddPersistentLine(Vec3f(v4), Vec3f(v6));

		AddPersistentPoint(origin);
	}

	void Debug::AddIrresoluteOBB(const Vec3f &origin, const Vec3f &extents, const Basis &basis)
	{
		Mat4f mat(basis.mat, origin);

		Vec4f v0 = Vec4f(extents, 1) * mat;
		Vec4f v1 = Vec4f(extents * -1.0f, 1) * mat;

		Vec4f v2 = (Vec4f(-extents.x, extents.y, extents.z, 1)) * mat;
		Vec4f v3 = (Vec4f(extents.x, -extents.y, extents.z, 1)) * mat;
		Vec4f v4 = (Vec4f(extents.x, extents.y, -extents.z, 1)) * mat;

		Vec4f v5 = (Vec4f(-extents.x, -extents.y, extents.z, 1)) * mat;
		Vec4f v6 = (Vec4f(extents.x, -extents.y, -extents.z, 1)) * mat;
		Vec4f v7 = (Vec4f(-extents.x, extents.y, -extents.z, 1)) * mat;

		AddIrresoluteLine(Vec3f(v0), Vec3f(v2));
		AddIrresoluteLine(Vec3f(v0), Vec3f(v4));
		AddIrresoluteLine(Vec3f(v0), Vec3f(v3));
		AddIrresoluteLine(Vec3f(v1), Vec3f(v5));
		AddIrresoluteLine(Vec3f(v1), Vec3f(v7));
		AddIrresoluteLine(Vec3f(v1), Vec3f(v6));
		AddIrresoluteLine(Vec3f(v3), Vec3f(v6));
		AddIrresoluteLine(Vec3f(v3), Vec3f(v5));
		AddIrresoluteLine(Vec3f(v2), Vec3f(v5));
		AddIrresoluteLine(Vec3f(v2), Vec3f(v7));
		AddIrresoluteLine(Vec3f(v4), Vec3f(v7));
		AddIrresoluteLine(Vec3f(v4), Vec3f(v6));

		AddIrresolutePoint(origin);
	}
	
	void Debug::Update(const Mat4f &projection, const Mat4f &view)
	{
		Debug::projection = projection;
		Debug::view = view;
	}

	void Debug::DrawTexture(const Texture &t)
	{
		BindShader(texture_shader);
		ShaderBindTexture(texture_shader, t, 0, "src_texture");
		RenderMesh(texture_shader, StandardMeshes::quad);
	}

	void Debug::DrawLines()
	{
		WriteBufferData(&debug_queue.persistent_vao.vertex_buffers[0], debug_queue.persistent_vertices, 0);

		BindShader(line_shader);
		ShaderSetMat4(&line_shader, "projection", projection.ptr);
		ShaderSetMat4(&line_shader, "view", view.ptr);

		BindVertexArray(debug_queue.persistent_vao);

		uint32 amount = static_cast<uint32>(debug_queue.persistent_vertices.size());

		glDrawArrays(GL_LINES, 0, amount);

		UnbindVertexArray();

		amount = (uint32)debug_queue.irresolute_vertices.size();
		if (amount > 0)
		{
			VertexBuffer irresolute_vbo;
			irresolute_vbo.lbo = BUFFER_LAYOUT(ShaderDataType::Float4);// padding byte
			irresolute_vbo.size_bytes = sizeof(Vec4f) * amount;
			irresolute_vbo.flags = VertexFlags::READ_WRITE;
			CreateVertexBuffer(&irresolute_vbo);
			WriteBufferData(&irresolute_vbo, debug_queue.irresolute_vertices, 0);

			VertexArray irresolute_vao;
			irresolute_vao.vertex_buffers.push_back(irresolute_vbo);

			CreateVertexArray(&irresolute_vao);

			BindVertexArray(irresolute_vao);

			glDrawArrays(GL_LINES, 0, amount);

			UnbindVertexArray();

			FreeVertexArray(&irresolute_vao, true);
			debug_queue.irresolute_vertices.clear();
		}
	}

	void Debug::Create()
	{
		Assert(!created);

		String line_vert_src = R"(
			#version 420
			layout(location = 0) in vec4 vpos; // @NOTE: Our vec3 have a padding byte

			uniform mat4 projection;
			uniform mat4 view;

			void main()
			{
				gl_Position =  projection * view * vec4(vpos.xyz, 1.0);
			}


		)";

		String line_frag_src = R"(
			#version 430
			out vec4 g_colour;

			void main()
			{				
				vec3 colour = vec3(0.1, 0.1, 0.1);
				g_colour = vec4(colour, 1.0);
			}
		)";


		String text_vert_src = R"(
			#version 420 core
			layout (location = 0) in vec3 vpos;
			layout (location = 1) in vec3 vnorm;
			layout (location = 2) in vec2 vtext;

			out vec2 texture_coords;
			void main()
			{
				texture_coords = vtext;	
				gl_Position = vec4(vpos, 1.0);
			}
		)";

		String text_frag_src = R"(
			#version 430
			out vec4 g_colour;

			uniform sampler2D src_texture;

			in vec2 texture_coords;

			void main()
			{
				//vec3 col = pow(texture(src_texture, texture_coords).rgb, vec3(2.2));
				vec3 col = texture(src_texture, texture_coords).rgb;
				g_colour = vec4(vec3(col), 1.0);
    
	
				// Depth testing for shadow map
				//float depthValue = texture(mesh_texture, fs_in.TexCoords).r;
				//FragColour = vec4(vec3(depthValue), 1.0);
				//FragColour = vec4(1.0);
			}

		)";
							   		 
		line_shader.config.src_vert = line_vert_src;
		line_shader.config.src_frag = line_frag_src;
		CreateShader(&line_shader);

		texture_shader.config.src_vert = text_vert_src;
		texture_shader.config.src_frag = text_frag_src;
		CreateShader(&texture_shader);

		created = true;
	}

	void Debug::Free()
	{
		Assert(0); // @REASON: Incomplete
	}

	static void APIENTRY glDebugOutput(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		std::cout << "---------------" << std::endl;
		std::cout << "Debug message (" << id << "): " << message << std::endl;

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
		} std::cout << std::endl;

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
		} std::cout << std::endl;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
		} std::cout << std::endl;
		std::cout << std::endl;
		Assert(0);
	}


	void InitializeDebug(uint32 window_width, uint32 window_height, uint32 vertex_allocation)
	{
		GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

		Debug::window_width = window_width;
		Debug::window_height = window_height;
		Debug::Create();

		DebugQueue *debug_queue = &Debug::debug_queue;
		debug_queue->MAX_VERTICES_SIZE = vertex_allocation;


		VertexBuffer vbo;
		vbo.lbo = BUFFER_LAYOUT(ShaderDataType::Float4); // @NOTE: padding 
		vbo.size_bytes = sizeof(Vec4f) * vertex_allocation;
		vbo.flags = VertexFlags::READ_WRITE;
		CreateVertexBuffer(&vbo);

		debug_queue->persistent_vao.vertex_buffers.push_back(vbo);

		CreateVertexArray(&debug_queue->persistent_vao);

	}

}


