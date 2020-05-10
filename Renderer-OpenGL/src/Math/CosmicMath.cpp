#pragma once
#include "CosmicMath.h"
namespace cm
{	   	 


	   





















	Mat3f Mat4ToMat3(const Mat4 &a)
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

	Mat4 CalculateTransformMatrix(const Vec3f & position, const Vec3f & scale, const Quatf & rotation)
	{
		// @HELP: Real time rendering book
		Mat4 trans(1);
		Mat4 rot(1);
		Mat4 scl(1);
		trans = Translate(trans, position);	
		rot = QuatToMat4(Conjugate(rotation));
		scl= ScaleCardinal(scl, scale);
		return scl * rot * trans;
	}

	void Print(const Mat4 &m)
	{
		for (int32 i = 0; i < 16; i++)
		{
			std::cout << m.ptr[i] << " ";
		}
		std::cout << std::endl;
	}
	
	void PrintPretty(const Mat4 &m)
	{
		std::cout << ToString(m) << std::endl;
	}

	real32 GetMatrixElement(const Mat4 &a, const int32 &row, const int32 &col)
	{
		return a.ptr[4 * row + col];
	}



	Vec4f GetColumn(const Mat4 &a, const uint32 &col)
	{
		Vec4f column(0, 0, 0, 0);
		column.x = a.ptr[4 * 0 + col];
		column.y = a.ptr[4 * 1 + col];
		column.z = a.ptr[4 * 2 + col];
		column.w = a.ptr[4 * 3 + col];
		return column;
	}







	String ToString(const Mat4 &a)
	{
		StringStream ss;
		String space = "            ";
		ss << "| " << a.ptr[0] << space << a.ptr[1] << space << a.ptr[2] << space << a.ptr[3] << " |" << '\n';
		ss << "| " << a.ptr[4] << space << a.ptr[5] << space << a.ptr[6] << space << a.ptr[7] << " |" << '\n';
		ss << "| " << a.ptr[8] << space << a.ptr[9] << space << a.ptr[10] << space << a.ptr[11] << " |" << '\n';
		ss << "| " << a.ptr[12] << space << a.ptr[13] << space << a.ptr[14] << space << a.ptr[15] << " |" << '\n';
		return ss.str();
	}

	Mat3f Adjoint(const Mat4 &a, const int32 &row, const int32 &col)
	{
		Mat3f result(1);
		int32 index = 0;
		// @SPEEDS: To many branches
		for (int32 r = 0; r < 4; r++)
		{
			if (row == r)
			{
				continue;
			}
			for (int32 c = 0; c < 4; c++)
			{
				if (c == col || c == col + 4 || c == col + 8 || c == col + 12)
				{
					continue;
				}
				// @NOTE: REMEMBER THE PADDING BYTE!!
				if (index == 3 || index == 7)
				{
					index++;
				}
				result.ptr[index++] = GetMatrixElement(a, r, c);	
			}
		}
		return result;
	}

	real32 Det(const Mat4 &a)
	{
		real32 f = a.ptr[0] * (
			a.ptr[5] * (a.ptr[10] * a.ptr[15] - a.ptr[11] * a.ptr[14]) +
			a.ptr[6] * (a.ptr[11] * a.ptr[13] - a.ptr[9] * a.ptr[15]) +
			a.ptr[7] * (a.ptr[9] * a.ptr[14] - a.ptr[10] * a.ptr[13]));

		real32 b = a.ptr[1] * (
			a.ptr[4] * (a.ptr[10] * a.ptr[15] - a.ptr[11] * a.ptr[14]) +
			a.ptr[6] * (a.ptr[11] * a.ptr[12] - a.ptr[8] * a.ptr[15]) +
			a.ptr[7] * (a.ptr[8] * a.ptr[14] - a.ptr[10] * a.ptr[12]));

		real32 c = a.ptr[2] * (
			a.ptr[4] * (a.ptr[9] * a.ptr[15] - a.ptr[11] * a.ptr[13]) +
			a.ptr[5] * (a.ptr[11] * a.ptr[12] - a.ptr[8] * a.ptr[15]) +
			a.ptr[7] * (a.ptr[8] * a.ptr[13] - a.ptr[9] * a.ptr[12]));

		real32 d = a.ptr[3] * (
			a.ptr[4] * (a.ptr[9] * a.ptr[14] - a.ptr[10] * a.ptr[13]) +
			a.ptr[5] * (a.ptr[10] * a.ptr[12] - a.ptr[8] * a.ptr[14]) +
			a.ptr[6] * (a.ptr[8] * a.ptr[13] - a.ptr[9] * a.ptr[12]));

		return f - b + c - d;
	}

