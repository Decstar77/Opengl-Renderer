#pragma once
#include "Core.h"
#include "OpenGL/OpenGl.h"
#include "GLFW/glfw3.h"


namespace cm
{
	// @TODO: Move into cpp file at some point
	// @TODO: Clean up memory when terminating
	
	struct DebugQueue 
	{
		uint32 MAX_VERTICES_SIZE;
		VertexArray persistent_vao;
		std::vector<Vec3> irresolute_vertices;
		std::vector<Vec3> persistent_vertices;
		std::vector<Vec3> colours;
	};

	// @NOTE: We could do something like this
	struct ExternalData
	{
		uint32 window_width;
		uint32 window_height;
		StandardMeshes *std_meshs;
	};
	// @NOTE: To prevent this. Or take in an external data as param
	//static void DebugDrawTextureOnScreen(Shader shader, Texture texture, , StandardMeshes meshes, window_width, uint32 window_height)
	//{		
	//	glViewport(0, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	//	ShaderSetFloat(shader, "exposure", 1.0f);
	//	ShaderBindTexture(shader, texture, 0, "scene_texture");
	//	RenderMesh(shader, standard_meshes.quad);
	//	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//}



	static DebugQueue debug_queue;

	static World debug_world;

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


	static void InitializeDebug()
	{
		// Amount of vertices not bytes 100 000
		uint32 alloc_size = 5000; 
		debug_queue.MAX_VERTICES_SIZE = alloc_size;


		VertexBuffer vbo;
		vbo.lbo = (std::vector<ShaderDataType> {ShaderDataType::Float4});// padding 
		vbo.size_bytes = sizeof(Vec4) * alloc_size;
		vbo.flags = VertexFlags::READ_WRITE;
		CreateVertexBuffer(&vbo);
		
		debug_queue.persistent_vao.vertex_buffers.push_back(vbo);

		CreateVertexArray(&debug_queue.persistent_vao);

		GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}		
	}

	static void DebugDrawTexture(Shader *shader, const Texture &t) // TODO: Remove the mesh parama
	{
		BindShader(*shader);
		ShaderSetMat4(shader, "model", Mat4(1).arr);
		ShaderBindTexture(*shader, t, 0, "mesh_texture");
		RenderMesh(*shader, StandardMeshes::quad);
	}

	static void DebugDrawLines(Shader *debug_shader)
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

	static void DebugAddPersistentLine(const Vec3 &a, const Vec3 &b)
	{
		Assert(debug_queue.persistent_vertices.size() + 2 < debug_queue.MAX_VERTICES_SIZE);
		debug_queue.persistent_vertices.push_back(a);
		debug_queue.persistent_vertices.push_back(b);
	}

	static void DebugAddIrresoluteLine(const Vec3 &a, const Vec3 &b)
	{
		debug_queue.irresolute_vertices.push_back(a);
		debug_queue.irresolute_vertices.push_back(b);
	}

	static void DebugAddPersistentAABBMinMax(const Vec3 &min, const Vec3 &max)
	{
		Vec3 v2 = Vec3(max.x, min.y, min.z);
		Vec3 v3 = Vec3(max.x, max.y, min.z);
		Vec3 v4 = Vec3(min.x, max.y, min.z);

		Vec3 v6 = Vec3(max.x, min.y, max.z);
		Vec3 v7 = Vec3(min.x, min.y, max.z);
		Vec3 v8 = Vec3(min.x, max.y, max.z);

		DebugAddPersistentLine(min, v2);
		DebugAddPersistentLine(min, v4);
		DebugAddPersistentLine(min, v7);

		DebugAddPersistentLine(max, v6);
		DebugAddPersistentLine(max, v8);
		DebugAddPersistentLine(max, v3);

		DebugAddPersistentLine(v3, v2);
		DebugAddPersistentLine(v3, v4);

		DebugAddPersistentLine(v2, v6);
		DebugAddPersistentLine(v6, v7);

		DebugAddPersistentLine(v8, v7);
		DebugAddPersistentLine(v8, v4);
	}

	static void DebugAddIrresoluteAABBMinMax(const Vec3 &min, const Vec3 &max)
	{
		Vec3 v2 = Vec3(max.x, min.y, min.z);
		Vec3 v3 = Vec3(max.x, max.y, min.z);
		Vec3 v4 = Vec3(min.x, max.y, min.z);

		Vec3 v6 = Vec3(max.x, min.y, max.z);
		Vec3 v7 = Vec3(min.x, min.y, max.z);
		Vec3 v8 = Vec3(min.x, max.y, max.z);

		DebugAddIrresoluteLine(min, v2);
		DebugAddIrresoluteLine(min, v4);
		DebugAddIrresoluteLine(min, v7);

		DebugAddIrresoluteLine(max, v6);
		DebugAddIrresoluteLine(max, v8);
		DebugAddIrresoluteLine(max, v3);

		DebugAddIrresoluteLine(v3, v2);
		DebugAddIrresoluteLine(v3, v4);

		DebugAddIrresoluteLine(v2, v6);
		DebugAddIrresoluteLine(v6, v7);

		DebugAddIrresoluteLine(v8, v7);
		DebugAddIrresoluteLine(v8, v4);
	}

	static void DebugAddPersistentAABBCenterRaduis(const Vec3 &center, const Vec3 &radius)
	{
		DebugAddPersistentAABBMinMax(center - radius, center + radius);
	}

	static void DebugAddIrresoluteAABBCenterRaduis(const Vec3 &center, const Vec3 &radius)
	{
		DebugAddIrresoluteAABBMinMax(center - radius, center + radius);
	}

	static void DebugAddPersistentPoint(const Vec3 &center)
	{
		DebugAddPersistentAABBCenterRaduis(center, Vec3(0.05f));
	}

	static void DebugAddIrresolutePoint(const Vec3 &center)
	{
		DebugAddIrresoluteAABBCenterRaduis(center, Vec3(0.05f));
	}



}

