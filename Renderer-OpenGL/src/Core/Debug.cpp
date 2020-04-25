#include "Debug.h"
namespace cm
{
	std::stringstream Debug::logging_stream;

	cm::DebugQueue Debug::debug_queue;

	bool Debug::created = false;
	
	uint32 Debug::window_height = 0;

	uint32 Debug::window_width = 0;

	void Debug::Log(const std::string &msg)
	{
		logging_stream << msg << '\n';
	}

	void Debug::Log(const bool &msg)
	{
		logging_stream << msg << '\n';
	}

	void Debug::Log(const uint32 &msg)
	{
		logging_stream << msg << '\n';
	}

	void Debug::Log(const int32 &msg)
	{
		logging_stream << msg << '\n';
	}

	void Debug::Log(const real32 &msg)
	{
		logging_stream << msg << '\n';
	}

	void Debug::Log(const Vec3 &msg)
	{
		logging_stream << ToString(msg) << '\n';
	}

	void Debug::Log(const Vec4 &msg)
	{
		logging_stream << ToString(msg) << '\n';
	}

	void Debug::Log(const Mat3 &msg)
	{
		Assert(0); // @REASON: Incomplete
		//logging_stream << ToString(msg)<< '\n';
	}

	void Debug::Log(const Mat4 &msg)
	{
		logging_stream << ToString(msg) << '\n';
	}

	void Debug::ClearLogger()
	{
		logging_stream.str(std::string());
	}

	void Debug::ClearPersistent()
	{

	}

