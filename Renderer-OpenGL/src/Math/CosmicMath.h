#pragma once
#include <math.h>
#include <random> 
#include <string>
#include <sstream>
#include <iostream>
namespace cm
{	
	//************************************
	// Defines
	//************************************

	#define PI 3.14159265359

#ifndef TYPES

#define TYPES

	typedef uint8_t		uint8;
	typedef uint16_t	uint16;
	typedef uint32_t	uint32;
	typedef uint64_t	uint64;

	typedef int8_t	int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;

	typedef bool	bool32;
	typedef float	real32;
	typedef double	real64;

	typedef std::string			String;
	typedef std::stringstream	StringStream;

#endif 


#ifndef Assert
#ifdef _DEBUG

#define Assert(Value) if (!(Value)) {*(int *)0 = 0;}

#else

#define Assert(Value)

#endif
#endif // !Assert(Value)

	//************************************
	// Standard functions
	//************************************

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
	inline constexpr T RadicalInverse(uint32 a)
	{
		T result = static_cast<T>(0.0);
		T f = static_cast<T>(0.5);

		while (a)
		{
			result += f * static_cast<T>((!a & 1));
			a /= static_cast<T>(2);
			f *= static_cast<T>(0.5);
		}

		return result;
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
	
	template<typename T> 
	struct PolarCoord;

	template<typename T>
	struct Vec2;

	typedef Vec2<real32> Vec2f;

	template<typename T>
	struct Vec3;

	typedef Vec3<real32> Vec3f;

	template<typename T>
	struct Vec4;

	typedef Vec4<real32> Vec4f;

	template<typename T>
	struct Quat;

	typedef Quat<real32> Quatf;
	
	template<typename T>
	struct Mat3;

	typedef Mat3<real32> Mat3f;

	template<typename T>
	struct Mat4;

	typedef Mat4<real32> Mat4f;
	
	template<typename T>
	struct PolarCoord
	{
		T r;
		T theta;
		T z;
	};

	//************************************
	// Vector2 
	//************************************

	template<typename T>
	struct Vec2
	{
		T x;
		T y;

		Vec2()
		{
			this->x = static_cast<T>(0); 
			this->y = static_cast<T>(0);
		}
		
		Vec2(const T &x, const T &y)
		{
			this->x = x; 
			this->y = y;
		}
		
		Vec2(const T &all)
		{
			this->x = all;
			this->y = all;
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
		T result = Sqrt(a.x * a.x + a.y * a.y);

		return result;
	}

	template<typename T>
	T Normalize(const Vec2<T> &a)
	{
		T magA = Mag(a);

		T result = a / magA;

		return result;
	}

	template<typename T>
	bool32 Equal(const Vec2<T> &a1, const Vec2<T> a2, const real32 &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		T dx = Abs(a1.x - a2.x);
		T dy = Abs(a1.y - a2.y);

		bool32 result = (dx < epsilon && dy < epsilon);

		return result;
	}

	template<typename T>
	inline constexpr Vec2<T> operator +(const Vec2<T> &a, const Vec2<T> &b)
	{
		Vec2<T> result =  Vec2<T>(a.x + b.x, a.y + b.y);

		return result;
	}
	
	template<typename T>
	inline constexpr Vec2<T> operator -(const Vec2<T> &a, const Vec2<T> &b)
	{
		Vec2<T> result = Vec2<T>(a.x - b.x, a.y - b.y);

		return result;
	}

	template<typename T>
	inline constexpr Vec2<T> operator /(const Vec2<T> &a, const T &b)
	{
		Vec2<T> result = Vec2<T>(a.x / b, a.y / b);

		return result;
	}

	template<typename T>
	inline constexpr bool32 operator ==(const Vec2<T> &a, const Vec2<T> &b)
	{
		bool32 result = (a.x == b.x && a.y == b.y);

		return result;
	}
		
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
			this->x		= static_cast<T>(0.0); 
			this->y		= static_cast<T>(0.0);
			this->z		= static_cast<T>(0.0);
			this->pad0	= static_cast<T>(0.0);
		}

		Vec3(const T &x, const T &y, const T &z)
		{
			this->x		= x; 
			this->y		= y; 
			this->z		= z; 
			this->pad0	= static_cast<T>(0.0);
		}

		Vec3(const T &all)
		{
			this->x		= all; 
			this->y		= all; 
			this->z		= all; 
			this->pad0	= static_cast<T>(0.0);
		}

		//TODO: Template this for type
		Vec3(const Vec4<T> &a)
		{
			this->x		= a.x; 
			this->y		= a.y; 
			this->z		= a.z; 
			this->pad0	= static_cast<T>(0.0);
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

	template<typename T>
	T Dot(const Vec3<T> &a, const Vec3<T> &b)
	{
		T result = a.x * b.x + a.y * b.y + a.z * b.z;

		return result;
	}

	template<typename T>
	T Mag(const Vec3<T> &a)
	{
		T result = Sqrt(a.x * a.x + a.y * a.y + a.z * a.z);

		return result;
	}
	
	template<typename T>
	T Distance(const Vec3<T> &a, const Vec3<T> &b)
	{
		T result = Mag(a - b);

		return result;
	}

	template<typename T>
	T SqrdDistance(const Vec3<T> &a, const Vec3<T> &b)
	{
		T result = (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);

		return result;
	}
	
	template<typename T>
	int32 MaxIndex(const Vec3<T> &a)
	{
		int32 result = (a.x > a.y) ? ((a.x > a.z) ? 0 : 2) : ((a.y > a.z) ? 1 : 2);

		return result;
	}

	template<typename T>
	int32 AbsMaxIndex(const Vec3<T> &a)
	{
		T x = Abs(a.x);
		T y = Abs(a.y);
		T z = Abs(a.z);

		int32 result = (x > y) ? ((x > z) ? 0 : 2) : ((y > z) ? 1 : 2);

		return result;
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
		
		Vec3<T> result = Vec3<T>(x, y, z);

		return result;
	}
	
	template<typename T>
	inline Vec3<T> Lerp(const Vec3<T> &a, const Vec3<T> &b, const T &t)
	{
		Vec3<T> result = Vec3<T>(
			a.x + t * (b.x - a.x),
			a.y + t * (b.y - a.y),
			a.z + t * (b.z - a.z)
		);

		return result;
	}

	template<typename T>
	inline Vec3<T> Abs(const Vec3<T> &a)
	{
		Vec3<T> result = Vec3<T>(Abs(a.x), Abs(a.y), Abs(a.z));

		return result;
	}

	template<typename T>
	bool32 Equal(const Vec3<T> &a, const Vec3<T> &b, const T &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		bool32 resx = Equal(a.x, b.x, epsilon);
		bool32 resy = Equal(a.y, b.y, epsilon);
		bool32 resz = Equal(a.z, b.z, epsilon);

		bool32 result = resx && resy && resz;

		return result;
	}

	template<typename T>
	String ToString(const Vec3<T> &a)
	{
		StringStream ss;

		ss << '(' << a.x << ", " << a.y << ", " << a.z << ')';
		
		return ss.str();
	}

	template<typename T>
	inline constexpr bool32 operator ==(const Vec3<T> &a, const Vec3<T> &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
	}

	template<typename T>
	inline constexpr bool32 operator !=(const Vec3<T> &a, const Vec3<T> &b)
	{		
		return (a.x != b.x) || (a.y != b.y) || (a.z != b.z);
	}

	template<typename T>
	inline constexpr Vec3<T> operator +(const Vec3<T> &a, const Vec3<T> &b)
	{
		return Vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	template<typename T>
	inline constexpr Vec3<T> operator -(const Vec3<T> &a, const Vec3<T> &b)
	{
		return Vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	template<typename T>
	inline constexpr Vec3<T> operator *(const Vec3<T> &a, const Vec3<T> &b)
	{
		return Vec3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	template<typename T>
	inline constexpr Vec3<T> operator /(const Vec3<T> &a, const Vec3<T> &b)
	{
		return Vec3<T>(a.x / b.x, a.y / b.y, a.z / b.z);
	}

	template<typename T>
	inline constexpr Vec3<T> operator *(const Vec3<T> &a, const T &b)
	{
		return Vec3<T>(a.x * b, a.y * b, a.z * b);
	}

	template<typename T>
	inline constexpr Vec3<T> operator *(const T &a, const Vec3<T> &b)
	{
		return Vec3<T>(b.x * a, b.y * a, b.z * a);
	}

#ifdef RELEASE
#pragma warning(push)
#pragma warning(disable: 4723)
#endif

	template<typename T>
	inline constexpr Vec3<T> operator /(const Vec3<T> &a, const T &b)
	{
		//Assert(a.x != 0 && a.y != 0 && a.z != 0);
		return Vec3<T>(a.x / b, a.y / b, a.z / b);
	}

	template<typename T>
	inline constexpr Vec3<T> operator /(const T &b, const Vec3<T> &a)
	{
		//Assert(a.x != 0 && a.y != 0 && a.z != 0);
		return Vec3<T>(b / a.x, b / a.y, b / a.z);

	}

#ifdef RELEASE
#pragma warning(pop)
#endif

	template<typename T>
	inline constexpr void operator +=(Vec3<T> &a, const Vec3<T> &b)
	{
		a = Vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	template<typename T>
	inline constexpr void operator -=(Vec3<T> &a, const Vec3<T> &b)
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
	inline constexpr bool32 operator ==(const Vec4<T> &a, const Vec4<T> &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == a.w);
	}

	template<typename T>
	inline constexpr bool32 operator !=(const Vec4<T> &a, const Vec4<T> &b)
	{
		return (a.x != b.x) || (a.y != b.y) || (a.z != b.z) || (a.w != b.w);
	}

	template<typename T>
	inline constexpr Vec4<T> operator +(const Vec4<T> &a, const Vec4<T> &b)
	{
		return Vec4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
	}

	template<typename T>
	inline constexpr Vec4<T> operator -(const Vec4<T> &a, const Vec4<T> &b)
	{
		return Vec4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
	}

	template<typename T>
	inline constexpr Vec4<T> operator *(const Vec4<T> &a, const Vec4<T> &b)
	{
		return Vec4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
	}

	template<typename T>
	inline constexpr Vec4<T> operator *(const Vec4<T> &a, const T &b)
	{
		return Vec4<T>(a.x * b, a.y * b, a.z * b, a.w * b);
	}

#ifdef RELEASE
#pragma warning(push)
#pragma warning(disable: 4723)
#endif

	template<typename T>
	inline Vec4<T> operator /(const Vec4<T> &a, const Vec4<T> &b)
	{
		//Assert(a.x != 0 && a.y != 0 && a.z != 0);
		return Vec4<T>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
	}

	template<typename T>
	inline Vec4<T> operator /(const Vec4<T> &a, const T &b)
	{
		//Assert(a.x != 0 && a.y != 0 && a.z != 0);
		return Vec4<T>(a.x / b, a.y / b, a.z / b, a.w / b);
	}

#ifdef RELEASE
#pragma warning(pop)
#endif
	
	//************************************
	// Quaterion functions
	//************************************

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
	Mat3<T> QuatToMat3(const Quat<T> &q)
	{
		Mat3<T> result(1);

		T qxx = (q.x * q.x);
		T qyy = (q.y * q.y);
		T qzz = (q.z * q.z);
		T qxz = (q.x * q.z);
		T qxy = (q.x * q.y);
		T qyz = (q.y * q.z);
		T qwx = (q.w * q.x);
		T qwy = (q.w * q.y);
		T qwz = (q.w * q.z);

		result.row0.x = 1 - (2) * (qyy + qzz);
		result.row0.y = (2) * (qxy + qwz);
		result.row0.z = (2) * (qxz - qwy);

		result.row1.x = (2) * (qxy - qwz);
		result.row1.y = (1) - (2) * (qxx + qzz);
		result.row1.z = (2) * (qyz + qwx);

		result.row2.x = (2) * (qxz + qwy);
		result.row2.y = (2) * (qyz - qwx);
		result.row2.z = (1) - (2) * (qxx + qyy);

		return result;
	}

	template<typename T>
	Mat4<T> QuatToMat4(const Quat<T> &q)
	{
		Mat4<T> result(1);

		T qxx = (q.x * q.x);
		T qyy = (q.y * q.y);
		T qzz = (q.z * q.z);
		T qxz = (q.x * q.z);
		T qxy = (q.x * q.y);
		T qyz = (q.y * q.z);
		T qwx = (q.w * q.x);
		T qwy = (q.w * q.y);
		T qwz = (q.w * q.z);

		result.row0.x = 1 - (2) * (qyy + qzz);
		result.row0.y = (2) * (qxy + qwz);
		result.row0.z = (2) * (qxz - qwy);

		result.row1.x = (2) * (qxy - qwz);
		result.row1.y = (1) - (2) * (qxx + qzz);
		result.row1.z = (2) * (qyz + qwx);

		result.row2.x = (2) * (qxz + qwy);
		result.row2.y = (2) * (qyz - qwx);
		result.row2.z = (1) - (2) * (qxx + qyy);

		return result;
	}

	template<typename T>
	String ToString(const Quat<T> &a)
	{
		StringStream ss;
		ss << '(' << '{' << a.x << ", " << a.y << ", " << a.z << '}' << ' ' << a.w << ')';
		return ss.str();
	}

	template<typename T>
	inline constexpr Quat<T> operator *(const Quat<T> &q, const Quat<T> &p)
	{
		T w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
		T x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
		T y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
		T z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
		return Quatf(x, y, z, w);
	}

	template<typename T>
	inline constexpr Quat<T> operator *(const Quat<T> &a, const Vec3<T> &b)
	{
		T w = -a.x * b.x - a.y - b.y - a.z * b.z;
		T x = a.w * b.x + a.y *b.z - a.z * b.y;
		T y = a.w * b.y + a.z *b.x - a.x * b.z;
		T z = a.w * b.z + a.x *b.y - a.y * b.x;
		return Quatf(x, w, z, w);
	}


	//************************************
	// Matrix 3x3
	//************************************

	template<typename T>
	struct Mat3
	{
		union
		{
			T ptr[12]; // @NOTE: 12 because padding byte
			struct
			{
				Vec3<T> row0;
				Vec3<T> row1;
				Vec3<T> row2;
			};
		};

		Mat3()
		{
			this->row0 = Vec3<T>(static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0));
			this->row1 = Vec3<T>(static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0));
			this->row2 = Vec3<T>(static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0));
		}

		Mat3(const T &a)
		{
			this->row0 = Vec3<T>(static_cast<T>(a), static_cast<T>(0.0), static_cast<T>(0.0));
			this->row1 = Vec3<T>(static_cast<T>(0.0), static_cast<T>(a), static_cast<T>(0.0));
			this->row2 = Vec3<T>(static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(a));
		}

		Mat3(const Vec3<T> &row0, const Vec3<T> &row1, const Vec3<T> &row2)
		{
			this->row0 = row0;
			this->row1 = row1;
			this->row2 = row2;
		}

		Mat3(const Mat4<T> &a)
		{
			this->row0 = Vec3<T>(a.row0);
			this->row1 = Vec3<T>(a.row1);
			this->row2 = Vec3<T>(a.row2);
		}

		Vec3<T>& operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 3);
			return (&row0)[index];
		}

		Vec3<T> operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 3);
			return (&row0)[index];
		}
	};

	template<typename T>
	T GetMatrixElement(const Mat3<T> &a, const int32 &row, const int32 &col)
	{
		return a.ptr[4 * row + col];
	}

	template<typename T>
	Vec3<T> GetColumn(const Mat3<T> &a, const int32 &col)
	{
		Vec3<T> column(0, 0, 0);

		column.x = a.ptr[4 * 0 + col];
		column.y = a.ptr[4 * 1 + col];
		column.z = a.ptr[4 * 2 + col];
		
		return column;
	}

	template<typename T>
	Vec3<T> DecomposeToScale(const Mat3<T> &a)
	{
		T x = Mag(a.row0);
		T y = Mag(a.row1);
		T z = Mag(a.row2);

		Vec3<T> result = Vec3<T>(x, y, z);

		return result;
	}

	template<typename T>
	Mat3<T> DecomposeToRotationMatrix(const Mat3<T> &a)
	{
		Vec3<T> scale = DecomposeToScale(a);
		Mat3<T> result;

		for (int32 i = 0; i < 3; i++)
		{
			result[i] = a[i] / scale[i];
		}

		return result;
	}

	template<typename T>
	String ToString(const Mat3<T> &a)
	{
		StringStream ss;

		String space = "            ";
		ss << "| " << a.ptr[0] << space << a.ptr[1] << space << a.ptr[2] << " |" << '\n';
		ss << "| " << a.ptr[4] << space << a.ptr[5] << space << a.ptr[6] << " |" << '\n';
		ss << "| " << a.ptr[8] << space << a.ptr[9] << space << a.ptr[10] << " |" << '\n';
		
		return ss.str();
	}

	template<typename T>
	T Det(const Mat3<T> &a)
	{
		T f = a.ptr[0] * (a.ptr[5] * a.ptr[10] - a.ptr[9] * a.ptr[6]);
		T b = a.ptr[1] * (a.ptr[4] * a.ptr[10] - a.ptr[8] * a.ptr[6]);
		T c = a.ptr[2] * (a.ptr[4] * a.ptr[9] - a.ptr[8] * a.ptr[5]);

		return f - b + c;
	}

	template<typename T>
	Mat3<T> ScaleDirection(const Mat3<T> &a, const T &k, Vec3<T> direction)
	{
		direction = Normalize(direction);

		Vec3<T> i_prime(0, 0, 0);
		i_prime.x = 1 + (k - 1) * direction.x * direction.x;
		i_prime.y = (k - 1) * direction.x * direction.y;
		i_prime.z = (k - 1) * direction.x * direction.z;

		Vec3<T> j_prime(0, 0, 0);
		j_prime.x = (k - 1) * direction.x * direction.y;
		j_prime.y = 1 + (k - 1) * direction.y * direction.y;
		j_prime.z = (k - 1) * direction.y * direction.z;

		Vec3<T> k_prime(0, 0, 0);
		k_prime.x = (k - 1) * direction.x * direction.z;
		k_prime.y = (k - 1) * direction.y * direction.z;
		k_prime.z = 1 + (k - 1) * direction.z * direction.z;

		Mat3<T> result(i_prime, j_prime, k_prime);

		return result;
	}

	template<typename T>
	Mat3<T> Rotate(const Mat3<T> &a, const T &d_angle, Vec3<T> axis)
	{
		axis = Normalize(axis);

		T theata = DegToRad(d_angle);
		T cos_theata = Cos(theata);
		T sin_theata = Sin(theata);

		Vec3<T> iPrime(0, 0, 0);
		iPrime.x = Round(axis.x *axis.x * (1 - cos_theata) + cos_theata);
		iPrime.y = Round(axis.x *axis.y * (1 - cos_theata) + axis.z * sin_theata);
		iPrime.z = Round(axis.x *axis.z * (1 - cos_theata) - axis.y * sin_theata);

		Vec3<T> jPrime(0, 0, 0);
		jPrime.x = Round(axis.x *axis.y * (1 - cos_theata) - axis.z *sin_theata);
		jPrime.y = Round(axis.y *axis.y * (1 - cos_theata) + cos_theata);
		jPrime.z = Round(axis.y *axis.z * (1 - cos_theata) + axis.x *sin_theata);

		Vec3<T> kPrime(0, 0, 0);
		kPrime.x = Round(axis.x *axis.z * (1 - cos_theata) + axis.y *sin_theata);
		kPrime.y = Round(axis.y *axis.z * (1 - cos_theata) - axis.x *sin_theata);
		kPrime.z = Round(axis.z *axis.z * (1 - cos_theata) + cos_theata);

		Mat3<T> result(iPrime, jPrime, kPrime);

		return result * a;
	}

	template<typename T>
	Quat<T> Mat3ToQuat(const Mat3<T> &a)
	{
		//@ HELP: 3D Math Primer for Graphics and Game Development		
		T m11 = a.row0.x;
		T m12 = a.row0.y;
		T m13 = a.row0.z;

		T m21 = a.row1.x;
		T m22 = a.row1.y;
		T m23 = a.row1.z;

		T m31 = a.row2.x;
		T m32 = a.row2.y;
		T m33 = a.row2.z;

		T x2 = m11 - m22 - m33;
		T y2 = m22 - m11 - m33;
		T z2 = m33 - m11 - m22;
		T w2 = m11 + m22 + m33;

		int32 index = 0;
		T big2 = w2;

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

		T big = Sqrt(big2 + static_cast<T>(1.0)) * static_cast<T>(0.5);
		T mult = static_cast<T>(0.25) / big;

		Quat<T> result;

		switch (index)
		{
		case 0: {
			T x = (m23 - m32) * mult;
			T y = (m31 - m13) * mult;
			T z = (m12 - m21) * mult;
			T w = big;

			result = Quat<T>(x, y, z, w);

			break;
		}
		case 1: {
			T x = big;
			T y = (m12 + m21) * mult;
			T z = (m31 + m13) * mult;
			T w = (m23 - m32) * mult;
		
			result = Quat<T>(x, y, z, w);

			break;
		}
		case 2: {
			T x = (m12 + m21) * mult;
			T y = big;
			T z = (m23 + m32) * mult;
			T w = (m31 - m13) * mult;
		
			result = Quat<T>(x, y, z, w);

			break;
		}
		case 3: {
			T x = (m31 + m13) * mult;
			T y = (m23 + m32) * mult;
			T z = big;
			T w = (m12 - m21) * mult;
		
			result = Quat<T>(x, y, z, w);

			break;
		}
		default:
			Assert(0);
			return Quat<T>(0, 0, 0, 1);
		}

		return result;
	}

	template<typename T>
	inline constexpr Mat3<T> operator /(const Mat3<T> &a, const T &b)
	{
		Mat3<T> result;

		for (int32 i = 0; i < 12; i++)
		{
			result.ptr[i] = a.ptr[i] / b;
		}

		return result;
	}

	template<typename T>
	inline constexpr Mat3<T> operator /(const T &a, const Mat3<T> &b)
	{
		Mat3<T> result;

		for (int32 i = 0; i < 12; i++)
		{
			result.ptr[i] = b.ptr[i] / a;
		}

		return result;
	}

	template<typename T>
	inline constexpr Vec3<T> operator *(const Vec3<T> &a, const Mat3<T> &b)
	{
		Vec3<T> result(0, 0, 0);

		for (uint32 i = 0; i < 3; i++)
		{
			Vec3<T> col = GetColumn(b, i);
			result[i] = Dot(col, a);
		}

		return result;
	}

	template<typename T>
	inline constexpr Mat3<T> operator *(const Mat3<T> &a, const Mat3<T> &b)
	{
		Mat3<T> result(1);

		for (int32 i = 0; i < 3; i++)
		{
			for (int32 y = 0; y < 3; y++)
			{
				Vec3<T> col(0, 0, 0);
				
				for (int32 x = 0; x < 3; x++)
				{
					col[x] = GetMatrixElement(b, x, y);
				}

				result.ptr[4 * i + y] = Dot(col, a[i]);
			}
		}

		return result;
	}
	
	//************************************
	// Matrix 4x4
	//************************************

	template<typename T>
	struct Mat4
	{
		union
		{
			T ptr[16];		

			struct
			{
				Vec4<T> row0;
				Vec4<T> row1;
				Vec4<T> row2;
				Vec4<T> row3;
			};
		};

		Mat4()
		{
			this->row0 = Vec4<T>(1, 0, 0, 0);
			this->row1 = Vec4<T>(0, 1, 0, 0);
			this->row2 = Vec4<T>(0, 0, 1, 0);
			this->row3 = Vec4<T>(0, 0, 0, 1);
		}

		Mat4(const T &a)
		{
			this->row0 = Vec4<T>(a, 0, 0, 0);
			this->row1 = Vec4<T>(0, a, 0, 0);
			this->row2 = Vec4<T>(0, 0, a, 0);
			this->row3 = Vec4<T>(0, 0, 0, a);
		}

		Mat4(const Vec4<T> &row0, const Vec4<T> &row1, const Vec4<T> &row2, const Vec4<T> &row3)
		{
			this->row0 = row0;
			this->row1 = row1;
			this->row2 = row2;
			this->row3 = row3;
		}
		
		Mat4(const Mat3<T> &a, const Vec3<T> &translation)
		{
			this->row0 = Vec4<T>(a.row0, 0);
			this->row1 = Vec4<T>(a.row1, 0);
			this->row2 = Vec4<T>(a.row2, 0);
			this->row3 = Vec4<T>(translation, 1);
		}

		Mat4(const Mat3<T> &a, const Vec4<T> &b)
		{
			this->row0 = Vec4<T>(a.row0, 0);
			this->row1 = Vec4<T>(a.row1, 0);
			this->row2 = Vec4<T>(a.row2, 0);
			this->row3 = b;			
		}

		Vec4<T> &operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 4);
			return (&row0)[index];
		}

		Vec4<T> operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 4);
			return (&row0)[index];
		}
	};



	template<typename T>
	inline constexpr T GetMatrixElement(const Mat4<T> &a, const int32 &row, const int32 &col)
	{
		T result = a.ptr[4 * row + col];

		return result;
	}

	template<typename T>
	inline constexpr Vec4<T> GetColumn(const Mat4<T> &a, const uint32 &col)
	{
		Vec4<T> result(0, 0, 0, 0);

		result.x = a.ptr[4 * 0 + col];
		result.y = a.ptr[4 * 1 + col];
		result.z = a.ptr[4 * 2 + col];
		result.w = a.ptr[4 * 3 + col];
		
		return result;
	}

	template<typename T>
	inline constexpr Mat4<T> Transpose(const Mat4<T> &a)
	{
		Vec4<T> c0 = GetColumn(a, 0);
		Vec4<T> c1 = GetColumn(a, 1);
		Vec4<T> c2 = GetColumn(a, 2);
		Vec4<T> c3 = GetColumn(a, 3);

		Mat4<T> result(c0, c1, c2, c3);

		return result;
	}

	template<typename T>
	inline constexpr T Det(const Mat4<T> &a)
	{
		T f = a.ptr[0] * (
			a.ptr[5] * (a.ptr[10] * a.ptr[15] - a.ptr[11] * a.ptr[14]) +
			a.ptr[6] * (a.ptr[11] * a.ptr[13] - a.ptr[9] * a.ptr[15]) +
			a.ptr[7] * (a.ptr[9] * a.ptr[14] - a.ptr[10] * a.ptr[13]));

		T b = a.ptr[1] * (
			a.ptr[4] * (a.ptr[10] * a.ptr[15] - a.ptr[11] * a.ptr[14]) +
			a.ptr[6] * (a.ptr[11] * a.ptr[12] - a.ptr[8] * a.ptr[15]) +
			a.ptr[7] * (a.ptr[8] * a.ptr[14] - a.ptr[10] * a.ptr[12]));

		T c = a.ptr[2] * (
			a.ptr[4] * (a.ptr[9] * a.ptr[15] - a.ptr[11] * a.ptr[13]) +
			a.ptr[5] * (a.ptr[11] * a.ptr[12] - a.ptr[8] * a.ptr[15]) +
			a.ptr[7] * (a.ptr[8] * a.ptr[13] - a.ptr[9] * a.ptr[12]));

		T d = a.ptr[3] * (
			a.ptr[4] * (a.ptr[9] * a.ptr[14] - a.ptr[10] * a.ptr[13]) +
			a.ptr[5] * (a.ptr[10] * a.ptr[12] - a.ptr[8] * a.ptr[14]) +
			a.ptr[6] * (a.ptr[8] * a.ptr[13] - a.ptr[9] * a.ptr[12]));

		T result = f - b + c - d;

		return result;
	}   	 

	template<typename T>
	inline constexpr Mat4<T> Translate(const Mat4<T> &a, const Vec3<T> &translation)
	{
		Mat4<T> result = a;
		
		result.row3 = Vec4<T>(translation, 1) * a;

		return result;
	}
	
	template<typename T>
	PolarCoord<T> Canonical(T r, T theta, T z)
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
				theta += static_cast<T>(PI);
			}
			if (fabs(theta) > PI)
			{
				theta += static_cast<T>(PI);
				theta -= static_cast<T>(floor(theta / 2 * PI) * 2 * PI);
				theta -= static_cast<T>(PI);
			}
		}

		return { r, theta, z };
	}

	// @TODO: PolarCoords
	template<typename T>
	Mat4<T> Translate(Mat4<T> a, T length, T d_angle, T z)
	{
		PolarCoord<T> p_coord = Canonical(length, d_angle, z);
		a.row3 = Vec4<T>(p_coord.r * Cos(p_coord.theta), p_coord.r * Sin(p_coord.theta), p_coord.z, 1) * a;
		return a;
	}

	// @TODO: PolarCoords
	template<typename T>
	Mat4<T> Translate(Mat4<T> a, PolarCoord<T> p_coord)
	{
		a.row3 = Vec4<T>(p_coord.r * Cos(p_coord.theta), p_coord.r * Sin(p_coord.theta), p_coord.z, 1) * a;
		return a;
	}


	
	template<typename T>
	inline constexpr Mat4<T> Rotate(const Mat4<T> &a, const T &d_angle, Vec3<T> axis)
	{
		axis = Normalize(axis);

		T theata = DegToRad(d_angle);
		T cos_theata = Cos(theata);
		T sin_theata = Sin(theata);

		Vec4<T> iPrime(0, 0, 0, 0);
		iPrime.x = Round(axis.x *axis.x * (1 - cos_theata) + cos_theata);
		iPrime.y = Round(axis.x *axis.y * (1 - cos_theata) + axis.z * sin_theata);
		iPrime.z = Round(axis.x *axis.z * (1 - cos_theata) - axis.y * sin_theata);
		iPrime.w = 0;

		Vec4<T> jPrime(0, 0, 0, 0);
		jPrime.x = Round(axis.x *axis.y * (1 - cos_theata) - axis.z *sin_theata);
		jPrime.y = Round(axis.y *axis.y * (1 - cos_theata) + cos_theata);
		jPrime.z = Round(axis.y *axis.z * (1 - cos_theata) + axis.x *sin_theata);
		jPrime.w = 0;

		Vec4<T> kPrime(0, 0, 0, 0);
		kPrime.x = Round(axis.x *axis.z * (1 - cos_theata) + axis.y *sin_theata);
		kPrime.y = Round(axis.y *axis.z * (1 - cos_theata) - axis.x *sin_theata);
		kPrime.z = Round(axis.z *axis.z * (1 - cos_theata) + cos_theata);
		kPrime.w = 0;

		Vec4<T> wPrime(0, 0, 0, 1);

		Mat4<T> result(iPrime, jPrime, kPrime, wPrime);

		result = result * a;

		return result;
	}	   

	template<typename T>
	inline constexpr Mat4<T> ScaleDirection(const Mat4<T> &a, const T &k, Vec3<T> unit_direction)
	{
		unit_direction = Normalize(unit_direction);

		Vec4<T> iPrime(0, 0, 0, 0);
		iPrime.x = 1 + (k - 1) * unit_direction.x * unit_direction.x;
		iPrime.y = (k - 1) * unit_direction.x * unit_direction.y;
		iPrime.z = (k - 1) * unit_direction.x * unit_direction.z;

		Vec4<T> jPrime(0, 0, 0, 0);
		jPrime.x = (k - 1) * unit_direction.x * unit_direction.y;
		jPrime.y = 1 + (k - 1) * unit_direction.y * unit_direction.y;
		jPrime.z = (k - 1) * unit_direction.y * unit_direction.z;

		Vec4<T> kPrime(0, 0, 0, 0);
		kPrime.x = (k - 1) * unit_direction.x * unit_direction.z;
		kPrime.y = (k - 1) * unit_direction.y * unit_direction.z;
		kPrime.z = 1 + (k - 1) * unit_direction.z * unit_direction.z;

		Vec4<T> wPrime(0, 0, 0, 1);

		Mat4<T> result(iPrime, jPrime, kPrime, wPrime);

		result = result * a;

		return result;
	}

	template<typename T>
	inline constexpr Mat4<T> ScaleCardinal(const Mat4<T> &a, const Vec3<T> &direction)
	{
		Mat4<T> result = a;

		result.row0 = a.row0 * direction.x;
		result.row1 = a.row1 * direction.y;
		result.row2 = a.row2 * direction.z;

		return result;
	}

	template<typename T>
	inline constexpr Mat4<T> CalculateTransformMatrix(const Vec3<T> &position, const Vec3<T> &scale, const Quat<T> &rotation)
	{
		// @HELP: Real time rendering book
		Mat4<T> result;
		Mat4<T> trans(1);
		Mat4<T> rot(1);
		Mat4<T> scl(1);

		trans = Translate(trans, position);
		rot = QuatToMat4(Conjugate(rotation));
		scl = ScaleCardinal(scl, scale);
		
		result = scl * rot * trans;

		return result;
	}

	template<typename T>
	bool32 CheckOrthogonal(const Mat4<T> &a, const T tolerance = 0.01)
	{
		Mat4<T> result = a * Transpose(a);

		for (int32 i = 0; i < 4; i++)
		{
			if (abs(1 - abs(result.ptr[i * 5])) > tolerance)
			{
				return false;
			}
		}

		return true;
	}

	template<typename T>
	Mat3<T> Adjoint(const Mat4<T> &a, const int32 &row, const int32 &col)
	{
		Mat3<T> result(1);
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

				if (index == 3 || index == 7) // @NOTE: REMEMBER THE PADDING BYTE!!
				{
					index++;
				}

				result.ptr[index++] = GetMatrixElement(a, r, c);
			}
		}

		return result;
	}

	template<typename T>
	Mat4<T> Inverse(const Mat4<T> &a)
	{
		if (CheckOrthogonal(a))
		{
			return Transpose(a);
		}

		Mat4<T> result(1);
		Mat3<T> ad(1);
		int32 index = 0;

		for (int32 row = 0; row < 4; row++)
		{
			for (int32 col = 0; col < 4; col++)
			{
				if ((row + col) % 2)
				{
					ad = Adjoint(a, row, col);
					T i = -Det(ad);
					result.ptr[index++] = i;
				}
				else
				{
					ad = Adjoint(a, row, col);
					T i = Det(ad);
					result.ptr[index++] = i;
				}
			}
		}

		T determinant = Det(a);

		result = Transpose(result) / determinant;

		return result;
	}
	
	template<typename T>
	Quat<T> Mat4ToQuat(const Mat4<T> &a)
	{
		//@ HELP: 3D Math Primer for Graphics and Game Development		
		T m11 = a.row0.x;
		T m12 = a.row0.y;
		T m13 = a.row0.z;

		T m21 = a.row1.x;
		T m22 = a.row1.y;
		T m23 = a.row1.z;

		T m31 = a.row2.x;
		T m32 = a.row2.y;
		T m33 = a.row2.z;

		T x2 = m11 - m22 - m33;
		T y2 = m22 - m11 - m33;
		T z2 = m33 - m11 - m22;
		T w2 = m11 + m22 + m33;

		int32 index = 0;
		T big2 = w2;
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

		T big = Sqrt(big2 + static_cast<T>(1.0)) * static_cast<T>(0.5);
		T mult = static_cast<T>(0.25) / big;

		Quat<T> result;

		switch (index)
		{
		case 0: {
			T x = (m23 - m32) * mult;
			T y = (m31 - m13) * mult;
			T z = (m12 - m21) * mult;
			T w = big;

			result = Quat<T>(x, y, z, w);

			break;
		}
		case 1: {
			T x = big;
			T y = (m12 + m21) * mult;
			T z = (m31 + m13) * mult;
			T w = (m23 - m32) * mult;

			result = Quat<T>(x, y, z, w);

			break;
		}
		case 2: {
			T x = (m12 + m21) * mult;
			T y = big;
			T z = (m23 + m32) * mult;
			T w = (m31 - m13) * mult;

			result = Quat<T>(x, y, z, w);

			break;
		}
		case 3: {
			T x = (m31 + m13) * mult;
			T y = (m23 + m32) * mult;
			T z = big;
			T w = (m12 - m21) * mult;

			result = Quat<T>(x, y, z, w);

			break;
		}
		default:
			Assert(0);
			return Quat<T>(0, 0, 0, 1);
		}

		return result;
	}

	template<typename T>
	inline String ToString(const Mat4<T> &a)
	{
		StringStream ss;
		String space = "            ";

		ss << "| " << a.ptr[0] << space << a.ptr[1] << space << a.ptr[2] << space << a.ptr[3] << " |" << '\n';
		ss << "| " << a.ptr[4] << space << a.ptr[5] << space << a.ptr[6] << space << a.ptr[7] << " |" << '\n';
		ss << "| " << a.ptr[8] << space << a.ptr[9] << space << a.ptr[10] << space << a.ptr[11] << " |" << '\n';
		ss << "| " << a.ptr[12] << space << a.ptr[13] << space << a.ptr[14] << space << a.ptr[15] << " |" << '\n';

		return ss.str();
	}

	template<typename T>
	inline void Print(const Mat4<T> &m)
	{
		for (int32 i = 0; i < 16; i++)
		{
			std::cout << m.ptr[i] << " ";
		}
		std::cout << std::endl;
	}

	template<typename T>
	inline void PrintPretty(const Mat4<T> &m)
	{
		std::cout << ToString(m) << std::endl;
	}

	template<typename T>
	inline constexpr Mat4<T> operator /(const Mat4<T> &a, const T &b)
	{
		Mat4<T> result;

		result.row0 = a.row0 / b;
		result.row1 = a.row1 / b;
		result.row2 = a.row2 / b;
		result.row3 = a.row3 / b;

		return result;
	}
	
	template<typename T>
	inline constexpr Mat4<T> operator +(const Mat4<T> &a, const Mat4<T> &b)
	{
		Mat4<T> result(1);

		result.row0 = a.row0 + b.row0;
		result.row1 = a.row1 + b.row1;
		result.row2 = a.row2 + b.row2;
		result.row3 = a.row3 + b.row3;

		return result;
	}

	template<typename T>
	inline constexpr Mat4<T> operator *(const Mat4<T> &a, const Mat4<T> &b)
	{
		Mat4<T> result(1);
		
		for (int32 i = 0; i < 4; i++)
		{
		
			for (int32 y = 0; y < 4; y++)
			{
			
				Vec4<T> col(0, 0, 0, 0);
				for (int32 x = 0; x < 4; x++)
				{
					col[x] = GetMatrixElement(b, x, y);
				}
				
				result.ptr[4 * i + y] = Dot(col, a[i]);
			}
		}

		return result;
	}

	template<typename T>
	inline constexpr Vec4<T> operator *(const Vec4<T> &a, const Mat4<T> &b)
	{
		Vec4<T> result(0, 0, 0, 0);

		for (uint32 i = 0; i < 4; i++)
		{
			Vec4<T> col = GetColumn(b, i);
			result[i] = Dot(col, a);
		}

		return result;
	}

	//************************************
	// Other Functions
	//************************************

	template<typename T>
	inline constexpr Vec4<T> GetNormalisedDeviceCoordinates(const T &window_width, const T &window_height, const T &mouse_x, const T &mouse_y)
	{
		// @NOTE: This is actualy clip space when the vec4 with -1 and 1
		T x = static_cast<T>(2.0) * mouse_x / window_width - static_cast<T>(1.0);
		T y = -(static_cast<T>(2.0) * mouse_y / window_height - static_cast<T>(1.0));
		T z = static_cast<T>(-1.0);
		T w = static_cast<T>(1.0);
		
		Vec4<T> result = Vec4<T>(x, y, z, w);		

		return result;
	}

	template<typename T>
	inline constexpr Vec4<T> ToViewCoords(const Mat4<T> &projection_matrix, const Vec4<T> &viewCoords)
	{
		Mat4<T> invproj = Inverse(projection_matrix);

		Vec4<T> result = viewCoords * invproj;

		return result;
	}

	template<typename T>
	inline constexpr Vec3<T> ToWorldCoords(const Mat4<T> &view_matrix, const Vec4<T> &viewCoords)
	{
		Mat4<T> invView = Inverse(view_matrix);
		Vec4<T> worldSpace = viewCoords * invView;
		Vec3<T> result = Vec3<T>(worldSpace.x, worldSpace.y, worldSpace.z);

		return result;
	}
	
	template<typename T>
	inline constexpr Mat4<T> Perspective(const T &dfovy, const T &aspect, const T &fnear, const T &ffar)
	{
		Mat4<T> result(1);

		T fovy = DegToRad(dfovy);
		T half_tan_fovy = Tan(fovy / static_cast<T>(2));
		
		result.row0 = Vec4<T>( (static_cast<T>(1) / (aspect * half_tan_fovy)) , static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		result.row1 = Vec4<T>(static_cast<T>(0), (static_cast<T>(1) / half_tan_fovy), static_cast<T>(0), static_cast<T>(0));

		T a = -(ffar + fnear) / (ffar - fnear);
		T b = (static_cast<T>(-2) * ffar * fnear) / (ffar - fnear);

		result.row2 = Vec4<T>(static_cast<T>(0), static_cast<T>(0), a, static_cast<T>(-1));
		result.row3 = Vec4<T>(static_cast<T>(0), static_cast<T>(0), b, static_cast<T>(0));

		return result;
	}

	template<typename T>
	inline constexpr Mat4<T> Orthographic(const T &left, const T &right, const T &top, const T &bottom, const T &_near, const T &_far)
	{
		Mat4<T> result(1);
	
		result.row0 = Vec4<T>(static_cast<T>(2) / (right - left), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		result.row1 = Vec4<T>(static_cast<T>(0), static_cast<T>(2) / (top - bottom), static_cast<T>(0), static_cast<T>(0));
		result.row2 = Vec4<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-2) / (_far - _near), static_cast<T>(0));
		result.row3 = Vec4<T>(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(_far + _near) / (_far - _near), static_cast<T>(1));

		return result;
	}

	template<typename T>
	inline constexpr Mat4<T> LookAt(const Vec3<T> &position, const Vec3<T> &target, const Vec3<T> &up)
	{
		// @NOTE: This code is column major, we just tranpose
		Vec3<T> camera_reverse_direction = Normalize((target - position));
		Vec3<T> basis_right = Normalize(Cross(camera_reverse_direction, up));
		Vec3<T> basis_up = Cross(basis_right, camera_reverse_direction);
		Vec3<T> basis_forward = Normalize(Cross(basis_up, basis_right));

		Mat4<T> rotation(static_cast<T>(1));

		rotation.row0 = Vec4f(basis_right, static_cast<T>(0));
		rotation.row1 = Vec4f(basis_up, static_cast<T>(0));
		rotation.row2 = Vec4f(camera_reverse_direction * static_cast<T>(-1.0), static_cast<T>(0));
		Mat4<T> translation(static_cast<T>(1));

		translation.row0 = Vec4f(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), -position.x);
		translation.row1 = Vec4f(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), -position.y);
		translation.row2 = Vec4f(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), -position.z);

		Mat4<T> result = Transpose(rotation * translation);

		return result;
	}
}