	Mat4 Mat3ToMat4(const Mat3f &a, const Vec4f &b)
	{
		Mat4 result(1);
		result.row0 = Vec4f(a.row0, 0);
		result.row1 = Vec4f(a.row1, 0);
		result.row2 = Vec4f(a.row2, 0);
		result.row3 = b;
		return result;
	}

	bool CheckOrthogonal(const Mat4 &a, const real32 tolerance)
	{
		Mat4 result = a * Transpose(a);
		for (int32 i = 0; i < 4; i++)
		{
			if (abs(1 - abs(result.ptr[i * 5])) > tolerance) {
				return false;
			}
		}
		return true;
	}

	Mat4 Transpose(Mat4 a)
	{
		Vec4f c0 = GetColumn(a, 0);
		Vec4f c1 = GetColumn(a, 1);
		Vec4f c2 = GetColumn(a, 2);
		Vec4f c3 = GetColumn(a, 3);

		Mat4 result(c0, c1, c2, c3);

		return result;
	}

	Mat4 Inverse(const Mat4 &a)
	{
		if (CheckOrthogonal(a))
		{
			return Transpose(a);
		}
		Mat4 result(1);
		Mat3f ad(1);
		int index = 0;
		for (int32 row = 0; row < 4; row++)
		{
			for (int32 col = 0; col < 4; col++)
			{
				if ((row + col) % 2)
				{
					ad = Adjoint(a, row, col);
					real32 i = -Det(ad);
					result.ptr[index++] = i;
				}
				else
				{
					ad = Adjoint(a, row, col);
					real32 i = Det(ad);
					result.ptr[index++] = i;
				}
			}
		}

		result = Transpose(result);

		real32 determinant = Det(a);

		return result / determinant;

	}

	Mat4 Translate(Mat4 a, Vec3f translation)
	{
		a.row3 = Vec4f(translation, 1) * a;
		return a;
	}

	Mat4 Translate(Mat4 a, real32 length, real32 d_angle, real32 z)
	{
		Polar_coord p_coord = Canonical(length, d_angle, z);
		a.row3 = Vec4f(p_coord.r * cosf(p_coord.theta), p_coord.r * sinf(p_coord.theta), p_coord.z, 1) * a;
		return a;
	}

	Mat4 Translate(Mat4 a, Polar_coord p_coord)
	{
		a.row3 = Vec4f(p_coord.r * cosf(p_coord.theta), p_coord.r * sinf(p_coord.theta), p_coord.z, 1) * a;
		return a;
	}

	Mat4 Rotate(Mat4 a, real32 d_angle, Vec3f axis)
	{
		axis = Normalize(axis);
		real32 theata = DegToRad(d_angle);
		real32 cos_theata = cosf(theata);
		real32 sin_theata = sinf(theata);

		Vec4f iPrime(0, 0, 0, 0);
		iPrime.x = Round(axis.x *axis.x * (1 - cos_theata) + cos_theata);
		iPrime.y = Round(axis.x *axis.y * (1 - cos_theata) + axis.z * sin_theata);
		iPrime.z = Round(axis.x *axis.z * (1 - cos_theata) - axis.y * sin_theata);
		iPrime.w = 0;

		Vec4f jPrime(0, 0, 0, 0);
		jPrime.x = Round(axis.x *axis.y * (1 - cos_theata) - axis.z *sin_theata);
		jPrime.y = Round(axis.y *axis.y * (1 - cos_theata) + cos_theata);
		jPrime.z = Round(axis.y *axis.z * (1 - cos_theata) + axis.x *sin_theata);
		jPrime.w = 0;

		Vec4f kPrime(0, 0, 0, 0);
		kPrime.x = Round(axis.x *axis.z * (1 - cos_theata) + axis.y *sin_theata);
		kPrime.y = Round(axis.y *axis.z * (1 - cos_theata) - axis.x *sin_theata);
		kPrime.z = Round(axis.z *axis.z * (1 - cos_theata) + cos_theata);
		kPrime.w = 0;

		Vec4f wPrime(0, 0, 0, 1);

		Mat4 result(iPrime, jPrime, kPrime, wPrime);

		return result * a;
	}






	Quatf Mat4ToQuat(const Mat4 &a)
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

	Mat4 ScaleDirection(Mat4 a, real32 k, Vec3f unit_direction, bool should_normalize)
	{
		if (should_normalize && Mag(unit_direction) != 1)
		{
			unit_direction = Normalize(unit_direction);
		}
		Vec4f iPrime(0, 0, 0, 0);
		iPrime.x = 1 + (k - 1) * unit_direction.x * unit_direction.x;
		iPrime.y = (k - 1) * unit_direction.x * unit_direction.y;
		iPrime.z = (k - 1) * unit_direction.x * unit_direction.z;

		Vec4f jPrime(0, 0, 0, 0);
		jPrime.x = (k - 1) * unit_direction.x * unit_direction.y;
		jPrime.y = 1 + (k - 1) * unit_direction.y * unit_direction.y;
		jPrime.z = (k - 1) * unit_direction.y * unit_direction.z;

		Vec4f kPrime(0, 0, 0, 0);
		kPrime.x = (k - 1) * unit_direction.x * unit_direction.z;
		kPrime.y = (k - 1) * unit_direction.y * unit_direction.z;
		kPrime.z = 1 + (k - 1) * unit_direction.z * unit_direction.z;

		Vec4f wPrime(0, 0, 0, 1);

		Mat4 result(iPrime, jPrime, kPrime, wPrime);
		return result * a;
	}



