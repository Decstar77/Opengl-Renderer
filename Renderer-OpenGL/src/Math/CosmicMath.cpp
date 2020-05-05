#pragma once
#include "CosmicMath.h"
namespace cm
{	   	 
	real32 Mag(const Vec2 &a)
	{
		return sqrt(a.x*a.x + a.y*a.y);
	}

	Vec2 Normalize(const Vec2 &a)
	{
		real32 magA = Mag(a);
		return a / magA;
	}

	bool Equal(const Vec2 &a1, const Vec2 a2, const real32 &epsilon /*= FLOATING_POINT_ERROR_PRESCION*/)
	{
		real32 dx = Abs(a1.x - a2.x);
		real32 dy = Abs(a1.y - a2.y);
		return (dx < epsilon && dy < epsilon);
	}

	bool Equal(const Quat &q1, const Quat &q2, const real32 &epsilon /*= FLOATING_POINT_ERROR_PRESCION*/)
	{
		real32 dx = Abs(q1.x - q2.x);
		real32 dy = Abs(q1.y - q2.y);
		real32 dz = Abs(q1.z - q2.z);
		real32 dw = Abs(q1.w - q2.w);
		return (dx < epsilon && dy < epsilon && dz < epsilon && dw < epsilon);
	}

	real32 Dot(const Vec3 &a, const Vec3 &b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	real32 SqrdDistance(const Vec3 &a, const Vec3 &b)
	{
		return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z);
	}

	real32 Distance(const Vec3 &a, const Vec3 &b)
	{
		return Mag(a - b);
	}

	int32 AbsMaxIndex(const Vec3 &a)
	{
		real32 x = Abs(a.x);
		real32 y = Abs(a.y);
		real32 z = Abs(a.z);
		return (x > y) ? ((x > z) ? 0 : 2) : ((y > z) ? 1 : 2);
	}

	int32 MaxIndex(const Vec3 &a)
	{
		return (a.x > a.y) ? ((a.x > a.z) ? 0 : 2) : ((a.y > a.z) ? 1 : 2);
	}

	real32 Mag(const Vec3 &a)
	{
		return sqrt(a.x*a.x + a.y * a.y + a.z * a.z);
	}

	Vec3 Cross(const Vec3 &a, const Vec3 &b)
	{
		real32 x = a.y * b.z - b.y * a.z;
		real32 y = a.z * b.x - b.z * a.x;
		real32 z = a.x * b.y - b.x * a.y;
		return Vec3(x, y, z);
	}

	Vec3 Normalize(const Vec3 &a)
	{
		real32 magA = Mag(a);
		__m128 div = _mm_div_ps(a.data, _mm_set1_ps(magA));
		return Vec3(div);
	}

	Vec3 Vec4ToVec3(const Vec4 &a)
	{
		return Vec3(a.x, a.y, a.z);
	}

	bool CompareVec(const Vec3 &a, const Vec3 &b, const real &epsilon /*= FLOATING_POINT_ERROR_PRESCION*/)
	{		
		bool resx = requal(a.x, b.x, epsilon);
		bool resy = requal(a.y, b.y, epsilon);
		bool resz = requal(a.z, b.z, epsilon);
		return resx && resy && resz;
	}

	String ToString(const Vec3 &a)
	{
		StringStream ss;
		ss << '(' << a.x << ", " << a.y << ", " << a.z << ')';
		return ss.str();
	}

	String ToString(const Mat3 &a)
	{
		StringStream ss;
		String space = "            ";
		ss << "| " << a.ptr[0] << space << a.ptr[1] << space << a.ptr[2] << " |" << '\n';
		ss << "| " << a.ptr[4] << space << a.ptr[5] << space << a.ptr[6] << " |" << '\n';
		ss << "| " << a.ptr[8] << space << a.ptr[9] << space << a.ptr[10] << " |" << '\n';
		return ss.str();
	}

