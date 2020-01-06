#pragma once
#include "CosmicMath.h"
//============ Orientation ============//
namespace cm
{
	Quat EulerToQuat(const Vec3 &euler_angle)
	{
		Vec3 c = Vec3(cos(deg_to_rad(euler_angle.x) / 2), cos(deg_to_rad(euler_angle.y) / 2), cos(deg_to_rad(euler_angle.z) / 2));
		Vec3 s = Vec3(sin(deg_to_rad(euler_angle.x) / 2), sin(deg_to_rad(euler_angle.y) / 2), sin(deg_to_rad(euler_angle.z) / 2));

		Quat q;

		q.x = s.x * c.y * c.z - c.x * s.y * s.z;
		q.y = c.x * s.y * c.z + s.x * c.y * s.z;
		q.z = c.x * c.y * s.z - s.x * s.y * c.z;
		q.w = c.x * c.y * c.z + s.x * s.y * s.z;
		return q;
	}


	Mat4 quat_to_mat4(const Quat &q)
	{
		//@Copy		
		Mat4 mat(1);

		float qxx = (q.x * q.x);
		float qyy = (q.y * q.y);
		float qzz = (q.z * q.z);
		float qxz = (q.x * q.z);
		float qxy = (q.x * q.y);
		float qyz = (q.y * q.z);
		float qwx = (q.w * q.x);
		float qwy = (q.w * q.y);
		float qwz = (q.w * q.z);

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

	Vec3 rotate(const float &d_angle, const Vec3 &point, const Vec3 &axis)
	{
		//@Speed, normalizing to be safe
		Vec3 ax = normalize(axis);

		float sh = sin(deg_to_rad(d_angle / 2));
		float ch = cos(deg_to_rad(d_angle / 2));

		Quat r(ax.x * sh,
			ax.y * sh,
			ax.z * sh,
			ch);

		Quat rc = conjugate(r);
		Quat pp = Quat(point, 0);

		Quat res = (r * pp) * rc;
		return res.vec;
	}

	Vec3 rotate(const Quat &r, const Vec3 &point)
	{
		//@Help: https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
		//@Speed, normalizing to be safe		
		Quat rc = conjugate(normalize(r));
		Quat pp = Quat(point, 0);
		Quat res = (r * pp) * rc;
		return res.vec;
	}

	Quat conjugate(const Quat &a)
	{
		return Quat(-a.x, -a.y, -a.z, a.w);
	}

	float mag(const Quat & a)
	{
		return sqrt(a.x* a.x + a.y * a.y + a.z * a.z + a.w * a.w);
	}

	Quat normalize(const Quat & a)
	{
		float m = mag(a);
		return Quat(a.x / m, a.y / m, a.z / m, a.w / m);
	}

	std::string toString(const Quat & a)
	{
		std::stringstream ss;
		ss << '(' << '{' << a.x << ", " << a.y << ", " << a.z << '}' << ' ' << a.w << ')';
		return ss.str();
	}
}