	Mat4 ScaleCardinal(Mat4 a, Vec3f direction)
	{
		a.row0 = a.row0 * direction.x;
		a.row1 = a.row1 * direction.y;
		a.row2 = a.row2 * direction.z;
		return a;
	}

	Mat4 Perspective(const real32 &dfovy, const real32 &aspect, const real32 &fnear, const real32 &ffar)
	{
		Mat4 p(1);
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

	Mat4 Orthographic(const real32 &left, const real32 &right, const real32 &top, const real32 &bottom, const real32 &_near, const real32 &_far)
	{
		Mat4 result(1);
		result.row0 = Vec4f(2 / (right - left), 0, 0, 0);
		result.row1 = Vec4f(0, 2 / (top - bottom), 0, 0);
		result.row2 = Vec4f(0, 0, -2 / (_far - _near), 0);
		result.row3 = Vec4f(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(_far + _near) / (_far - _near), 1);
		return result;
	}

	Mat4 LookAt(const Vec3f &position, const Vec3f &target, const Vec3f &up)
	{
		Vec3f camera_reverse_direction = Normalize((target - position));

		Vec3f basis_right = Normalize(Cross(camera_reverse_direction, up));
		Vec3f basis_up = Cross(basis_right, camera_reverse_direction);

		Vec3f basis_forward = Normalize(Cross(basis_up, basis_right));

		// @NOTE: THIS IS CODE IS ACTUALLY COLUNM MAJOR
		//		: We just yeet it by transposing it at the end. Also care for order of multiplication order
		Mat4 rotation(1);

		rotation.row0 = Vec4f(basis_right, 0);
		rotation.row1 = Vec4f(basis_up, 0);
		rotation.row2 = Vec4f(camera_reverse_direction * -1.0f, 0);
		Mat4 translation(1);

		translation.row0 = Vec4f(1, 0, 0, -position.x);
		translation.row1 = Vec4f(0, 1, 0, -position.y);
		translation.row2 = Vec4f(0, 0, 1, -position.z);

		return Transpose(rotation * translation);
	}





	Mat4 operator /(Mat4 a, real32 b)
	{
		a.row0 = a.row0 / b;
		a.row1 = a.row1 / b;
		a.row2 = a.row2 / b;
		a.row3 = a.row3 / b;
		return a;
	}

	Mat4 operator *(const Mat4 &a, const Mat4 &b)
	{
		Mat4 result(1);
		// @NOTE: Steps through the rows
		for (int32 i = 0; i < 4; i++)
		{
			// @NOTE: Steps through the columns
			for (int32 y = 0; y < 4; y++)
			{
				// @NOTE: Gets the column vector of the right hand side
				Vec4f col(0, 0, 0, 0);
				for (int32 x = 0; x < 4; x++)
				{
					col[x] = GetMatrixElement(b, x, y);
				}
				// @NOTE: Adds to result
				// @NOTE: This is 4 * because of the padding byte
				result.ptr[4 * i + y] = Dot(col, a[i]);
			}
		}
		return result;
	}

	Mat4 operator+(const Mat4 & a, const Mat4 & b)
	{
		Mat4 result(1);
		result.row0 = a.row0 + b.row0;
		result.row1 = a.row1 + b.row1;
		result.row2 = a.row2 + b.row2;
		result.row3 = a.row3 + b.row3;
		return result;
	}

	Vec4f operator *(const Vec4f &a, const Mat4 &b)
	{
		Vec4f result(0, 0, 0, 0);
		for (uint32 i = 0; i < 4; i++)
		{
			Vec4f col = GetColumn(b, i);
			result[i] = Dot(col, a);
		}
		return result;
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

	Mat4 QuatToMat4(const Quatf &q)
	{
		Mat4 mat(1);

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

	Vec4f ToViewCoords(const Mat4 &projection_matrix, const Vec4f &viewCoords)
	{
		Mat4 invproj = Inverse(projection_matrix);
		return viewCoords * invproj;
	}

	Vec3f ToWorldCoords(const Mat4 &view_matrix, const Vec4f &viewCoords)
	{
		Mat4 invView = Inverse(view_matrix);
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