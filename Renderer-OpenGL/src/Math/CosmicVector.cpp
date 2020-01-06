#pragma once
#include "CosmicMath.h"
namespace cm
{
	typedef uint8_t		uint8;
	typedef uint16_t	uint16;
	typedef uint32_t	uint32;
	typedef uint64_t	uint64;

	typedef int8_t int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int164;
	#define Assert(Value) if (!(Value)) {*(int *)0 = 0;}

	//============Other Functions============//
	polar_coord canonical(float r, float theta, float z)
	{
		theta = deg_to_rad(theta);
		if (r == 0)
		{
			theta = 0;
		}
		else
		{
			if (r < 0.f)
			{
				r = -(r);
				theta += static_cast<float>(PI);
			}
			if (fabs(theta) > PI)
			{
				theta += static_cast<float>(PI);
				theta -= static_cast<float>(floor(theta / 2 * PI) * 2 * PI);
				theta -= static_cast<float>(PI);
			}
		}
		return polar_coord(r, theta, z);
	}

	uint32 GetShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}
		Assert(0);
		return 0;
	}

	uint32 GetShaderDataTypeComponentCount(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 1;
		case ShaderDataType::Float2:   return 1 * 2;
		case ShaderDataType::Float3:   return 1 * 3;
		case ShaderDataType::Float4:   return 1 * 4;
		case ShaderDataType::Mat3:     return 1 * 3 * 3;
		case ShaderDataType::Mat4:     return 1 * 4 * 4;
		case ShaderDataType::Int:      return 1;
		case ShaderDataType::Int2:     return 1 * 2;
		case ShaderDataType::Int3:     return 1 * 3;
		case ShaderDataType::Int4:     return 1 * 4;
		case ShaderDataType::Bool:     return 1;
		}
		Assert(0);
		return 0;
	}





	//============Vector2 Functions============//
	
	float mag(const Vec2 &a)
	{
		return sqrt(a.x*a.x + a.y*a.y);
	}

	Vec2 normalize(const Vec2 &a)
	{
		float magA = mag(a);			
		return a/magA;
	}



	//============Vector3 Functions============//

	float dot(const Vec3 a, const Vec3 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	float sqrd_distance(const Vec3 &a, const Vec3 &b)
	{
		return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z);
	}

	float mag(Vec3 a)
	{
		return sqrt(a.x*a.x + a.y * a.y + a.z * a.z);
	}

	Vec3 cross(Vec3 a, Vec3 b)
	{
		float x = a.y * b.z - b.y * a.z;
		float y = a.z * b.x - b.z * a.x;
		float z = a.x * b.y - b.x * a.y;
		return Vec3(x, y, z);
	}

	Vec3 normalize(Vec3 a)
	{
		float magA = mag(a);		
		Assert(magA != 0);
		__m128 div = _mm_div_ps(a.data, _mm_set1_ps(magA));
		return Vec3(div);		
	}

	Vec3 vec4tovec3(Vec4 &a)
	{
		return Vec3(a.x, a.y, a.z);
	}


	std::string toString(Vec3 a)
	{
		std::stringstream ss;
		ss << '(' << a.x << ", " << a.y << ", " << a.z << ')';
		return ss.str();
	}

	//============Vector4 Functions============//

	float dot(Vec4 a, Vec4 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	float mag(Vec4 a)
	{
		return sqrt(a.x*a.x + a.y * a.y + a.z * a.z + a.w *a.w);
	}

	Vec4 normalize(Vec4 a)
	{
		float magA = mag(a);
		if (magA != 1)
		{
			__m128 div = _mm_div_ps(a.data, _mm_set1_ps(magA));
			return Vec4(div);
		}
		return a;
	}
	
	Vec4 vec3tovec4(Vec3 &a, float w)
	{
		return Vec4(a.x, a.y, a.z, w);
	}

	std::string toString(Vec4 a)
	{
		std::stringstream ss;
		ss << '(' << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ')';
		return ss.str();
		
	}
}
