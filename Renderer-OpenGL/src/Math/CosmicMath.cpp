#pragma once
#include "CosmicMath.h"
namespace cm
{	   	 


	   





















	Mat3f Mat4ToMat3(const Mat4f &a)
	{
		Mat3f result(1);
		result.row0 = Vec3(a.row0);
		result.row1 = Vec3(a.row1);
		result.row2 = Vec3(a.row2);
		return result;
	}

	cm::Quatf Mat3ToQuat(const Mat3f &a)
	{
		//@ HELP: 3D Math Primer for Graphics and Game Development		
		real32 m11 = a.row0.x;
		real32 m12 = a.row0.y;
		real32 m13 = a.row0.z;

		real32 m21 = a.row1.x;
		real32 m22 = a.row1.y;
		real32 m23 = a.row1.z;

		real32 m31 = a.row2.x;
		real32 m32 = a.row2.y;
		real32 m33 = a.row2.z;

		real32 x2 = m11 - m22 - m33;
		real32 y2 = m22 - m11 - m33;
		real32 z2 = m33 - m11 - m22;
		real32 w2 = m11 + m22 + m33;

		int32 index = 0;
		real32 big2 = w2;
		if (x2 > big2)
		{
			big2 = x2;
			index = 1;
		}
		if (y2 > big2)
		{
			big2 = y2;
			index = 2;
		}
		if (z2 > big2)
		{
			big2 = z2;
			index = 3;
		}

		real32 big = sqrt(big2 + 1.0f) * 0.5f;
		real32 mult = 0.25f / big;

		switch (index)
		{
		case 0: {
			real32 x = (m23 - m32) * mult;
			real32 y = (m31 - m13) * mult;
			real32 z = (m12 - m21) * mult;
			real32 w = big;
			return Quatf(x, y, z, w);
		}
		case 1: {
			real32 x = big;
			real32 y = (m12 + m21) * mult;
			real32 z = (m31 + m13) * mult;
			real32 w = (m23 - m32) * mult;
			return Quatf(x, y, z, w);
		}
		case 2: {
			real32 x = (m12 + m21) * mult;
			real32 y = big;
			real32 z = (m23 + m32) * mult;
			real32 w = (m31 - m13) * mult;
			return Quatf(x, y, z, w);
		}
		case 3: {
			real32 x = (m31 + m13) * mult;
			real32 y = (m23 + m32) * mult;
			real32 z = big;
			real32 w = (m12 - m21) * mult;
			return Quatf(x, y, z, w);
		}
		default:
			Assert(0);
			return Quatf(0, 0, 0, 1);
		}
	}




	




















	Mat4f Mat3ToMat4(const Mat3f &a, const Vec4f &b)
	{
		Mat4f result(1);
		result.row0 = Vec4f(a.row0, 0);
		result.row1 = Vec4f(a.row1, 0);
		result.row2 = Vec4f(a.row2, 0);
		result.row3 = b;
		return result;
	}



















	Quatf Mat4ToQuat(const Mat4f &a)
	{
		//@ HELP: 3D Math Primer for Graphics and Game Development		
		real32 m11 = a.row0.x;
		real32 m12 = a.row0.y;
		real32 m13 = a.row0.z;

		real32 m21 = a.row1.x;
		real32 m22 = a.row1.y;
		real32 m23 = a.row1.z;
		
		real32 m31 = a.row2.x;
		real32 m32 = a.row2.y;
		real32 m33 = a.row2.z;

		real32 x2 = m11 - m22 - m33;
		real32 y2 = m22 - m11 - m33;
		real32 z2 = m33 - m11 - m22;
		real32 w2 = m11 + m22 + m33;

		int32 index = 0;
		real32 big2 = w2;
		if (x2 > big2)
		{
			big2 = x2;
			index = 1;
		}
		if (y2 > big2)
		{
			big2 = y2;
			index = 2;
		}
		if (z2 > big2)
		{
			big2 = z2;
			index = 3;
		}

		real32 big = sqrt(big2 + 1.0f) * 0.5f;
		real32 mult = 0.25f / big;

		switch (index)
		{
		case 0: {
			real32 x = (m23 - m32) * mult;
			real32 y = (m31 - m13) * mult;
			real32 z = (m12 - m21) * mult;
			real32 w = big;
			return Quatf(x, y, z, w);
		}
		case 1: {
			real32 x = big;
			real32 y = (m12 + m21) * mult;
			real32 z = (m31 + m13) * mult;
			real32 w = (m23 - m32) * mult;
			return Quatf(x, y, z, w);
		}
		case 2: {
			real32 x = (m12 + m21) * mult;
			real32 y = big;
			real32 z = (m23 + m32) * mult;
			real32 w = (m31 - m13) * mult;
			return Quatf(x, y, z, w);
		}
		case 3: {
			real32 x = (m31 + m13) * mult;
			real32 y = (m23 + m32) * mult;
			real32 z = big;
			real32 w = (m12 - m21) * mult;
			return Quatf(x, y, z, w);
		}
		default: 
			Assert(0);
			return Quatf(0, 0, 0, 1);
		}
	}






	Mat4f Perspective(const real32 &dfovy, const real32 &aspect, const real32 &fnear, const real32 &ffar)
	{
		Mat4f p(1);
		real32 fovy = DegToRad(dfovy);
		real32 half_tan_fovy = tan(fovy / 2);
		p.row0 = Vec4f((1 / (aspect * half_tan_fovy)), 0, 0, 0);
		p.row1 = Vec4f(0, 1 / half_tan_fovy, 0, 0);

		real32 a = -(ffar + fnear) / (ffar - fnear);
		real32 b = (-2 * ffar * fnear) / (ffar - fnear);
		p.row2 = Vec4f(0, 0, a, -1);
		p.row3 = Vec4f(0, 0, b, 0);
		return p;
	}

