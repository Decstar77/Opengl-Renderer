#pragma once
#include "../Core.h"
#include <math.h>
#include <random> 
#include <intrin.h>
#include <xmmintrin.h>

//============       NOTES       ============//

//TODO all matrices and vectors are floats/real32
//TODO check_orthogonal could be faster
//TODO SIMD more ??
//TODO Vectors still contain more data than need (vec2)
//============     Constants     ============//



namespace cm
{	
	//************************************
	// Constants
	//************************************

	#define PI 3.14159265359

	//************************************
	// Enums 
	//************************************

	enum class Space
	{
		None = 0, Object, World, View, Projection
	};

	inline constexpr uint32 SafeTruncateUint64(uint64 value)
	{
		Assert(value <= 0xFFFFFFFF);
		uint32 result = static_cast<uint32>(value);
		return result;
	}

	inline constexpr real32 SafeTruncateDouble(real64 value)
	{
		Assert(value <= 0xFFFFFFFF);
		real32 result = static_cast<real32>(value);
		return result;
	}

	template<typename T>
	inline constexpr T Abs(const T &a)
	{
		return std::abs(a);
	}

	template<typename T>
	inline constexpr T RandomUnillateral() // @NOTE: 0 to 1
	{
		T res = (T)rand() / (T)RAND_MAX;
		return res;
	}

	template<typename T>
	inline constexpr T RandomBillateral() // @NOTE: -1 to 1
	{
		T res = static_cast<T>(2.0) * RandomUnillateral<T>() - static_cast<T>(1.0);
		return res;
	}

	template<typename T>
	inline constexpr T RandomReal(const T &min, const T &max) // @NOTE: min to max - 1
	{
		T c = max - min;
		return  c == 0 ? min : min + static_cast <T> (rand()) / (static_cast <T> (RAND_MAX / c));
	}

	template<typename T>
	inline constexpr T RandomUInt(const T &min, const T &max) // @NOTE: min to max - 1
	{
		T c = max - min;
		return c == 0 ? min : rand() % (max - min) + min;
	}

	template<typename T>
	inline constexpr T Normalize(const T &value, const T &min, const T &max)
	{
		return (value - min) / (max - min);
	}

	template<typename T>
	inline constexpr T Lerp(const T &a, const T &b, const T &t)
	{
		return a * (static_cast<T>(1.0) - t) + b * t;
	}
	
	template<typename T>
	inline constexpr T DegToRad(const T &degrees)
	{
		return (static_cast<T>(PI) * degrees) / static_cast<T>(180.0);
	}

	template<typename T>
	inline constexpr T RadToDeg(const T &radians)
	{
		return (static_cast<T>(180.0) * radians) / static_cast<T>(PI);
	}

	template<typename T>
	inline constexpr T Sin(const T &rads)
	{
		return std::sin(rads);
	}

	template<typename T>
	inline constexpr T Asin(const T &rads)
	{
		return std::asin(rads);
	}

	template<typename T>
	inline constexpr T Cos(const T &rads)
	{
		return std::cos(rads);
	}

	template<typename T>
	inline constexpr T Acos(const T &rads)
	{
		return std::acos(rads);
	}
	
	template<typename T>
	inline constexpr T Tan(const T &rads)
	{
		return std::tan(rads);
	}

	template<typename T>
	inline constexpr T Tan(const T &y, const T &x)
	{
		return std::atan2(y, x);
	}

	template<typename T>
	inline constexpr T Atan(const T &rads)
	{
		return std::atan(rads);
	}

	template<typename T>
	inline constexpr T Atan(const T &y, const T &x)
	{
		return std::atan2(y, x);
	}

	template<typename T>
	inline constexpr T Round(const T &val)
	{
		return std::round(val * static_cast<T>(1000000.0)) / static_cast<T>(1000000.0f);
	}

	template<typename T>
	inline constexpr T Floor(const T &val)
	{
		return std::floor(val);
	}

