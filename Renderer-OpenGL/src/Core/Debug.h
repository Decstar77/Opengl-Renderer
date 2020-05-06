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
		// @TODO: More cleanup
	public:
		static bool created;
		static Shader line_shader;
		static Shader texture_shader;
		static Mat4 projection;
		static Mat4 view;
	public:
		static uint32 window_height;
		static uint32 window_width; 		
		static StringStream log_info_stream;
		static StringStream log_warnings_stream;
		static DebugQueue debug_queue;

	public:
		static void Log(const String &msg);
		static void Log(const bool &msg);
		static void Log(const uint32 &msg);
		static void Log(const int32 &msg);
		static void Log(const real32 &msg);

		static void Log(const Vec3 &msg);
		static void Log(const Vec4 &msg);
		static void Log(const Mat3 &msg);
		static void Log(const Mat4 &msg);	
		
		static void LogWarning(const String &msg);
		static void LogWarning(const uint32 &msg);
		static void LogWarning(const int32 &msg);
		static void LogWarning(const real32 &msg);

		static void LogWarning(const Vec3 &msg);
		static void LogWarning(const Vec4 &msg);
		static void LogWarning(const Mat3 &msg);
		static void LogWarning(const Mat4 &msg);
		
		static void AddPersistentLine(const Vec3 &a, const Vec3 &b);
		static void AddIrresoluteLine(const Vec3 &a, const Vec3 &b);

		static void AddPersistentAABBMinMax(const Vec3 &min, const Vec3 &max);
		static void AddIrresoluteAABBMinMax(const Vec3 &min, const Vec3 &max);

		static void AddPersistentAABBCenterRaduis(const Vec3 &center, const Vec3 &extents);
		static void AddIrresoluteAABBCenterRaduis(const Vec3 &center, const Vec3 &extents);

		static void AddPersistentPlane(const Vec3 &origin, const Vec3 &normal);
		static void AddIrresolutePlane(const Vec3 &origin, const Vec3 &normal);

		static void AddPersistentPoint(const Vec3 &center);
		static void AddIrresolutePoint(const Vec3 &center);

		static void AddPersistentOBB(const Vec3 &origin, const Vec3 &extents, const Basis &basis);
		static void AddIrresoluteOBB(const Vec3 &origin, const Vec3 &extents, const Basis &basis);

		static void Update(const Mat4 &projection, const Mat4 &view);

		static void DrawLines();
		static void DrawTexture(const Texture &t);

		static void Create();
		static void Free();

		static void ClearLogger();
		static void ClearPersistent();
	};

	void InitializeDebug(uint32 window_width, uint32 window_height, uint32 vertex_allocation);

}

