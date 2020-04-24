#pragma once
#include "Core.h"
#include "OpenGL/OpenGl.h"
#include "GLFW/glfw3.h"


namespace cm
{
	struct DebugQueue
	{
		// @TODO: Remove std::vector
		uint32 MAX_VERTICES_SIZE;
		VertexArray persistent_vao;
		std::vector<Vec3> irresolute_vertices;
		std::vector<Vec3> persistent_vertices;
		std::vector<Vec3> colours;
	};
	   
	class Debug
	{
		// @TODO: Clean up memory when terminating
		// @TODO: Move shaders into class
		// @TODO: More cleanup
	private:
		static bool created;

	public:
		static uint32 window_height;
		static uint32 window_width; 		
		static std::stringstream logging_stream;
		static DebugQueue debug_queue;

	public:
		static void Log(const std::string &msg);
		static void Log(const bool &msg);
		static void Log(const uint32 &msg);
		static void Log(const int32 &msg);
		static void Log(const real32 &msg);

		static void Log(const Vec3 &msg);
		static void Log(const Vec4 &msg);
		static void Log(const Mat3 &msg);
		static void Log(const Mat4 &msg);					   		 
		
		static void AddPersistentLine(const Vec3 &a, const Vec3 &b);
		static void AddIrresoluteLine(const Vec3 &a, const Vec3 &b);

		static void AddPersistentAABBMinMax(const Vec3 &min, const Vec3 &max);
		static void AddIrresoluteAABBMinMax(const Vec3 &min, const Vec3 &max);

		static void AddPersistentAABBCenterRaduis(const Vec3 &center, const Vec3 &radius);
		static void AddIrresoluteAABBCenterRaduis(const Vec3 &center, const Vec3 &radius);

		static void AddPersistentPlane(const Vec3 &origin, const Vec3 &normal);
		static void AddIrresolutePlane(const Vec3 &origin, const Vec3 &normal);

		static void AddPersistentPoint(const Vec3 &center);
		static void AddIrresolutePoint(const Vec3 &center);

		static void Draw(Shader *debug_shader);
		static void DrawTexture(Shader *shader, const Texture &t);

		static void Create();
		static void Free();

		static void ClearLogger();
		static void ClearPersistent();
	};

	void InitializeDebug(uint32 window_width, uint32 window_height, uint32 vertex_allocation);

}