	template<typename T>
	inline constexpr T Ceil(const T &val)
	{
		return std::ceil(val);
	}

	template<typename T>
	inline constexpr int32 Sign(const T &val)
	{
		return (static_cast<T>(0.0) < val) - (val < static_cast<T>(0));
	}

	template<typename T>
	inline constexpr T Sqrt(const T &val)
	{
		return std::sqrt(val);
	}

	template<typename T>
	inline constexpr void Swap(T *a1, T *a2)
	{
		T temp = *a1;
		*a1 = *a2;
		*a2 = temp;
	}

	template<typename T>
	inline constexpr T Min(const T &f)
	{
		return f;
	}

	template<typename T, typename... Args>
	inline constexpr T Min(const T &f, const Args&... args)
	{
		T min = Min(args...);
		return (f < min) ? f : min;
	}

	template<typename T>
	inline constexpr T Max(const T &f)
	{
		return f;
	}

	template<typename T, typename... Args>
	inline constexpr T Max(const T &f, const Args&... args)
	{
		T max = Max(args...);
		return (f > max) ? f : max;
	}

	template<typename T>
	inline constexpr T Clamp(const T &value, const T &lowerBound, const T &upperBound)
	{
		return std::clamp(value, lowerBound, upperBound);
	}

	template<typename T>
	inline constexpr bool32 Equal(const T &a, const T &b, const T &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return (Abs(a - b) < epsilon) ? true : false;
	}

	template<typename T>
	inline constexpr bool32 Nequal(const T &a, const T &b, const T &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return !Equal(a, b);
	}
	
	//************************************
	// Math structures
	//************************************
	
	//************************************
	// Vector2 
	//************************************

	template<typename T>
	struct Vec2;

	template<typename T>
	struct Vec3;

	template<typename T>
	struct Vec4;

	template<typename T>
	struct Quat;

	struct Mat3;
	struct Mat4;

	template<typename T>
	struct Vec2
	{
		T x;
		T y;

		Vec2()
		{
			x = 0; y = 0;
		}
		
		Vec2(const T &x, const T &y)
		{
			this->x = x; this->y = y;
		}
		
		Vec2(const T &all)
		{
			x = all;
			y = all;
		}

