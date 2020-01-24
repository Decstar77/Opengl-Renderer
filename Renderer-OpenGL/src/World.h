#pragma once
#include "OpenGL/OpenGlRenderer.h"

namespace cm
{
	class CameraController
	{		
		Camera main_camera;
	public:
		CameraController() {};
		CameraController(Camera cam) : main_camera(cam) {}
		~CameraController() {};

		void Rotate();
		void Movement();

	};



}