#include "CosmicMath.h"

namespace cm
{

	Vec4 GetNormalisedDeviceCoordinates(const float & window_width, const float & window_height, const float & mouse_x, 
		const float & mouse_y)
	{
		//This is actualy clip space when the vec4 with -1 and 1
		return Vec4(2 * mouse_x / window_width - 1, -(2 * mouse_y / window_height - 1), -1, 1);
	}

	Vec4 ToViewCoords(Mat4 & projection_matrix, Vec4 & viewCoords)
	{
		Mat4 invproj = inverse(projection_matrix);
		return viewCoords * invproj;	
	}

	Vec3 ToWorldCoords(Mat4 & view_matrix, Vec4 & viewCoords)
	{
		Mat4 invView = inverse(view_matrix);
		Vec4 worldSpace = viewCoords * invView;
		return Vec3(worldSpace.x, worldSpace.y, worldSpace.z);
	}

}