		T& operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 2);
			return (&x)[index];
		}

		T operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 2);
			return (&x)[index];
		}
	};

	template<typename T>
	T Mag(const Vec2<T> &a)
	{
		return std::sqrt(a.x * a.x + a.y * a.y);
	}

	template<typename T>
	T Normalize(const Vec2<T> &a)
	{
		T magA = Mag(a);
		return a / magA;
	}

	template<typename T>
	bool Equal(const Vec2<T> &a1, const Vec2<T> a2, const real32 &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		T dx = Abs(a1.x - a2.x);
		T dy = Abs(a1.y - a2.y);
		return (dx < epsilon && dy < epsilon);
	}

	template<typename T>
	inline Vec2<T> operator +(const Vec2<T> &a, const Vec2<T> &b)
	{
		return Vec2(a.x + b.x, a.y + b.y);
	}
	
	template<typename T>
	inline Vec2<T> operator -(const Vec2<T> &a, const Vec2<T> &b)
	{
		return Vec2(a.x - b.x, a.y - b.y);
	}

	template<typename T>
	inline Vec2<T> operator /(const Vec2<T> &a, T b)
	{
		return Vec2(a.x / b, a.y / b);
	}

	template<typename T>
	inline bool32 operator  ==(const Vec2<T> &a, const Vec2<T> &b)
	{
		return (a.x == b.x && a.y == b.y);
	}

	typedef Vec2<real32> Vec2f;

	//************************************
	// Vector3
	//************************************

	template<typename T>
	struct Vec3
	{
		union
		{
			T ptr[4];
			struct
			{
				union
				{
					T x;
					T r;
				};
				union
				{
					T y;
					T g;
				};
				union
				{
					T z;
					T b;
				};
				union
				{
					T pad0;
					T pad1;
				};
			};
		};

		Vec3()
		{			
			this->x = static_cast<T>(0.0); 
			this->y = static_cast<T>(0.0);
			this->z = static_cast<T>(0.0);
			this->pad0 = static_cast<T>(0.0);
		}

		Vec3(const T &x, const T &y, const T &z)
		{
			this->x = x; this->y = y; this->z = z; this->pad0 = 0;
		}

		Vec3(const T &all)
		{
			this->x = all; this->y = all; this->z = all; this->pad0 = 0;
		}

		//TODO: Template this for type
		Vec3(const Vec4<T> &a)
		{
			this->x = a.x; this->y = a.y; this->z = a.z; this->pad0 = 0;
		}

		T& operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 3);
			return (&x)[index];
		}

		T operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 3);
			return (&x)[index];
		}
	};

	typedef Vec3<real32> Vec3f;


	template<typename T>
	T Dot(const Vec3<T> &a, const Vec3<T> &b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	template<typename T>
	T Mag(const Vec3<T> &a)
	{
		return Sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	}
	
	template<typename T>
	T Distance(const Vec3<T> &a, const Vec3<T> &b)
	{
		return Mag(a - b);
	}

	template<typename T>
	T SqrdDistance(const Vec3<T> &a, const Vec3<T> &b)
	{
		return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
	}
	
	template<typename T>
	int32 MaxIndex(const Vec3<T> &a)
	{
		return (a.x > a.y) ? ((a.x > a.z) ? 0 : 2) : ((a.y > a.z) ? 1 : 2);
	}

	template<typename T>
	int32 AbsMaxIndex(const Vec3<T> &a)
	{
		T x = Abs(a.x);
		T y = Abs(a.y);
		T z = Abs(a.z);
		return (x > y) ? ((x > z) ? 0 : 2) : ((y > z) ? 1 : 2);
	}

	template<typename T>
	Vec3<T> Normalize(const Vec3<T> &a)
	{
		T m = Mag(a);
		Vec3<T> result = Vec3<T>(a.x / m, a.y / m, a.z / m);
		return result;
	}

	template<typename T>
	Vec3<T> Cross(const Vec3<T> &a, const Vec3<T> &b)
	{
		T x = a.y * b.z - b.y * a.z;
		T y = a.z * b.x - b.z * a.x;
		T z = a.x * b.y - b.x * a.y;
		return Vec3<T>(x, y, z);
	}

	template<typename T>
	bool32 Equal(const Vec3<T> &a, const Vec3<T> &b, const T &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		bool32 resx = Equal(a.x, b.x, epsilon);
		bool32 resy = Equal(a.y, b.y, epsilon);
		bool32 resz = Equal(a.z, b.z, epsilon);
		return resx && resy && resz;
	}

	template<typename T>
	String ToString(const Vec3<T> &a)
	{
		StringStream ss;
		ss << '(' << a.x << ", " << a.y << ", " << a.z << ')';
		return ss.str();
	}

	template<typename T>
	inline bool32 operator ==(const Vec3<T> &a, const Vec3<T> &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
	}

	template<typename T>
	inline bool32 operator !=(const Vec3<T> &a, const Vec3<T> &b)
	{		
		return (a.x != b.x) || (a.y != b.y) || (a.z != b.z);
	}

	template<typename T>
	inline Vec3<T> operator +(const Vec3<T> &a, const Vec3<T> &b)
	{
		return Vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	template<typename T>
	inline Vec3<T> operator -(const Vec3<T> &a, const Vec3<T> &b)
	{
		return Vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	template<typename T>
	inline Vec3<T> operator *(const Vec3<T> &a, const Vec3<T> &b)
	{
		return Vec3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	template<typename T>
	inline Vec3<T> operator /(const Vec3<T> &a, const Vec3<T> &b)
	{
		return Vec3<T>(a.x / b.x, a.y / b.y, a.z / b.z);
	}

	template<typename T>
	inline Vec3<T> operator *(const Vec3<T> &a, const T &b)
	{
		return Vec3<T>(a.x * b, a.y * b, a.z * b);
	}

	template<typename T>
	inline Vec3<T> operator *(const T &a, const Vec3<T> &b)
	{
		return Vec3<T>(b.x * a, b.y * a, b.z * a);
	}

	template<typename T>
	inline Vec3<T> operator /(const Vec3<T> &a, const T &b)
	{
		return Vec3<T>(a.x / b, a.y / b, a.z / b);
	}

	template<typename T>
	inline Vec3<T> operator /(const T &b, const Vec3<T> &a)
	{
		return Vec3<T>(b / a.x, b / a.y, b / a.z);
	}

	template<typename T>
	inline void operator +=(Vec3<T> &a, const Vec3<T> &b)
	{
		a = Vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	template<typename T>
	inline void operator -=(Vec3<T> &a, const Vec3<T> &b)
	{
		a = Vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
	}
	
	//************************************
	// Vector4
	//************************************

	template<typename T>
	struct Vec4
	{
		union
		{
			T ptr[4];
			struct
			{
				union
				{
					T x;
					T r;
				};
				union
				{
					T y;
					T g;
				};
				union
				{
					T z;
					T b;
				};
				union
				{
					T w;
					T a;
				};
			};
		};

		Vec4()
		{
			this->x = 0; this->y = 0; this->z = 0; this->w = 0;
		}
		
		Vec4(const T &x, const T &y, const T &z, const T &w)
		{
			this->x = x; this->y = y; this->z = z; this->w = w;
		}
		
		Vec4(const T &all)
		{
			this->x = all; this->y = all;  this->z = all; this->w = all;
		}
		
		// @TODO: Template with cast
		Vec4(const Vec3<T> &xyz, const T &w)
		{
			this->x = xyz.x;
			this->y = xyz.y;
			this->z = xyz.z;
			this->w = w;
		}

		T& operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 4);
			return (&x)[index];
		}

		T operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 4);
			return (&x)[index];
		}
	};

	typedef Vec4<real32> Vec4f;

	template<typename T>
	T Mag(const Vec4<T> &a)
	{
		return Sqrt(a.x*a.x + a.y * a.y + a.z * a.z + a.w *a.w);
	}

	template<typename T>
	Vec4<T> Normalize(const Vec4<T> &a)
	{
		T m = Mag(a);
		Vec4<T> result = Vec4<T>(a.x / m, a.y / m, a.z / m, a.w / m);
		return result;
	}

	template<typename T>
	T Dot(const Vec4<T> &a, const Vec4<T> &b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	template<typename T>
	String ToString(const Vec4<T> &a)
	{
		StringStream ss;
		
		ss << '(' << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ')';
		
		return ss.str();
	}

	template<typename T>
	inline bool32 operator ==(const Vec4<T> &a, const Vec4<T> &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == a.w);
	}

	template<typename T>
	inline bool32 operator !=(const Vec4<T> &a, const Vec4<T> &b)
	{
		return (a.x != b.x) || (a.y != b.y) || (a.z != b.z) || (a.w != b.w);
	}

	template<typename T>
	inline Vec4<T> operator +(const Vec4<T> &a, const Vec4<T> &b)
	{
		return Vec4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
	}

	template<typename T>
	inline Vec4<T> operator -(const Vec4<T> &a, const Vec4<T> &b)
	{
		return Vec4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
	}

	template<typename T>
	inline Vec4<T> operator *(const Vec4<T> &a, const Vec4<T> &b)
	{
		return Vec4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
	}

	template<typename T>
	inline Vec4<T> operator /(const Vec4<T> &a, const Vec4<T> &b)
	{
		return Vec4<T>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
	}

	template<typename T>
	inline Vec4<T> operator *(const Vec4<T> &a, const T &b)
	{
		return Vec4<T>(a.x * b, a.y * b, a.z * b, a.w * b);
	}

	template<typename T>
	inline Vec4<T> operator /(const Vec4<T> &a, const T &b)
	{
		return Vec4<T>(a.x / b, a.y / b, a.z / b, a.w / b);
	}
	
	// @TODO: Name comment

	template<typename T>
	struct Quat
	{
		union
		{		
			T ptr[4];
			struct
			{
				T x;
				T y;
				T z;
				T w;
			};
		};

		Quat()
		{
			this->x = static_cast<T>(0.0); 
			this->y = static_cast<T>(0.0); 
			this->z = static_cast<T>(0.0); 
			this->w = static_cast<T>(1.0);
		}

		Quat(const T &x, const T &y, const T &z, const T &w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		Quat(const Vec3f &xyz, const T &w)
		{
			this->x = xyz.x;
			this->y = xyz.y;
			this->z = xyz.z;
			this->w = w;
		}

		T& operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 4);
			return (&x)[index];
		}

		T operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 4);
			return (&x)[index];
		}
	};

	typedef Quat<real32> Quatf;

	template<typename T>
	Quat<T> Conjugate(const Quat<T> &a)
	{
		return Quat<T>(-a.x, -a.y, -a.z, a.w);
	}

	template<typename T>
	T Mag(const Quat<T> &a)
	{
		return Sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
	}

	template<typename T>
	Quat<T> Normalize(const Quat<T> &a)
	{
		T m = Mag(a);
		return Quat<T>(a.x / m, a.y / m, a.z / m, a.w / m);
	}

	template<typename T>
	Quat<T> EulerToQuat(const Vec3<T> &euler_angle)
	{
		Vec3<T> c = Vec3f(Cos(DegToRad(euler_angle.x) / 2.0f), Cos(DegToRad(euler_angle.y) / 2.0f), Cos(DegToRad(euler_angle.z) / 2.0f));
		Vec3<T> s = Vec3f(Sin(DegToRad(euler_angle.x) / 2.0f), Sin(DegToRad(euler_angle.y) / 2.0f), Sin(DegToRad(euler_angle.z) / 2.0f));

		Quat<T> q;
		q.x = s.x * c.y * c.z - c.x * s.y * s.z;
		q.y = c.x * s.y * c.z + s.x * c.y * s.z;
		q.z = c.x * c.y * s.z - s.x * s.y * c.z;
		q.w = c.x * c.y * c.z + s.x * s.y * s.z;
		return q;
	}

	template<typename T>
	Vec3<T> QuatToEuler(const Quat<T> &q)
	{
		// @HELP: Glm and Math book
		Vec3<T> euler;
		Vec2<T> sp;
		sp.x = 2.f * (q.y * q.z + q.w * q.x);
		sp.y = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

		if (Equal(Vec2<T>(sp.x, sp.y), Vec2<T>(0.0)))
		{
			euler.x = 2.0f * Atan(q.w, q.x);
		}
		else
		{
			euler.x = Atan(sp.x, sp.y);
		}

		euler.y = Asin(Clamp(-2.0f * (q.x * q.z - q.w * q.y), -1.0f, 1.0f));
		euler.z = Atan(2.0f * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);

		euler.x = RadToDeg(euler.x);
		euler.y = RadToDeg(euler.y);
		euler.z = RadToDeg(euler.z);

		return euler;
	}

	template<typename T>
	Quat<T> Rotate(const Quat<T> &a, const Quat<T> &b)
	{
		return a * b;
	}

	template<typename T>
	Vec3<T> Rotate(const Quat<T> &r, const Vec3<T> &point)
	{
		//@Help: https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
		Quat<T> rc = Conjugate(Normalize(r));
		Quat<T> pp = Quat<T>(point, 0);
		Quat<T> res = (r * pp) * rc;

		return Vec3<T>(res.x, res.y, res.z);
	}

	template<typename T>
	Vec3<T> Rotate(const T &d_angle, const Vec3<T> &point, const Vec3<T> &axis)
	{
		Vec3<T> ax = Normalize(axis);

		T sh = Sin(DegToRad(d_angle / 2));
		T ch = Cos(DegToRad(d_angle / 2));

		Quat<T> r(ax.x * sh,
			ax.y * sh,
			ax.z * sh,
			ch);

		Quat<T> rc = Conjugate(r);
		Quat<T> pp = Quat<T>(point, 0);

		Quat<T> res = (r * pp) * rc;

		return Vec3<T>(res.x, res.y, res.z);
	}

	template<typename T>
	Quat<T> Rotate(const Quat<T> &q, const T &d_angle, Vec3<T> axis)
	{
		// @HELP: https://stackoverflow.com/questions/4436764/rotating-a-quaternion-on-1-axis
		axis = Normalize(axis);

		T rangle = DegToRad(d_angle);

		T s = SafeTruncateDouble(sin(rangle / 2.0));
		T c = SafeTruncateDouble(cos(rangle / 2.0));

		T x = axis.x * s;
		T y = axis.y * s;
		T z = axis.z * s;
		T w = c;

		Quat<T> result = Normalize(Quat<T>(x, y, z, w));
		return result;
	}

	template<typename T>
	Quat<T> Slerp(const Quat<T> &a, const Quat<T> &b, const T &t)
	{
		Quat<T> an = Normalize(a);
		Quat<T> bn = Normalize(b);

		T d = an.x * bn.x + an.y * bn.y + an.z * bn.z + an.w * bn.w;
		T tinv = 1.0f - t;
		int32 ds = Sign(d);

		Quat<T> result;
		result.x = an.x * tinv + ds * t * bn.x;
		result.y = an.y * tinv + ds * t * bn.y;
		result.z = an.z * tinv + ds * t * bn.z;
		result.w = an.w * tinv + ds * t * bn.w;

		return Normalize(result);
	}

	template<typename T>
	bool32 Equal(const Quat<T> &q1, const Quat<T> &q2, const T &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		T dx = Abs(q1.x - q2.x);
		T dy = Abs(q1.y - q2.y);
		T dz = Abs(q1.z - q2.z);
		T dw = Abs(q1.w - q2.w);
		return (dx < epsilon && dy < epsilon && dz < epsilon && dw < epsilon);
	}

	template<typename T>
	String ToString(const Quat<T> &a)
	{
		StringStream ss;
		ss << '(' << '{' << a.x << ", " << a.y << ", " << a.z << '}' << ' ' << a.w << ')';
		return ss.str();
	}

	Mat3 QuatToMat3(const Quatf &q);

	Mat4 QuatToMat4(const Quatf &q);

	template<typename T>
	inline Quat<T> operator *(const Quat<T> &q, const Quat<T> &p)
	{
		T w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
		T x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
		T y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
		T z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
		return Quatf(x, y, z, w);
	}

	template<typename T>
	inline Quat<T> operator *(const Quat<T> &a, const Vec3<T> &b)
	{
		T w = -a.x * b.x - a.y - b.y - a.z * b.z;
		T x = a.w * b.x + a.y *b.z - a.z * b.y;
		T y = a.w * b.y + a.z *b.x - a.x * b.z;
		T z = a.w * b.z + a.x *b.y - a.y * b.x;
		return Quatf(x, w, z, w);
	}





	struct  Mat3
	{
		union
		{
			real32 ptr[12]; // @NOTE: 12 because padding byte
			struct
			{
				Vec3f row0;
				Vec3f row1;
				Vec3f row2;
			};
		};
		Mat3()
		{
			row0 = Vec3f(1, 0, 0);
			row1 = Vec3f(0, 1, 0);
			row2 = Vec3f(0, 0, 1);
		}

		Mat3(const real32 &a)
		{
			row0 = Vec3f(a, 0, 0);
			row1 = Vec3f(0, a, 0);
			row2 = Vec3f(0, 0, a);
		}
		Mat3(const Vec3f &_row0, const Vec3f &_row1, const Vec3f &_row2)
		{
			row0 = _row0;
			row1 = _row1;
			row2 = _row2;
		}

		Vec3f& operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 3);
			return (&row0)[index];
		}

		Vec3f operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 3);
			return (&row0)[index];
		}
	};

	struct  Mat4
	{
		union
		{
			real32 ptr[16];			
			struct
			{
				Vec4f row0;
				Vec4f row1;
				Vec4f row2;
				Vec4f row3;
			};
		};

		Mat4()
		{
			row0 = Vec4f(1, 0, 0, 0);
			row1 = Vec4f(0, 1, 0, 0);
			row2 = Vec4f(0, 0, 1, 0);
			row3 = Vec4f(0, 0, 0, 1);
		}

		Mat4(const real32 &a)
		{
			row0 = Vec4f(a, 0, 0, 0);
			row1 = Vec4f(0, a, 0, 0);
			row2 = Vec4f(0, 0, a, 0);
			row3 = Vec4f(0, 0, 0, a);
		}

		Mat4(const Vec4f &_row0, const Vec4f &_row1, const Vec4f &_row2, const Vec4f &_row3)
		{
			row0 = _row0;
			row1 = _row1;
			row2 = _row2;
			row3 = _row3;
		}
		
		Mat4(const Mat3 &a, const Vec3f &translation)
		{
			row0 = Vec4f(a.row0, 0);
			row1 = Vec4f(a.row1, 0);
			row2 = Vec4f(a.row2, 0);
			row3 = Vec4f(translation, 1);
		}

		Vec4f & operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 4);
			return (&row0)[index];
		}

		Vec4f operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 4);
			return (&row0)[index];
		}
	};

	struct  Polar_coord
	{
		real r;
		real theta;
		real z;
	};

	struct Basis
	{
		union 
		{
			Mat3 mat;
			struct
			{
				Vec3f right;
				Vec3f upward;
				Vec3f forward;
			};
		};

		Basis()
		{
			mat = Mat3(1);
		}

		Basis(const Mat3 &basis)
		{
			this->mat = basis;
		}
		
		Basis(const Vec3f &right, const Vec3f &upward, const Vec3f &forward)
		{
			this->right = right;
			this->upward = upward;
			this->forward = forward;
		}
	};


	//************************************
	// Quaterion functions
	//************************************



	//************************************
	// Coordinate spaces
	//************************************

	Vec4f GetNormalisedDeviceCoordinates(const real32 &window_width, const real32 &window_height, const real32 &mouse_x, const real32 &mouse_y);

	Vec4f ToViewCoords(const Mat4 &projection_matrix, const Vec4f &viewCoords);

	Vec3f ToWorldCoords(const Mat4 &view_matrix, const Vec4f &viewCoords);






	//************************************
	// Matrix 3x3
	//************************************

	real32 GetMatrixElement(const Mat3 &a, const int32 &row, const int32 &col);
	
	Vec3f GetColumn(const Mat3 &a, const uint32 &col);

	Vec3f DecomposeToScale(const Mat3 &a);

	Mat3 DecomposeToRotationMatrix(const Mat3 &a);
	
	String ToString(const Mat3 &a);

	real32 Det(const Mat3 &a);

	Mat3 ScaleDirection(const Mat3 &a, const real32 &k, Vec3f direction);

	Mat3 Rotate(const Mat3 &a, const real32 &d_angle, Vec3f axis);

	Mat3 Mat4ToMat3(const Mat4 &a);

	Quatf Mat3ToQuat(const Mat3 &a);

	//************************************
	// Matrix 4x4
	//************************************

	real32 GetMatrixElement(const Mat4 &a, const int32 &row, const int32 &col);
	
	Vec4f GetColumn(const Mat4 &a, const uint32 &col);

	String ToString(const Mat4 &a);

	bool CheckOrthogonal(const Mat4 &a, const real32 tolerance = 0.01);

	Mat3 Adjoint(const Mat4 &a, const int32 &row, const int32 &col);
	
	Mat4 Inverse(const Mat4 &a);

	Mat4 Transpose(Mat4 a);

	real32 Det(const Mat4 &a);

	Mat4 Mat3ToMat4(const Mat3 &a, const Vec4f &b);

	Mat4 Translate(Mat4 a, Vec3f translation);

	Mat4 Translate(Mat4 a, real32 length, real32 d_angle, real32 z);

	Mat4 Translate(Mat4 a, Polar_coord p_coord);

	Mat4 Rotate(Mat4 a, real32 dangle, Vec3f axis);

	Quatf Mat4ToQuat(const Mat4 &a);

	Mat4 ScaleDirection(Mat4 a, real32 k, Vec3f unit_direction, bool should_normalize = true);

	Mat4 ScaleCardinal(Mat4 a, Vec3f direction);
	
	Mat4 CalculateTransformMatrix(const Vec3f &position, const Vec3f &scale, const Quatf &rotation);
	
	void Print(const Mat4 &m);

	void PrintPretty(const Mat4 &m);
			
	//************************************
	// Other Functions
	//************************************

	Polar_coord Canonical(real32 r, real32 theta, real32 z);

	Mat4  Perspective(const real32 &dfovy, const real32 &aspect, const real32 &fnear, const real32 &ffar);

	Mat4 Orthographic(const real32 &left, const real32 &right, const real32 &top, const real32 &bottom, const real32 &_near, const real32 &_far);

	Mat4 LookAt(const Vec3f &position, const Vec3f &target, const Vec3f &up);
	   	 
	//************************************
	// Usefull classes for everything
	//************************************

	class Transform 
	{
	public:
		Vec3f position = Vec3f(0, 0, 0);
		Vec3f scale = Vec3f(1, 1, 1);
		Quatf rotation = Quatf(0, 0, 0, 1);
		Basis basis;

	public:
		Mat4 CalcTransformMatrix() const
		{
			return CalculateTransformMatrix(position, scale, rotation);
		}
	};
	
	//************************************
	// Inline math functions
	//************************************





	inline float RadicalInverse(uint32 a)
	{
		float x = 0.0;
		float f = 0.5;
		while (a)
		{
			x += f * (float)(!a & 1);
			a /= 2;
			f *= 0.5;
		}
		return x;
	}


	inline Vec3f Lerp(const Vec3f &a, const Vec3f &b, const real32 &t)
	{
		Vec3f r = Vec3f(
			a.x + t * (b.x - a.x),
			a.y + t * (b.y - a.y),
			a.z + t * (b.z - a.z)
		);
		return r;
	}

	inline Vec3f Abs(const Vec3f &a)
	{
		return Vec3f(std::abs(a.x), std::abs(a.y), std::abs(a.z));
	}


	//************************************
	// Operator overloads for math structures
	//************************************


	





	inline Mat3 operator /(const Mat3 &a, const real32 &b)
	{
		Mat3 result;
		for (int32 i = 0; i < 12; i++)
		{
			result.ptr[i] = a.ptr[i] / b;
		}
		return result;
	}

	inline Mat3 operator /(const real32 &a, const Mat3 &b)
	{
		Mat3 result;
		for (int32 i = 0; i < 12; i++)
		{
			result.ptr[i] = b.ptr[i] / a;
		}
		return result;
	}


	Vec3f operator *(const Vec3f &a, const Mat3 &b);

	Mat3 operator *(const Mat3 &a, const Mat3 &b);

	Mat4  operator /(Mat4 a, float b);

	Mat4  operator *(const Mat4 &a, const Mat4&b);

	Mat4 operator +(const Mat4 &a, const Mat4 &b);
	
	Vec4f  operator *(const Vec4f &a, const Mat4 &b);	



}