	real32 Dot(const Vec4 &a, const Vec4 &b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	real32 Mag(const Vec4 &a)
	{
		return sqrt(a.x*a.x + a.y * a.y + a.z * a.z + a.w *a.w);
	}

	Vec4 Normalize(const Vec4 &a)
	{
		real32 magA = Mag(a);
		if (magA != 1)
		{
			__m128 div = _mm_div_ps(a.data, _mm_set1_ps(magA));
			return Vec4(div);
		}
		return a;
	}

	Vec4 Vec3ToVec4(const Vec3 &a, const real32 &w)
	{
		return Vec4(a.x, a.y, a.z, w);
	}

	String ToString(const Vec4 &a)
	{
		StringStream ss;
		ss << '(' << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ')';
		return ss.str();

	}

	real32 Det(const Mat3 &a)
	{
		real32 f = a.ptr[0] * (a.ptr[5] * a.ptr[10] - a.ptr[9] * a.ptr[6]);
		real32 b = a.ptr[1] * (a.ptr[4] * a.ptr[10] - a.ptr[8] * a.ptr[6]);
		real32 c = a.ptr[2] * (a.ptr[4] * a.ptr[9] - a.ptr[8] * a.ptr[5]);

		return f - b + c;
	}

	Mat3 Mat4ToMat3(const Mat4 &a)
	{
		Mat3 result(1);
		result.row0 = Vec4ToVec3(a.row0);
		result.row1 = Vec4ToVec3(a.row1);
		result.row2 = Vec4ToVec3(a.row2);
		return result;
	}

	cm::Quat Mat3ToQuat(const Mat3 &a)
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
			return Quat(x, y, z, w);
		}
		case 1: {
			real32 x = big;
			real32 y = (m12 + m21) * mult;
			real32 z = (m31 + m13) * mult;
			real32 w = (m23 - m32) * mult;
			return Quat(x, y, z, w);
		}
		case 2: {
			real32 x = (m12 + m21) * mult;
			real32 y = big;
			real32 z = (m23 + m32) * mult;
			real32 w = (m31 - m13) * mult;
			return Quat(x, y, z, w);
		}
		case 3: {
			real32 x = (m31 + m13) * mult;
			real32 y = (m23 + m32) * mult;
			real32 z = big;
			real32 w = (m12 - m21) * mult;
			return Quat(x, y, z, w);
		}
		default:
			Assert(0);
			return Quat(0, 0, 0, 1);
		}
	}

	Mat4 CalculateTransformMatrix(const Vec3 & position, const Vec3 & scale, const Quat & rotation)
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
			std::cout << m.arr[i] << " ";
		}
		std::cout << std::endl;
	}
	
	void PrintPretty(const Mat4 &m)
	{
		std::cout << ToString(m) << std::endl;
	}

	real32 GetMatrixElement(const Mat4 &a, const int32 &row, const int32 &col)
	{
		return a.arr[4 * row + col];
	}

	real32 GetMatrixElement(const Mat3 &a, const int32 &row, const int32 &col)
	{
		return a.ptr[4 * row + col];
	}

	Vec4 GetColumn(const Mat4 &a, const uint32 &col)
	{
		Vec4 column(0, 0, 0, 0);
		column.x = a.arr[4 * 0 + col];
		column.y = a.arr[4 * 1 + col];
		column.z = a.arr[4 * 2 + col];
		column.w = a.arr[4 * 3 + col];
		return column;
	}

	Vec3 GetColumn(const Mat3 &a, const uint32 &col)
	{
		Vec3 column(0, 0, 0);
		column.x = a.ptr[4 * 0 + col];
		column.y = a.ptr[4 * 1 + col];
		column.z = a.ptr[4 * 2 + col];
		return column;
	}

	cm::Vec3 DecomposeToScale(const Mat3 &a)
	{
		real32 x = Mag(a.row0);
		real32 y = Mag(a.row1);
		real32 z = Mag(a.row2);
		Vec3 result = Vec3(x, y, z);
		return result;
	}

	cm::Mat3 DecomposeToRotationMatrix(const Mat3 &a)
	{
		Vec3 scale = DecomposeToScale(a);
		Mat3 result;

		for (int32 i = 0; i < 3; i++)
		{
			result[i] = a[i] / scale[i];
		}

		return result;
	}

	String ToString(const Mat4 &a)
	{
		StringStream ss;
		String space = "            ";
		ss << "| " << a.arr[0] << space << a.arr[1] << space << a.arr[2] << space << a.arr[3] << " |" << '\n';
		ss << "| " << a.arr[4] << space << a.arr[5] << space << a.arr[6] << space << a.arr[7] << " |" << '\n';
		ss << "| " << a.arr[8] << space << a.arr[9] << space << a.arr[10] << space << a.arr[11] << " |" << '\n';
		ss << "| " << a.arr[12] << space << a.arr[13] << space << a.arr[14] << space << a.arr[15] << " |" << '\n';
		return ss.str();
	}

	Mat3 Adjoint(const Mat4 &a, const int32 &row, const int32 &col)
	{
		Mat3 result(1);
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
		real32 f = a.arr[0] * (
			a.arr[5] * (a.arr[10] * a.arr[15] - a.arr[11] * a.arr[14]) +
			a.arr[6] * (a.arr[11] * a.arr[13] - a.arr[9] * a.arr[15]) +
			a.arr[7] * (a.arr[9] * a.arr[14] - a.arr[10] * a.arr[13]));

		real32 b = a.arr[1] * (
			a.arr[4] * (a.arr[10] * a.arr[15] - a.arr[11] * a.arr[14]) +
			a.arr[6] * (a.arr[11] * a.arr[12] - a.arr[8] * a.arr[15]) +
			a.arr[7] * (a.arr[8] * a.arr[14] - a.arr[10] * a.arr[12]));

		real32 c = a.arr[2] * (
			a.arr[4] * (a.arr[9] * a.arr[15] - a.arr[11] * a.arr[13]) +
			a.arr[5] * (a.arr[11] * a.arr[12] - a.arr[8] * a.arr[15]) +
			a.arr[7] * (a.arr[8] * a.arr[13] - a.arr[9] * a.arr[12]));

		real32 d = a.arr[3] * (
			a.arr[4] * (a.arr[9] * a.arr[14] - a.arr[10] * a.arr[13]) +
			a.arr[5] * (a.arr[10] * a.arr[12] - a.arr[8] * a.arr[14]) +
			a.arr[6] * (a.arr[8] * a.arr[13] - a.arr[9] * a.arr[12]));

		return f - b + c - d;
	}

	Mat4 Mat3ToMat4(const Mat3 &a, const Vec4 &b)
	{
		Mat4 result(1);
		result.row0 = Vec3ToVec4(a.row0, 0);
		result.row1 = Vec3ToVec4(a.row1, 0);
		result.row2 = Vec3ToVec4(a.row2, 0);
		result.row3 = b;
		return result;
	}

	bool CheckOrthogonal(const Mat4 &a, const real32 tolerance)
	{
		Mat4 result = a * Transpose(a);
		for (int32 i = 0; i < 4; i++)
		{
			if (abs(1 - abs(result.arr[i * 5])) > tolerance) {
				return false;
			}
		}
		return true;
	}

	Mat4 Transpose(Mat4 a)
	{
		_MM_TRANSPOSE4_PS(a.row0.data, a.row1.data, a.row2.data, a.row3.data);
		return a;
	}

	Mat4 Inverse(const Mat4 &a)
	{
		if (CheckOrthogonal(a))
		{
			return Transpose(a);
		}
		Mat4 result(1);
		Mat3 ad(1);
		int index = 0;
		for (int32 row = 0; row < 4; row++)
		{
			for (int32 col = 0; col < 4; col++)
			{
				if ((row + col) % 2)
				{
					ad = Adjoint(a, row, col);
					real32 i = -Det(ad);
					result.arr[index++] = i;
				}
				else
				{
					ad = Adjoint(a, row, col);
					real32 i = Det(ad);
					result.arr[index++] = i;
				}
			}
		}

		result = Transpose(result);

		real32 determinant = Det(a);

		return result / determinant;

	}

	Mat4 Translate(Mat4 a, Vec3 translation)
	{
		a.row3 = Vec4(translation, 1) * a;
		return a;
	}

	Mat4 Translate(Mat4 a, real32 length, real32 d_angle, real32 z)
	{
		Polar_coord p_coord = Canonical(length, d_angle, z);
		a.row3 = Vec4(p_coord.r * cosf(p_coord.theta), p_coord.r * sinf(p_coord.theta), p_coord.z, 1) * a;
		return a;
	}

	Mat4 Translate(Mat4 a, Polar_coord p_coord)
	{
		a.row3 = Vec4(p_coord.r * cosf(p_coord.theta), p_coord.r * sinf(p_coord.theta), p_coord.z, 1) * a;
		return a;
	}

	Mat4 Rotate(Mat4 a, real32 d_angle, Vec3 axis)
	{
		axis = Normalize(axis);
		real32 theata = DegToRad(d_angle);
		real32 cos_theata = cosf(theata);
		real32 sin_theata = sinf(theata);

		Vec4 iPrime(0, 0, 0, 0);
		iPrime.x = Round(axis.x *axis.x * (1 - cos_theata) + cos_theata);
		iPrime.y = Round(axis.x *axis.y * (1 - cos_theata) + axis.z * sin_theata);
		iPrime.z = Round(axis.x *axis.z * (1 - cos_theata) - axis.y * sin_theata);
		iPrime.w = 0;

		Vec4 jPrime(0, 0, 0, 0);
		jPrime.x = Round(axis.x *axis.y * (1 - cos_theata) - axis.z *sin_theata);
		jPrime.y = Round(axis.y *axis.y * (1 - cos_theata) + cos_theata);
		jPrime.z = Round(axis.y *axis.z * (1 - cos_theata) + axis.x *sin_theata);
		jPrime.w = 0;

		Vec4 kPrime(0, 0, 0, 0);
		kPrime.x = Round(axis.x *axis.z * (1 - cos_theata) + axis.y *sin_theata);
		kPrime.y = Round(axis.y *axis.z * (1 - cos_theata) - axis.x *sin_theata);
		kPrime.z = Round(axis.z *axis.z * (1 - cos_theata) + cos_theata);
		kPrime.w = 0;

		Vec4 wPrime(0, 0, 0, 1);

		Mat4 result(iPrime, jPrime, kPrime, wPrime);

		return result * a;
	}

	Mat3 Rotate(const Mat3 &a, const real32 &d_angle, Vec3 axis)
	{
		axis = Normalize(axis);
		
		real32 theata = DegToRad(d_angle);
		real32 cos_theata = cosf(theata);
		real32 sin_theata = sinf(theata);

		Vec3 iPrime(0, 0, 0);
		iPrime.x = Round(axis.x *axis.x * (1 - cos_theata) + cos_theata);
		iPrime.y = Round(axis.x *axis.y * (1 - cos_theata) + axis.z * sin_theata);
		iPrime.z = Round(axis.x *axis.z * (1 - cos_theata) - axis.y * sin_theata);

		Vec3 jPrime(0, 0, 0);
		jPrime.x = Round(axis.x *axis.y * (1 - cos_theata) - axis.z *sin_theata);
		jPrime.y = Round(axis.y *axis.y * (1 - cos_theata) + cos_theata);
		jPrime.z = Round(axis.y *axis.z * (1 - cos_theata) + axis.x *sin_theata);

		Vec3 kPrime(0, 0, 0);
		kPrime.x = Round(axis.x *axis.z * (1 - cos_theata) + axis.y *sin_theata);
		kPrime.y = Round(axis.y *axis.z * (1 - cos_theata) - axis.x *sin_theata);
		kPrime.z = Round(axis.z *axis.z * (1 - cos_theata) + cos_theata);
		
		Mat3 result(iPrime, jPrime, kPrime);

		return result * a;
	}

	Quat Rotate(const Quat &q, const real32 &d_angle, Vec3 axis)
	{
		// @HELP: https://stackoverflow.com/questions/4436764/rotating-a-quaternion-on-1-axis
		axis = Normalize(axis);

		real32 rangle = DegToRad(d_angle);
		
		real32 s = SafeTruncateDouble(sin(rangle / 2.0));
		real32 c = SafeTruncateDouble(cos(rangle / 2.0));

		real32 x = axis.x * s;
		real32 y = axis.y * s;
		real32 z = axis.z * s;
		real32 w = c;

		Quat result = Normalize(Quat(x, y, z, w));
		return result;
	}

	Quat Rotate(const Quat &a, const Quat &b)
	{
		// @NOTE: I always forget this, that's why I made this a function. Also more readable this way
		return a * b;
	}

	Quat Mat4ToQuat(const Mat4 &a)
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
			return Quat(x, y, z, w);
		}
		case 1: {
			real32 x = big;
			real32 y = (m12 + m21) * mult;
			real32 z = (m31 + m13) * mult;
			real32 w = (m23 - m32) * mult;
			return Quat(x, y, z, w);
		}
		case 2: {
			real32 x = (m12 + m21) * mult;
			real32 y = big;
			real32 z = (m23 + m32) * mult;
			real32 w = (m31 - m13) * mult;
			return Quat(x, y, z, w);
		}
		case 3: {
			real32 x = (m31 + m13) * mult;
			real32 y = (m23 + m32) * mult;
			real32 z = big;
			real32 w = (m12 - m21) * mult;
			return Quat(x, y, z, w);
		}
		default: 
			Assert(0);
			return Quat(0, 0, 0, 1);
		}
	}

	Mat4 ScaleDirection(Mat4 a, real32 k, Vec3 unit_direction, bool should_normalize)
	{
		if (should_normalize && Mag(unit_direction) != 1)
		{
			unit_direction = Normalize(unit_direction);
		}
		Vec4 iPrime(0, 0, 0, 0);
		iPrime.x = 1 + (k - 1) * unit_direction.x * unit_direction.x;
		iPrime.y = (k - 1) * unit_direction.x * unit_direction.y;
		iPrime.z = (k - 1) * unit_direction.x * unit_direction.z;

		Vec4 jPrime(0, 0, 0, 0);
		jPrime.x = (k - 1) * unit_direction.x * unit_direction.y;
		jPrime.y = 1 + (k - 1) * unit_direction.y * unit_direction.y;
		jPrime.z = (k - 1) * unit_direction.y * unit_direction.z;

		Vec4 kPrime(0, 0, 0, 0);
		kPrime.x = (k - 1) * unit_direction.x * unit_direction.z;
		kPrime.y = (k - 1) * unit_direction.y * unit_direction.z;
		kPrime.z = 1 + (k - 1) * unit_direction.z * unit_direction.z;

		Vec4 wPrime(0, 0, 0, 1);

		Mat4 result(iPrime, jPrime, kPrime, wPrime);
		return result * a;
	}

	cm::Mat3 ScaleDirection(const Mat3 &a, const real32 &k, Vec3 direction)
	{
		direction = Normalize(direction);

		Vec3 i_prime(0, 0, 0);
		i_prime.x = 1 + (k - 1) * direction.x * direction.x;
		i_prime.y = (k - 1) * direction.x * direction.y;
		i_prime.z = (k - 1) * direction.x * direction.z;

		Vec3 j_prime(0, 0, 0);
		j_prime.x = (k - 1) * direction.x * direction.y;
		j_prime.y = 1 + (k - 1) * direction.y * direction.y;
		j_prime.z = (k - 1) * direction.y * direction.z;

		Vec3 k_prime(0, 0, 0);
		k_prime.x = (k - 1) * direction.x * direction.z;
		k_prime.y = (k - 1) * direction.y * direction.z;
		k_prime.z = 1 + (k - 1) * direction.z * direction.z;

		Mat3 result(i_prime, j_prime, k_prime);
		return result;
	}

	Mat4 ScaleCardinal(Mat4 a, Vec3 direction)
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
		p.row0 = Vec4((1 / (aspect * half_tan_fovy)), 0, 0, 0);
		p.row1 = Vec4(0, 1 / half_tan_fovy, 0, 0);

		real32 a = -(ffar + fnear) / (ffar - fnear);
		real32 b = (-2 * ffar * fnear) / (ffar - fnear);
		p.row2 = Vec4(0, 0, a, -1);
		p.row3 = Vec4(0, 0, b, 0);
		return p;
	}

	Mat4 Orthographic(const real32 &left, const real32 &right, const real32 &top, const real32 &bottom, const real32 &_near, const real32 &_far)
	{
		Mat4 result(1);
		result.row0 = Vec4(2 / (right - left), 0, 0, 0);
		result.row1 = Vec4(0, 2 / (top - bottom), 0, 0);
		result.row2 = Vec4(0, 0, -2 / (_far - _near), 0);
		result.row3 = Vec4(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(_far + _near) / (_far - _near), 1);
		return result;
	}

	Mat4 LookAt(const Vec3 &position, const Vec3 &target, const Vec3 &up)
	{
		Vec3 camera_reverse_direction = Normalize((target - position));

		Vec3 basis_right = Normalize(Cross(camera_reverse_direction, up));
		Vec3 basis_up = Cross(basis_right, camera_reverse_direction);

		Vec3 basis_forward = Normalize(Cross(basis_up, basis_right));

		// @NOTE: THIS IS CODE IS ACTUALLY COLUNM MAJOR
		//		: We just yeet it by transposing it at the end. Also care for order of multiplication order
		Mat4 rotation(1);

		rotation.row0 = Vec4(basis_right, 0);
		rotation.row1 = Vec4(basis_up, 0);
		rotation.row2 = Vec4(camera_reverse_direction * -1, 0);
		Mat4 translation(1);

		translation.row0 = Vec4(1, 0, 0, -position.x);
		translation.row1 = Vec4(0, 1, 0, -position.y);
		translation.row2 = Vec4(0, 0, 1, -position.z);

		return Transpose(rotation * translation);
	}

	Mat3 operator*(const Mat3 &a, const Mat3 &b)
	{
		Mat3 result(1);
		// @NOTE: Steps through the rows
		for (int32 i = 0; i < 3; i++)
		{
			// @NOTE: Steps through the columns
			for (int32 y = 0; y < 3; y++)
			{
				// @NOTE: Gets the column vector of the right hand side
				Vec3 col(0, 0, 0);
				for (int32 x = 0; x < 3; x++)
				{
					col[x] = GetMatrixElement(b, x, y);
				}
				// @NOTE: Adds to result
				result.ptr[4 * i + y] = Dot(col, a[i]);
			}
		}
		return result;
	}

	Vec3 operator*(const Vec3 &a, const Mat3 &b)
	{
		Vec3 result(0, 0, 0);
		for (uint32 i = 0; i < 3; i++)
		{
			Vec3 col = GetColumn(b, i);
			result[i] = Dot(col, a);
		}
		return result;
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
				Vec4 col(0, 0, 0, 0);
				for (int32 x = 0; x < 4; x++)
				{
					col[x] = GetMatrixElement(b, x, y);
				}
				// @NOTE: Adds to result
				// @NOTE: This is 4 * because of the padding byte
				result.arr[4 * i + y] = Dot(col, a.data[i]);
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

	Vec4 operator *(const Vec4 &a, const Mat4 &b)
	{
		Vec4 result(0, 0, 0, 0);
		for (uint32 i = 0; i < 4; i++)
		{
			Vec4 col = GetColumn(b, i);
			result[i] = Dot(col, a);
		}
		return result;
	}

	Quat EulerToQuat(const Vec3 &euler_angle)
	{
		Vec3 c = Vec3(cos(DegToRad(euler_angle.x) / 2.0f), cos(DegToRad(euler_angle.y) / 2.0f), cos(DegToRad(euler_angle.z) / 2.0f));
		Vec3 s = Vec3(sin(DegToRad(euler_angle.x) / 2.0f), sin(DegToRad(euler_angle.y) / 2.0f), sin(DegToRad(euler_angle.z) / 2.0f));

		Quat q;
		q.x = s.x * c.y * c.z - c.x * s.y * s.z;
		q.y = c.x * s.y * c.z + s.x * c.y * s.z;
		q.z = c.x * c.y * s.z - s.x * s.y * c.z;
		q.w = c.x * c.y * c.z + s.x * s.y * s.z;		

		return q;
	}
	
	cm::Vec3 QuatToEuler(const Quat &q)
	{
		// @HELP: Glm and Math book
		Vec3 euler;
		Vec2 sp;
		sp.x = 2.f * (q.y * q.z + q.w * q.x);
		sp.y = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;
				
		if (Equal(Vec2(sp.x, sp.y), Vec2(0)))
		{
			euler.x = 2.0f * atan2(q.w, q.x);
		}
		else
		{
			euler.x = atan2(sp.x, sp.y);
		}
		
		euler.y = asin( Clamp(-2.0f * (q.x * q.z - q.w * q.y), -1.0, 1.0f) );
		euler.z = atan2( 2.0f * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z );

		euler.x = RadToDeg(euler.x);
		euler.y = RadToDeg(euler.y);
		euler.z = RadToDeg(euler.z);

		return euler;
	}

	Quat Slerp(const Quat &a, const Quat &b, const real32 &t)
	{
		Quat an = Normalize(a);
		Quat bn = Normalize(b);

		real32 d = an.x * bn.x + an.y * bn.y + an.z * bn.z + an.w * bn.w;
		real32 tinv = 1.0f - t;
		int32 ds = Sign(d);

		Quat result;		
		result.x = an.x * tinv + ds * t * bn.x;
		result.y = an.y * tinv + ds * t * bn.y;
		result.z = an.z * tinv + ds * t * bn.z;
		result.w = an.w * tinv + ds * t * bn.w;
		
		return Normalize(result);
	}

	Mat3 QuatToMat3(const Quat &q)
	{
		Mat3 mat(1);

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

	Mat4 QuatToMat4(const Quat &q)
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

	Vec3 Rotate(const real32 &d_angle, const Vec3 &point, const Vec3 &axis)
	{
		Vec3 ax = Normalize(axis);

		real32 sh = sin(DegToRad(d_angle / 2));
		real32 ch = cos(DegToRad(d_angle / 2));

		Quat r(ax.x * sh,
			ax.y * sh,
			ax.z * sh,
			ch);

		Quat rc = Conjugate(r);
		Quat pp = Quat(point, 0);

		Quat res = (r * pp) * rc;
		return Vec3(res.x, res.y, res.z);
	}

	Vec3 Rotate(const Quat &r, const Vec3 &point)
	{
		//@Help: https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
		Quat rc = Conjugate(Normalize(r));
		Quat pp = Quat(point, 0);
		Quat res = (r * pp) * rc;
		return Vec3(res.x, res.y, res.z);
	}

	Quat Conjugate(const Quat &a)
	{
		return Quat(-a.x, -a.y, -a.z, a.w);
	}

	real32 Mag(const Quat & a)
	{
		return sqrt(a.x* a.x + a.y * a.y + a.z * a.z + a.w * a.w);
	}

	Quat Normalize(const Quat & a)
	{
		real32 m = Mag(a);
		return Quat(a.x / m, a.y / m, a.z / m, a.w / m);
	}

	String ToString(const Quat & a)
	{
		StringStream ss;
		ss << '(' << '{' << a.x << ", " << a.y << ", " << a.z << '}' << ' ' << a.w << ')';
		return ss.str();
	}

	Vec4 GetNormalisedDeviceCoordinates(const real32 &window_width, const real32 &window_height, const real32 &mouse_x,
		const real32 &mouse_y)
	{
		// @NOTE: This is actualy clip space when the vec4 with -1 and 1
		return Vec4(2 * mouse_x / window_width - 1, -(2 * mouse_y / window_height - 1), -1, 1);
	}

	Vec4 ToViewCoords(const Mat4 &projection_matrix, const Vec4 &viewCoords)
	{
		Mat4 invproj = Inverse(projection_matrix);
		return viewCoords * invproj;
	}

	Vec3 ToWorldCoords(const Mat4 &view_matrix, const Vec4 &viewCoords)
	{
		Mat4 invView = Inverse(view_matrix);
		Vec4 worldSpace = viewCoords * invView;
		return Vec3(worldSpace.x, worldSpace.y, worldSpace.z);
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