	void Debug::Draw(Shader *debug_shader)
	{
		//@Redo: DebugQueue stores a irrseloute and we just free the vbo inside.
		WriteBufferData(&debug_queue.persistent_vao.vertex_buffers[0], debug_queue.persistent_vertices, 0);

		BindShader(*debug_shader);

		BindVertexArray(debug_queue.persistent_vao);

		uint32 amount = (uint32)debug_queue.persistent_vertices.size();

		glDrawArrays(GL_LINES, 0, amount);

		UnbindVertexArray();

		amount = (uint32)debug_queue.irresolute_vertices.size();
		if (amount > 0)
		{
			VertexBuffer irresolute_vbo;
			irresolute_vbo.lbo = (std::vector<ShaderDataType> {ShaderDataType::Float4});// padding byte
			irresolute_vbo.size_bytes = sizeof(Vec4) * amount;
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

	void Debug::AddPersistentLine(const Vec3 &a, const Vec3 &b)
	{
		Assert(debug_queue.persistent_vertices.size() + 2 < debug_queue.MAX_VERTICES_SIZE);
		debug_queue.persistent_vertices.push_back(a);
		debug_queue.persistent_vertices.push_back(b);
	}

	void Debug::AddIrresoluteLine(const Vec3 &a, const Vec3 &b)
	{
		debug_queue.irresolute_vertices.push_back(a);
		debug_queue.irresolute_vertices.push_back(b);
	}

	void Debug::AddPersistentAABBMinMax(const Vec3 &min, const Vec3 &max)
	{
		Vec3 v2 = Vec3(max.x, min.y, min.z);
		Vec3 v3 = Vec3(max.x, max.y, min.z);
		Vec3 v4 = Vec3(min.x, max.y, min.z);

		Vec3 v6 = Vec3(max.x, min.y, max.z);
		Vec3 v7 = Vec3(min.x, min.y, max.z);
		Vec3 v8 = Vec3(min.x, max.y, max.z);

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

	void Debug::AddIrresoluteAABBMinMax(const Vec3 &min, const Vec3 &max)
	{
		Vec3 v2 = Vec3(max.x, min.y, min.z);
		Vec3 v3 = Vec3(max.x, max.y, min.z);
		Vec3 v4 = Vec3(min.x, max.y, min.z);

		Vec3 v6 = Vec3(max.x, min.y, max.z);
		Vec3 v7 = Vec3(min.x, min.y, max.z);
		Vec3 v8 = Vec3(min.x, max.y, max.z);

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

	void Debug::AddPersistentAABBCenterRaduis(const Vec3 &center, const Vec3 &extents)
	{
		AddPersistentAABBMinMax(center - extents, center + extents);
	}

	void Debug::AddIrresoluteAABBCenterRaduis(const Vec3 &center, const Vec3 &extents)
	{
		AddIrresoluteAABBMinMax(center - extents, center + extents);
	}

	void Debug::AddPersistentPlane(const Vec3 &origin, const Vec3 &normal)
	{
		AddPersistentPoint(origin);
		AddPersistentLine(origin, origin + normal);
	}

	void Debug::AddIrresolutePlane(const Vec3 &origin, const Vec3 &normal)
	{
		AddIrresolutePoint(origin);
		AddIrresoluteLine(origin, origin + normal);
	}

	void Debug::AddPersistentPoint(const Vec3 &center)
	{
		AddPersistentAABBCenterRaduis(center, Vec3(0.05f));
	}

	void Debug::AddIrresolutePoint(const Vec3 &center)
	{
		AddIrresoluteAABBCenterRaduis(center, Vec3(0.05f));
	}

	void Debug::AddPersistentOBB(const Vec3 &origin, const Vec3 &extents, const Basis &basis)
	{	
		Mat4 mat(basis.mat, origin);

		Vec4 v0 = Vec4(extents, 1) * mat;
		Vec4 v1 = Vec4(extents * -1, 1) * mat;
		
		Vec4 v2 = (Vec4(-extents.x,  extents.y, extents.z, 1)) * mat;
		Vec4 v3 = (Vec4(extents.x, -extents.y, extents.z, 1)) * mat;
		Vec4 v4 = (Vec4(extents.x, extents.y, -extents.z, 1)) * mat;

		Vec4 v5 = (Vec4(-extents.x, -extents.y, extents.z, 1)) * mat;
		Vec4 v6 = (Vec4(extents.x, -extents.y, -extents.z, 1)) * mat;
		Vec4 v7 = (Vec4(-extents.x, extents.y, -extents.z, 1)) * mat;
	
		AddPersistentLine(Vec4ToVec3(v0), Vec4ToVec3(v2));
		AddPersistentLine(Vec4ToVec3(v0), Vec4ToVec3(v4));
		AddPersistentLine(Vec4ToVec3(v0), Vec4ToVec3(v3));

		AddPersistentLine(Vec4ToVec3(v1), Vec4ToVec3(v5));
		AddPersistentLine(Vec4ToVec3(v1), Vec4ToVec3(v7));
		AddPersistentLine(Vec4ToVec3(v1), Vec4ToVec3(v6));
		AddPersistentLine(Vec4ToVec3(v3), Vec4ToVec3(v6));
		AddPersistentLine(Vec4ToVec3(v3), Vec4ToVec3(v5));
		AddPersistentLine(Vec4ToVec3(v2), Vec4ToVec3(v5));
		AddPersistentLine(Vec4ToVec3(v2), Vec4ToVec3(v7));
		AddPersistentLine(Vec4ToVec3(v4), Vec4ToVec3(v7));
		AddPersistentLine(Vec4ToVec3(v4), Vec4ToVec3(v6));

		//Vec3 right = (extents) *basis.mat;

		//AddPersistentLine(origin, origin + Normalize(right));

		AddPersistentPoint(origin);
	}

	void Debug::AddIrresoluteOBB(const Vec3 &origin, const Vec3 &extents, const Basis &basis)
	{
		//Vec3 v0 = (origin + extents) * basis.mat;
		//Vec3 v1 = (origin - extents) * basis.mat;

		//Vec3 v2 = (origin + Vec3(-extents.x, extents.y, extents.z)) * basis.mat;
		//Vec3 v3 = (origin + Vec3(extents.x, -extents.y, extents.z)) * basis.mat;
		//Vec3 v4 = (origin + Vec3(extents.x, extents.y, -extents.z)) * basis.mat;

		//Vec3 v5 = (origin + Vec3(-extents.x, -extents.y, extents.z)) * basis.mat;
		//Vec3 v6 = (origin + Vec3(extents.x, -extents.y, -extents.z)) * basis.mat;
		//Vec3 v7 = (origin + Vec3(-extents.x, extents.y, -extents.z)) * basis.mat;
		//
		//AddIrresoluteLine(v0, v2);
		//AddIrresoluteLine(v0, v4);
		//AddIrresoluteLine(v0, v3);

		//AddIrresoluteLine(v1, v5);
		//AddIrresoluteLine(v1, v7);
		//AddIrresoluteLine(v1, v6);

		//AddIrresoluteLine(v3, v6);
		//AddIrresoluteLine(v3, v5);

		//AddIrresoluteLine(v2, v5);
		//AddIrresoluteLine(v2, v7);

		//AddIrresoluteLine(v4, v7);
		//AddIrresoluteLine(v4, v6);
	
		//AddIrresolutePoint(origin);
			   		 	  	  
		Mat4 mat(basis.mat, origin);

		Vec4 v0 = Vec4(extents, 1) * mat;
		Vec4 v1 = Vec4(extents * -1, 1) * mat;

		Vec4 v2 = (Vec4(-extents.x, extents.y, extents.z, 1)) * mat;
		Vec4 v3 = (Vec4(extents.x, -extents.y, extents.z, 1)) * mat;
		Vec4 v4 = (Vec4(extents.x, extents.y, -extents.z, 1)) * mat;

		Vec4 v5 = (Vec4(-extents.x, -extents.y, extents.z, 1)) * mat;
		Vec4 v6 = (Vec4(extents.x, -extents.y, -extents.z, 1)) * mat;
		Vec4 v7 = (Vec4(-extents.x, extents.y, -extents.z, 1)) * mat;

		AddIrresoluteLine(Vec4ToVec3(v0), Vec4ToVec3(v2));
		AddIrresoluteLine(Vec4ToVec3(v0), Vec4ToVec3(v4));
		AddIrresoluteLine(Vec4ToVec3(v0), Vec4ToVec3(v3));
		AddIrresoluteLine(Vec4ToVec3(v1), Vec4ToVec3(v5));
		AddIrresoluteLine(Vec4ToVec3(v1), Vec4ToVec3(v7));
		AddIrresoluteLine(Vec4ToVec3(v1), Vec4ToVec3(v6));
		AddIrresoluteLine(Vec4ToVec3(v3), Vec4ToVec3(v6));
		AddIrresoluteLine(Vec4ToVec3(v3), Vec4ToVec3(v5));
		AddIrresoluteLine(Vec4ToVec3(v2), Vec4ToVec3(v5));
		AddIrresoluteLine(Vec4ToVec3(v2), Vec4ToVec3(v7));
		AddIrresoluteLine(Vec4ToVec3(v4), Vec4ToVec3(v7));
		AddIrresoluteLine(Vec4ToVec3(v4), Vec4ToVec3(v6));

		//Vec3 right = (extents) *basis.mat;

		//AddPersistentLine(origin, origin + Normalize(right));
		AddIrresolutePoint(origin);
	}

	void Debug::DrawTexture(Shader *shader, const Texture &t)
	{
		BindShader(*shader);
		ShaderSetMat4(shader, "model", Mat4(1).arr);
		ShaderBindTexture(*shader, t, 0, "mesh_texture");
		RenderMesh(*shader, StandardMeshes::quad);
	}


	void Debug::Create()
	{
		Assert(!created);
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
		Debug::window_width = window_width;
		Debug::window_height = window_height;
		Debug::Create();

		DebugQueue *debug_queue = &Debug::debug_queue;
		debug_queue->MAX_VERTICES_SIZE = vertex_allocation;


		VertexBuffer vbo;
		vbo.lbo = (std::vector<ShaderDataType> {ShaderDataType::Float4});// padding 
		vbo.size_bytes = sizeof(Vec4) * vertex_allocation;
		vbo.flags = VertexFlags::READ_WRITE;
		CreateVertexBuffer(&vbo);

		debug_queue->persistent_vao.vertex_buffers.push_back(vbo);

		CreateVertexArray(&debug_queue->persistent_vao);

		GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

	}

}


