#pragma once
#include "Core.h"
#include "Math/CosmicMath.h"
#include "Math/CosmicGeometry.h"
#include "Engine/Input.h"

namespace cm
{
	struct Camera 
	{
		real  camera_yaw = -90.0f;
		real  camera_pitch = 0.0f;

		Transform transform;
		Vec3f target;
		Mat4f view_matrix;
		Mat4f projection_matrix;
	};

	class CameraController
	{
	public:
		Camera main_camera;
	
	public:
		void CameraRotate(real delta_pitch, real delta_yaw);
		void CameraMovement(real delta_time);
		void UpdateCamera(float delta_time);
		Ray RayFromCamera(const Vec2f &mouse_position, const Vec2f &window_dimenions);
	
	public:
		CameraController();
		CameraController(Camera cam);
		~CameraController();


	};

}