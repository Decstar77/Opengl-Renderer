#pragma once
#include "../Core.h"
#include <math.h>
#include <random> 
#include <intrin.h>
#include <xmmintrin.h>

//============       NOES       ============//

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





	struct  Vec3
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
					real32 pad0;
					real32 pad1;
				};
			};
		};

		Vec3()
		{
			data = _mm_set_ps(0, 0, 0, 0);
		}

		Vec3(const real32 &x, const real32 &y, const real32 &z)
		{
			data = _mm_set_ps(0, z, y, x);
		}

		Vec3(const real32 &all)
		{
			data = _mm_set_ps(0, all, all, all);
		}

		Vec3(__m128 _data)
		{
			data = _data;
		}

		real32& operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 3);
			return (&x)[index];
		}

		real32 operator[](const int32 &index) const
		{
			Assert(index >= 0 && index < 3);
			return (&x)[index];
		}
	};

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
		
		Vec4(const Vec3 &xyz, const real32 &_w)
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

		Quat(const Vec3 &xyz, const real32 &_w)
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
				Vec3 row0;
				Vec3 row1;
				Vec3 row2;
			};
		};
		Mat3()
		{
			row0 = Vec3(1, 0, 0);
			row1 = Vec3(0, 1, 0);
			row2 = Vec3(0, 0, 1);
		}

		Mat3(const real32 &a)
		{
			row0 = Vec3(a, 0, 0);
			row1 = Vec3(0, a, 0);
			row2 = Vec3(0, 0, a);
		}
		Mat3(const Vec3 &_row0, const Vec3 &_row1, const Vec3 &_row2)
		{
			row0 = _row0;
			row1 = _row1;
			row2 = _row2;
		}

		Vec3& operator[](const int32 &index)
		{
			Assert(index >= 0 && index < 3);
			return (&row0)[index];
		}

		Vec3 operator[](const int32 &index) const
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
		
		Mat4(const Mat3 &a, const Vec3 &translation)
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
				Vec3 right;
				Vec3 upward;
				Vec3 forward;
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
		
		Basis(const Vec3 &right, const Vec3 &upward, const Vec3 &forward)
		{
			this->right = right;
			this->upward = upward;
			this->forward = forward;
		}
	};


	//************************************
	// Quaterion functions
	//************************************

	Quat EulerToQuat(const Vec3 &euler_angle);
	
	Vec3 QuatToEuler(const Quat &q);

	Quat Slerp(const Quat &a, const Quat &b, const real32 &t);

	Quat Conjugate(const Quat &a);

	Quat Normalize(const Quat &a);

	Mat3 QuatToMat3(const Quat &q);

	Mat4 QuatToMat4(const Quat &q);
		
	real32 Mag(const Quat &a);

	String ToString(const Quat &a);

	bool Equal(const Quat &q1, const Quat &q2, const real32 &epsilon = FLOATING_POINT_ERROR_PRESCION);

	Vec3 Rotate(const Quat &r, const Vec3 &point);

	Vec3 Rotate(const real32 &d_angle, const Vec3 &point, const Vec3 &axis);

	Quat Rotate(const Quat &q, const real32 &d_angle, Vec3 axis);

	Quat Rotate(const Quat &a, const Quat &b);

	//************************************
	// Coordinate spaces
	//************************************

	Vec4 GetNormalisedDeviceCoordinates(const real32 &window_width, const real32 &window_height, const real32 &mouse_x, const real32 &mouse_y);

	Vec4 ToViewCoords(const Mat4 &projection_matrix, const Vec4 &viewCoords);

	Vec3 ToWorldCoords(const Mat4 &view_matrix, const Vec4 &viewCoords);

	//************************************
	// Vector2 
	//************************************



	//************************************
	// Vector3
	//************************************

	real32  Mag(const Vec3 &a);
	
	real32  Dot(const Vec3 &a, const Vec3 &b);

	real32 Distance(const Vec3 &a, const Vec3 &b);

	int32 AbsMaxIndex(const Vec3 &a);

	int32 MaxIndex(const Vec3 &a);
	
	real32 SqrdDistance(const Vec3 &a, const Vec3 &b);
	
	Vec3 Cross(const Vec3 &a, const Vec3 &b);
	
	Vec3 Normalize(const Vec3 &a);

	Vec3 Vec4ToVec3(const Vec4 &a);	

	bool CompareVec(const Vec3 &a, const Vec3 &b, const real &epsilon = FLOATING_POINT_ERROR_PRESCION);

	String ToString(const Vec3 &a);

	//************************************
	// Vector4
	//************************************

	real32 Mag(const Vec4 &a);
	
	real32 Dot(const Vec4 &a, const Vec4 &b);
	
	Vec4 Normalize(const Vec4 &a);

	Vec4 Vec3ToVec4(const Vec3 &a, const real32  &w);

	String ToString(const Vec4 &a);

	//************************************
	// Matrix 3x3
	//************************************

	real32 GetMatrixElement(const Mat3 &a, const int32 &row, const int32 &col);
	
	Vec3 GetColumn(const Mat3 &a, const uint32 &col);

	Vec3 DecomposeToScale(const Mat3 &a);

	Mat3 DecomposeToRotationMatrix(const Mat3 &a);
	
	String ToString(const Mat3 &a);

	real32 Det(const Mat3 &a);

	Mat3 ScaleDirection(const Mat3 &a, const real32 &k, Vec3 direction);

	Mat3 Rotate(const Mat3 &a, const real32 &d_angle, Vec3 axis);

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

	Mat4 Translate(Mat4 a, Vec3 translation);

	Mat4 Translate(Mat4 a, real32 length, real32 d_angle, real32 z);

	Mat4 Translate(Mat4 a, Polar_coord p_coord);

	Mat4 Rotate(Mat4 a, real32 dangle, Vec3 axis);

	Quat Mat4ToQuat(const Mat4 &a);

	Mat4 ScaleDirection(Mat4 a, real32 k, Vec3 unit_direction, bool should_normalize = true);

	Mat4 ScaleCardinal(Mat4 a, Vec3 direction);
	
	Mat4 CalculateTransformMatrix(const Vec3 &position, const Vec3 &scale, const Quat &rotation);
	
	void Print(const Mat4 &m);

	void PrintPretty(const Mat4 &m);
			
	//************************************
	// Other Functions
	//************************************

	Polar_coord Canonical(real32 r, real32 theta, real32 z);

	Mat4  Perspective(const real32 &dfovy, const real32 &aspect, const real32 &fnear, const real32 &ffar);

	Mat4 Orthographic(const real32 &left, const real32 &right, const real32 &top, const real32 &bottom, const real32 &_near, const real32 &_far);

	Mat4 LookAt(const Vec3 &position, const Vec3 &target, const Vec3 &up);


	//************************************
	// Templated boyos
	//************************************


	   	 
	//************************************
	// Usefull classes for everything
	//************************************

	class Transform 
	{
	public:
		Vec3 position = Vec3(0, 0, 0);
		Vec3 scale = Vec3(1, 1, 1);
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

	inline uint32 SafeTruncateUint64(uint64 Value)
	{
		Assert(Value <= 0xFFFFFFFF);
		uint32 Result = static_cast<uint32>(Value);
		return(Result);
	}

	inline real32 SafeTruncateDouble(real64 Value)
	{
		Assert(Value <= 0xFFFFFFFF);
		float Result = static_cast<float>(Value);
		return(Result);
	}

	inline  real32 DegToRad(real32 degrees)
	{
		return SafeTruncateDouble((PI * degrees) / 180);
	}

	inline  real32 RadToDeg(real32 radians)
	{
		return SafeTruncateDouble((180 * radians) / PI);
	}

	inline real32 Round(real32 val)
	{
		return std::roundf(val * 1000000.0f) / 1000000.0f;
	}

	inline real32 Floor(const real32 &val)
	{
		return std::floor(val);
	}

	inline real32 Ceil(const real32 &val)
	{
		return std::ceil(val);
	}

	inline int32 Sign(const real32 &val)
	{
		return (0 < val) - (val < 0);
	}

	inline real32 Sqrt(const real32 &val)
	{
		return sqrtf(val);
	}

	inline void Swap(real32 *a1, real32 *a2)
	{
		real32 temp = *a1;
		*a1 = *a2;
		*a2 = temp;
	}

	inline float FloatChop(float val, float decimal_count)
	{
		float d_amout = powf(10, decimal_count);
		return static_cast<float>(static_cast<int>(val * d_amout)) / d_amout;
	}

	inline float Clamp(float value, float lowerBound, float upperBound)
	{
		return std::clamp(value, lowerBound, upperBound);
	}

	inline bool fequal(const float &a, const float &b, const float &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return (abs(a - b) < epsilon) ? true : false;
	}

	inline bool fnequal(const float &a, const float &b, const float &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return !fequal(a, b);
	}

	inline bool dequal(const double &a, const double &b, const double &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return (abs(a - b) < epsilon) ? true : false;
	}

	inline bool dnequal(const double &a, const double &b, const double &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return !dequal(a, b);
	}

	inline bool requal(const real32 &a, const real32 &b, const real32 &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return (abs(a - b) < epsilon) ? true : false;
	}

	inline bool rnequal(const real32 &a, const real32 &b, const real32 &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return !requal(a, b);
	}

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


	inline Vec3 Lerp(const Vec3 &a, const Vec3 &b, const real32 &t)
	{
		Vec3 r = Vec3(
			a.x + t * (b.x - a.x),
			a.y + t * (b.y - a.y),
			a.z + t * (b.z - a.z)
		);
		return r;
	}

	inline Vec3 Abs(const Vec3 &a)
	{
		return Vec3(std::abs(a.x), std::abs(a.y), std::abs(a.z));
	}


	//************************************
	// Operator overloads for math structures
	//************************************


	
	inline  Vec3 operator  +(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(_mm_add_ps(a.data, b.data));
	}

	inline  Vec3 operator  -(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(_mm_sub_ps(a.data, b.data));
	}

	inline  Vec3 operator  *(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(_mm_mul_ps(a.data, b.data));
	}

	inline  Vec3 operator  /(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(_mm_div_ps(a.data, b.data));
	}

	inline  Vec3 operator *(const Vec3 &a, const float b)
	{
		return _mm_mul_ps(a.data, _mm_set1_ps(b));
	}

	inline  Vec3 operator *(const float a, const Vec3 &b)
	{
		return _mm_mul_ps(b.data, _mm_set1_ps(a));
	}

	inline  Vec3 operator /(const Vec3 &a, const float b)
	{
		return _mm_div_ps(a.data, _mm_set1_ps(b));
	}

	inline  bool operator  ==(const Vec3 &a, const Vec3 &b)
	{
		int32 *compared = reinterpret_cast<int32*>(&_mm_cmpeq_ps(a.data, b.data));
		return(compared[0] & compared[1] & compared[2] & compared[3]);
	}

	inline  bool operator  !=(const Vec3 &a, const Vec3 &b)
	{
		int32 *compared = reinterpret_cast<int32*>(&_mm_cmpneq_ps(a.data, b.data));
		return(compared[0] | compared[1] | compared[2] | compared[3]);
	}

	inline  void operator +=(Vec3 &a, const Vec3 &b)
	{
		a = Vec3(_mm_add_ps(a.data, b.data));
	}

	inline  void operator -=(Vec3 &a, const Vec3 &b)
	{
		a  = Vec3(_mm_sub_ps(a.data, b.data));
	}
	
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


	Vec3 operator *(const Vec3 &a, const Mat3 &b);

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

	inline Quat operator *(const Quat &a, const Vec3 &b)
	{
		real32 w = -a.x * b.x - a.y - b.y - a.z * b.z;
		real32 x =  a.w * b.x + a.y *b.z - a.z * b.y;
		real32 y =  a.w * b.y + a.z *b.x - a.x * b.z;
		real32 z =  a.w * b.z + a.x *b.y - a.y * b.x;
		return Quat(x, w, z, w);
	}


}