	Mat4f Orthographic(const real32 &left, const real32 &right, const real32 &top, const real32 &bottom, const real32 &_near, const real32 &_far)
	{
		Mat4f result(1);
		result.row0 = Vec4f(2 / (right - left), 0, 0, 0);
		result.row1 = Vec4f(0, 2 / (top - bottom), 0, 0);
		result.row2 = Vec4f(0, 0, -2 / (_far - _near), 0);
		result.row3 = Vec4f(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(_far + _near) / (_far - _near), 1);
		return result;
	}

	Mat4f LookAt(const Vec3f &position, const Vec3f &target, const Vec3f &up)
	{
		Vec3f camera_reverse_direction = Normalize((target - position));

		Vec3f basis_right = Normalize(Cross(camera_reverse_direction, up));
		Vec3f basis_up = Cross(basis_right, camera_reverse_direction);

		Vec3f basis_forward = Normalize(Cross(basis_up, basis_right));

		// @NOTE: THIS IS CODE IS ACTUALLY COLUNM MAJOR
		//		: We just yeet it by transposing it at the end. Also care for order of multiplication order
		Mat4f rotation(1);

		rotation.row0 = Vec4f(basis_right, 0);
		rotation.row1 = Vec4f(basis_up, 0);
		rotation.row2 = Vec4f(camera_reverse_direction * -1.0f, 0);
		Mat4f translation(1);

		translation.row0 = Vec4f(1, 0, 0, -position.x);
		translation.row1 = Vec4f(0, 1, 0, -position.y);
		translation.row2 = Vec4f(0, 0, 1, -position.z);

		return Transpose(rotation * translation);
	}














	




	Mat3f QuatToMat3(const Quatf &q)
	{
		Mat3f mat(1);

		real32 qxx = (q.x * q.x);
		real32 qyy = (q.y * q.y);
		real32 qzz = (q.z * q.z);
		real32 qxz = (q.x * q.z);
		real32 qxy = (q.x * q.y);
		real32 qyz = (q.y * q.z);
		real32 qwx = (q.w * q.x);
		real32 qwy = (q.w * q.y);
		real32 qwz = (q.w * q.z);

		mat.row0.x = 1 - (2) * (qyy + qzz);
		mat.row0.y = (2) * (qxy + qwz);
		mat.row0.z = (2) * (qxz - qwy);

		mat.row1.x = (2) * (qxy - qwz);
		mat.row1.y = (1) - (2) * (qxx + qzz);
		mat.row1.z = (2) * (qyz + qwx);

		mat.row2.x = (2) * (qxz + qwy);
		mat.row2.y = (2) * (qyz - qwx);
		mat.row2.z = (1) - (2) * (qxx + qyy);
		return mat;
	}

	Mat4f QuatToMat4(const Quatf &q)
	{
		Mat4f mat(1);

		real32 qxx = (q.x * q.x);
		real32 qyy = (q.y * q.y);
		real32 qzz = (q.z * q.z);
		real32 qxz = (q.x * q.z);
		real32 qxy = (q.x * q.y);
		real32 qyz = (q.y * q.z);
		real32 qwx = (q.w * q.x);
		real32 qwy = (q.w * q.y);
		real32 qwz = (q.w * q.z);

		mat.row0.x = 1 - (2) * (qyy + qzz);
		mat.row0.y = (2) * (qxy + qwz);
		mat.row0.z = (2) * (qxz - qwy);

		mat.row1.x = (2) * (qxy - qwz);
		mat.row1.y = (1) - (2) * (qxx + qzz);
		mat.row1.z = (2) * (qyz + qwx);

		mat.row2.x = (2) * (qxz + qwy);
		mat.row2.y = (2) * (qyz - qwx);
		mat.row2.z = (1) - (2) * (qxx + qyy);
		return mat;
	}












	Vec4f GetNormalisedDeviceCoordinates(const real32 &window_width, const real32 &window_height, const real32 &mouse_x,
		const real32 &mouse_y)
	{
		// @NOTE: This is actualy clip space when the vec4 with -1 and 1
		return Vec4f(2 * mouse_x / window_width - 1, -(2 * mouse_y / window_height - 1), -1, 1);
	}

	Vec4f ToViewCoords(const Mat4f &projection_matrix, const Vec4f &viewCoords)
	{
		Mat4f invproj = Inverse(projection_matrix);
		return viewCoords * invproj;
	}

	Vec3f ToWorldCoords(const Mat4f &view_matrix, const Vec4f &viewCoords)
	{
		Mat4f invView = Inverse(view_matrix);
		Vec4f worldSpace = viewCoords * invView;
		return Vec3f(worldSpace.x, worldSpace.y, worldSpace.z);
	}

	Polar_coord Canonical(real32 r, real32 theta, real32 z)
	{
		theta = DegToRad(theta);
		if (r == 0)
		{
			theta = 0;
		}
		else
		{
			if (r < 0.f)
			{
				r = -(r);
				theta += static_cast<real32>(PI);
			}
			if (fabs(theta) > PI)
			{
				theta += static_cast<real32>(PI);
				theta -= static_cast<real32>(floor(theta / 2 * PI) * 2 * PI);
				theta -= static_cast<real32>(PI);
			}
		}
		return { r, theta, z };
	}
}