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
		std::vector<Vec3f> irresolute_vertices;
		std::vector<Vec3f> persistent_vertices;
		std::vector<Vec3f> colours;
	};
	   
	class Debug
	{
		// @TODO: Clean up memory when terminating
		// @TODO: More cleanup
	public:
		static bool created;
		static Shader line_shader;
		static Shader texture_shader;
		static Mat4f projection;
		static Mat4f view;
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

		static void Log(const Vec3f &msg);
		static void Log(const Vec4f &msg);
		static void Log(const Mat3f &msg);
		static void Log(const Mat4f &msg);	
		
		static void LogWarning(const String &msg);
		static void LogWarning(const uint32 &msg);
		static void LogWarning(const int32 &msg);
		static void LogWarning(const real32 &msg);

		static void LogWarning(const Vec3f &msg);
		static void LogWarning(const Vec4f &msg);
		static void LogWarning(const Mat3f &msg);
		static void LogWarning(const Mat4f &msg);
		
		static void AddPersistentLine(const Vec3f &a, const Vec3f &b);
		static void AddIrresoluteLine(const Vec3f &a, const Vec3f &b);

		static void AddPersistentAABBMinMax(const Vec3f &min, const Vec3f &max);
		static void AddIrresoluteAABBMinMax(const Vec3f &min, const Vec3f &max);

		static void AddPersistentAABBCenterRaduis(const Vec3f &center, const Vec3f &extents);
		static void AddIrresoluteAABBCenterRaduis(const Vec3f &center, const Vec3f &extents);

		static void AddPersistentPlane(const Vec3f &origin, const Vec3f &normal);
		static void AddIrresolutePlane(const Vec3f &origin, const Vec3f &normal);

		static void AddPersistentPoint(const Vec3f &center);
		static void AddIrresolutePoint(const Vec3f &center);

		static void AddPersistentOBB(const Vec3f &origin, const Vec3f &extents, const Basis &basis);
		static void AddIrresoluteOBB(const Vec3f &origin, const Vec3f &extents, const Basis &basis);

		static void Update(const Mat4f &projection, const Mat4f &view);

		static void DrawLines();
		static void DrawTexture(const Texture &t);

		static void Create();
		static void Free();

		static void ClearLogger();
		static void ClearPersistent();
	};

	void InitializeDebug(uint32 window_width, uint32 window_height, uint32 vertex_allocation);

}

