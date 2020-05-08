#include "Camera.h"



namespace cm
{
	CameraController::CameraController()
	{

	}

	CameraController::CameraController(Camera cam) : main_camera(cam)
	{

	}

	CameraController::~CameraController()
	{

	}

	void CameraController::CameraRotate(real delta_pitch, real delta_yaw)
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
		Vec3f front(0, 0, 0);
		front.x = cos(DegToRad(main_camera.camera_yaw)) * cos(DegToRad(main_camera.camera_pitch));
		front.y = sin(DegToRad(main_camera.camera_pitch));
		front.z = sin(DegToRad(main_camera.camera_yaw)) * cos(DegToRad(main_camera.camera_pitch));
		front = Normalize(front);

		Vec3f look = main_camera.transform.position + front;

		Vec3f up(0.0f, 1.0f, 0.0f); // World Space
		Vec3f camera_reverse_direction = Normalize((look - main_camera.transform.position));
		main_camera.transform.basis.right = Normalize(Cross(camera_reverse_direction, up));
		main_camera.transform.basis.upward = Cross(main_camera.transform.basis.right, camera_reverse_direction);
		main_camera.transform.basis.forward = Normalize(Cross(main_camera.transform.basis.upward, main_camera.transform.basis.right));
	}

	void CameraController::CameraMovement(real delta_time)
	{
		float cameraSpeed = 4 * delta_time;
		Vec3f cameraPos = main_camera.transform.position;
		Basis basis = main_camera.transform.basis;
		Vec3f cameraFront = basis.forward;
		Vec3f cameraUp = basis.upward;
		Vec3f cameraRight = basis.right;

		if (Input::GetKeyHeldDown(KEY_Q))
		{
			cameraPos.y += cameraSpeed;
		}
		if (Input::GetKeyHeldDown(KEY_E))
		{
			cameraPos.y -= cameraSpeed;
		}
		if (Input::GetKeyHeldDown(KEY_W))
		{
			cameraPos += cameraSpeed * cameraFront;
		}
		if (Input::GetKeyHeldDown(KEY_S))
		{
			cameraPos -= cameraSpeed * cameraFront;
		}
		if (Input::GetKeyHeldDown(KEY_A))
		{
			cameraPos -= cameraRight * cameraSpeed;
		}
		if (Input::GetKeyHeldDown(KEY_D))
		{
			cameraPos += cameraRight * cameraSpeed;
		}
		main_camera.transform.position = cameraPos;
	}

	Ray CameraController::RayFromCamera(const Vec2f &mouse_position, const Vec2f &window_dimenions)
	{
		Vec4f normal_coords = GetNormalisedDeviceCoordinates(window_dimenions.x, window_dimenions.y, mouse_position.x, mouse_position.y);
		Vec4f view_coords = ToViewCoords(main_camera.projection_matrix, normal_coords);
		// This -1 ensure we a have something pointing in to the screen
		view_coords = Vec4f(view_coords.x, view_coords.y, -1, 0);
		Vec3f world_coords = ToWorldCoords(main_camera.view_matrix, view_coords);

		Ray ray;
		ray.origin = main_camera.transform.position;
		ray.direction = Normalize(Vec3f(world_coords.x, world_coords.y, world_coords.z));
		return ray;
	}

	void CameraController::UpdateCamera(float delta_time)
	{
		CameraMovement(delta_time);
		Vec3f look = main_camera.transform.position + main_camera.transform.basis.forward;
		main_camera.view_matrix = LookAt(main_camera.transform.position, look, Vec3f(0, 1, 0));
	}

}