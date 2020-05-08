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
	//===============================================================//
	/*
		I've taken a OOP approach in this section for exploritory purposes.
		Ie, its see the advantages and disadvantages
	*/
	//===============================================================//
	
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

	inline uint32 SafeTruncateUint64(uint64 value)
	{
		Assert(value <= 0xFFFFFFFF);
		uint32 result = static_cast<uint32>(value);
		return result;
	}

	inline real32 SafeTruncateDouble(real64 value)
	{
		Assert(value <= 0xFFFFFFFF);
		real32 result = static_cast<real32>(value);
		return result;
	}

	template<typename T>
	inline T Abs(const T &a)
	{
		return std::abs(a);
	}

	template<typename T>
	inline T RandomUnillateral() // @NOTE: 0 to 1
	{
		T res = (T)rand() / (T)RAND_MAX;
		return res;
	}

	template<typename T>
	inline T RandomBillateral() // @NOTE: -1 to 1
	{
		T res = static_cast<T>(2.0) * RandomUnillateral<T>() - static_cast<T>(1.0);
		return res;
	}

	template<typename T>
	inline T RandomReal(const T &min, const T &max) // @NOTE: min to max - 1
	{
		T c = max - min;
		return  c == 0 ? min : min + static_cast <T> (rand()) / (static_cast <T> (RAND_MAX / c));
	}

	template<typename T>
	inline T RandomUInt(const T &min, const T &max) // @NOTE: min to max - 1
	{
		T c = max - min;
		return c == 0 ? min : rand() % (max - min) + min;
	}

	template<typename T>
	inline T Normalize(const T &value, const T &min, const T &max)
	{
		return (value - min) / (max - min);
	}

	template<typename T>
	inline T Lerp(const T &a, const T &b, const T &t)
	{
		return a * (static_cast<T>(1.0) - t) + b * t;
	}
	
	template<typename T>
	inline T DegToRad(T degrees)
	{
		return (static_cast<T>(PI) * degrees) / static_cast<T>(180.0);
	}

	template<typename T>
	inline T RadToDeg(T radians)
	{
		return (static_cast<T>(180.0) * radians) / static_cast<T>(PI);
	}

	template<typename T>
	inline T Round(T val)
	{
		return std::round(val * static_cast<T>(1000000.0)) / static_cast<T>(1000000.0f);
	}

	template<typename T>
	inline T Floor(const T &val)
	{
		return std::floor(val);
	}

	template<typename T>
	inline T Ceil(const T &val)
	{
		return std::ceil(val);
	}

	template<typename T>
	inline int32 Sign(const T &val)
	{
		return (static_cast<T>(0.0) < val) - (val < static_cast<T>(0));
	}

	template<typename T>
	inline T Sqrt(const T &val)
	{
		return std::sqrt(val);
	}

	template<typename T>
	inline void Swap(T *a1, T *a2)
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
	inline T Clamp(const T &value, const T &lowerBound, const T &upperBound)
	{
		return std::clamp(value, lowerBound, upperBound);
	}

	template<typename T>
	inline bool32 Equal(const T &a, const T &b, const T &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return (Abs(a - b) < epsilon) ? true : false;
	}

	template<typename T>
	inline bool32 Nequal(const T &a, const T &b, const T &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return !Equal(a, b);
	}
	
	//************************************
	// Math structures
	//************************************

	template<typename T>
	struct  Vec2
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

	template<typename T>
	struct  Vec3
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
	Vec3<T> Normalize(const Vec3<T> &a)
	{
		T m = Mag(a);
		Vec3<T> result = Vec3<T>(a.x / m, a.y / m, a.z / m);
		return result;
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
	Vec3<T> Cross(const Vec3<T> &a, const Vec3<T> &b)
	{
		T x = a.y * b.z - b.y * a.z;
		T y = a.z * b.x - b.z * a.x;
		T z = a.x * b.y - b.x * a.y;
		return Vec3<T>(x, y, z);
	}

	template<typename T>
	String ToString(const Vec3<T> &a)
	{
		StringStream ss;
		ss << '(' << a.x << ", " << a.y << ", " << a.z << ')';
		return ss.str();
	}

	bool CompareVec(const Vec3f &a, const Vec3f &b, const real &epsilon = FLOATING_POINT_ERROR_PRESCION);
	
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
	
	
	
	struct  Vec4
	{
		union
		{
			__m128 data;
			real32 ptr[4];
			struct
			{
				union
				{
					real32 x;
					real32 r;
				};
				union
				{
					real32 y;
					real32 g;
				};
				union
				{
					real32 z;
					real32 b;
				};
				union
				{
					real32 w;
					real32 a;
				};
			};
		};

		Vec4()
		{
			data = _mm_set_ps(0, 0, 0, 0);
		}
		
		Vec4(const real32 &x, const real32 &y, const real32 &z, const real32 &w)
		{
			data = _mm_set_ps(w, z, y, x);
		}
		
		Vec4(const real32 &all)
		{
			data = _mm_set_ps(all, all, all, all);
		}
		
		Vec4(const Vec3f &xyz, const real32 &_w)
		{
			x = xyz.x;
			y = xyz.y;
			z = xyz.z;
			w = _w;
		}
		
		Vec4(__m128 _data)
		{
			data = _data;
		}

		real32& operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 4);
			return (&x)[index];
		}

		real32 operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 4);
			return (&x)[index];
		}
	};

	struct Quat
	{
		union
		{
			__m128 data;
			real32 ptr[4];
			struct
			{
				real32 x;
				real32 y;
				real32 z;
				real32 w;
			};
		};

		Quat()
		{
			data = _mm_set_ps(1, 0, 0, 0);
		}

		Quat(const real32 &x, const real32 &y, const real32 &z, const real32 &w)
		{
			data = _mm_set_ps(w, z, y, x);
		}

		Quat(const Vec3f &xyz, const real32 &_w)
		{
			x = xyz.x;
			y = xyz.y;
			z = xyz.z;
			w = _w;
		}

		Quat(__m128 _data)
		{
			data = _data;
		}

		real32& operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 4);
			return (&x)[index];
		}

		real32 operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 4);
			return (&x)[index];
		}
	};

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
				Vec4 row0;
				Vec4 row1;
				Vec4 row2;
				Vec4 row3;
			};
		};

		Mat4()
		{
			row0 = Vec4(1, 0, 0, 0);
			row1 = Vec4(0, 1, 0, 0);
			row2 = Vec4(0, 0, 1, 0);
			row3 = Vec4(0, 0, 0, 1);
		}

		Mat4(const real32 &a)
		{
			row0 = Vec4(a, 0, 0, 0);
			row1 = Vec4(0, a, 0, 0);
			row2 = Vec4(0, 0, a, 0);
			row3 = Vec4(0, 0, 0, a);
		}

		Mat4(const Vec4 &_row0, const Vec4 &_row1, const Vec4 &_row2, const Vec4 &_row3)
		{
			row0 = _row0;
			row1 = _row1;
			row2 = _row2;
			row3 = _row3;
		}
		
		Mat4(const Mat3 &a, const Vec3f &translation)
		{
			row0 = Vec4(a.row0, 0);
			row1 = Vec4(a.row1, 0);
			row2 = Vec4(a.row2, 0);
			row3 = Vec4(translation, 1);
		}

		Vec4 & operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 4);
			return (&row0)[index];
		}

		Vec4 operator[](const int32 &index) const
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

	Quat EulerToQuat(const Vec3f &euler_angle);
	
	Vec3f QuatToEuler(const Quat &q);

	Quat Slerp(const Quat &a, const Quat &b, const real32 &t);

	Quat Conjugate(const Quat &a);

	Quat Normalize(const Quat &a);

	Mat3 QuatToMat3(const Quat &q);

	Mat4 QuatToMat4(const Quat &q);
		
	real32 Mag(const Quat &a);

	String ToString(const Quat &a);

	bool Equal(const Quat &q1, const Quat &q2, const real32 &epsilon = FLOATING_POINT_ERROR_PRESCION);

	Vec3f Rotate(const Quat &r, const Vec3f &point);

	Vec3f Rotate(const real32 &d_angle, const Vec3f &point, const Vec3f &axis);

	Quat Rotate(const Quat &q, const real32 &d_angle, Vec3f axis);

	Quat Rotate(const Quat &a, const Quat &b);

	//************************************
	// Coordinate spaces
	//************************************

	Vec4 GetNormalisedDeviceCoordinates(const real32 &window_width, const real32 &window_height, const real32 &mouse_x, const real32 &mouse_y);

	Vec4 ToViewCoords(const Mat4 &projection_matrix, const Vec4 &viewCoords);

	Vec3f ToWorldCoords(const Mat4 &view_matrix, const Vec4 &viewCoords);

	//************************************
	// Vector2 
	//************************************



	//************************************
	// Vector3
	//************************************
	
	Vec3f Vec4ToVec3(const Vec4 &a);

	//************************************
	// Vector4
	//************************************

	real32 Mag(const Vec4 &a);
	
	real32 Dot(const Vec4 &a, const Vec4 &b);
	
	Vec4 Normalize(const Vec4 &a);

	Vec4 Vec3ToVec4(const Vec3f &a, const real32  &w);

	String ToString(const Vec4 &a);

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

	Quat Mat3ToQuat(const Mat3 &a);

	//************************************
	// Matrix 4x4
	//************************************

	real32 GetMatrixElement(const Mat4 &a, const int32 &row, const int32 &col);
	
	Vec4 GetColumn(const Mat4 &a, const uint32 &col);

	String ToString(const Mat4 &a);

	bool CheckOrthogonal(const Mat4 &a, const real32 tolerance = 0.01);

	Mat3 Adjoint(const Mat4 &a, const int32 &row, const int32 &col);
	
	Mat4 Inverse(const Mat4 &a);

	Mat4 Transpose(Mat4 a);

	real32 Det(const Mat4 &a);

	Mat4 Mat3ToMat4(const Mat3 &a, const Vec4 &b);

	Mat4 Translate(Mat4 a, Vec3f translation);

	Mat4 Translate(Mat4 a, real32 length, real32 d_angle, real32 z);

	Mat4 Translate(Mat4 a, Polar_coord p_coord);

	Mat4 Rotate(Mat4 a, real32 dangle, Vec3f axis);

	Quat Mat4ToQuat(const Mat4 &a);

	Mat4 ScaleDirection(Mat4 a, real32 k, Vec3f unit_direction, bool should_normalize = true);

	Mat4 ScaleCardinal(Mat4 a, Vec3f direction);
	
	Mat4 CalculateTransformMatrix(const Vec3f &position, const Vec3f &scale, const Quat &rotation);
	
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
	// Templated boyos
	//************************************


	   	 
	//************************************
	// Usefull classes for everything
	//************************************

	class Transform 
	{
	public:
		Vec3f position = Vec3f(0, 0, 0);
		Vec3f scale = Vec3f(1, 1, 1);
		Quat rotation = Quat(0, 0, 0, 1);
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


	



	inline  Vec4 operator  +(const Vec4 &a, const Vec4 &b)
	{
		return Vec4(_mm_add_ps(a.data, b.data));
	}

	inline  Vec4 operator  -(const Vec4 &a, const Vec4 &b)
	{
		return Vec4(_mm_sub_ps(a.data, b.data));
	}

	inline  Vec4 operator  *(const Vec4 &a, const Vec4 &b)
	{
		return Vec4(_mm_mul_ps(a.data, b.data));
	}

	inline  Vec4 operator  /(const Vec4 &a, const Vec4 &b)
	{
		return Vec4(_mm_div_ps(a.data, b.data));
	}

	inline  Vec4 operator *(const Vec4 &a, const float &b)
	{
		return _mm_mul_ps(a.data, _mm_set1_ps(b));
	}

	inline  Vec4 operator /(const Vec4 &a, const float &b)
	{
		return _mm_div_ps(a.data, _mm_set1_ps(b));
	}

	inline bool operator  ==(const Vec4 &a, const Vec4 &b)
	{
		int32 *compared = reinterpret_cast<int32*>(&_mm_cmpeq_ps(a.data, b.data));
		return(compared[0] & compared[1] & compared[2] & compared[3]);
	}

	inline bool operator  !=(const Vec4 &a, const Vec4 &b)
	{
		int32 *compared = reinterpret_cast<int32*>(&_mm_cmpneq_ps(a.data, b.data));
		return(compared[0] | compared[1] | compared[2] | compared[3]);
	}	

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
	
	Vec4  operator *(const Vec4 &a, const Mat4 &b);	

	inline Quat operator *(const Quat &q, const Quat &p)
	{
		real32 w = p.w * q.w		 - p.x * q.x		- p.y * q.y			- p.z * q.z;
		real32 x = p.w * q.x		 + p.x * q.w		+ p.y * q.z			- p.z * q.y;
		real32 y = p.w * q.y		 + p.y * q.w		+ p.z * q.x			- p.x * q.z;
		real32 z = p.w * q.z		 + p.z * q.w		+ p.x * q.y			- p.y * q.x;
		return Quat(x, y, z, w);
	}

	inline Quat operator *(const Quat &a, const Vec3f &b)
	{
		real32 w = -a.x * b.x - a.y - b.y - a.z * b.z;
		real32 x =  a.w * b.x + a.y *b.z - a.z * b.y;
		real32 y =  a.w * b.y + a.z *b.x - a.x * b.z;
		real32 z =  a.w * b.z + a.x *b.y - a.y * b.x;
		return Quat(x, w, z, w);
	}


}