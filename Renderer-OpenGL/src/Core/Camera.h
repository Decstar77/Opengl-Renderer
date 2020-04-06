#pragma once
#include "Core.h"
#include "Math/CosmicMath.h"
#include "Engine/Input.h"


namespace cm
{
	struct Camera 
	{
		real  camera_yaw = -90.0f;
		real  camera_pitch = 0.0f;

		Transform transform;
		Vec3 target;
		Mat4 view_matrix;
		Mat4 projection_matrix;
	};

	class CameraController
	{
	public:
		Camera main_camera;
	public:
		CameraController();
		CameraController(Camera cam);
		~CameraController();
		void CameraRotate(real delta_pitch, real delta_yaw);
		void CameraMovement(real delta_time);
		void UpdateCamera(float delta_time);
		Ray RayFromCamera(const Vec2 &mouse_position, const Vec2 &window_dimenions);


	};

}