#pragma once
#include "../Core.h"
#include <math.h>
#include <random> 
#include <intrin.h>
#include <xmmintrin.h>

//============       NOES       ============//

//TODO all matrices and vectors are floats/real32
//TODO check_orthogonal could be faster
//TODO nothing is tested
//TODO Nomalize/trapsose have SIMD operators, case 
//TODO SIMD more ??
//TODO Single header file full of operators ?!?!?
//TODO Vectors still contain more data than need (vec2)
//============     Constants     ============//
#define PI 3.14159265359

//============Forward Declerations============//
namespace cm
{
	struct  Vec2;
	struct  Vec3;
	struct  Vec4;
	struct  vecN;

	struct  Mat2;
	struct  Mat3;
	struct  Mat4;
	struct  matN;

	struct Quat;
	struct Basis;	
	struct polar_coord;

	class Transform;
	struct Ray;	

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	enum class Space
	{
		None = 0, Object, World, View, Projection
	};

	//============Inline Function Declerations============//

	inline  uint32_t safe_truncate_uint64(uint64_t Value)
	{
		Assert(Value <= 0xFFFFFFFF);
		uint32_t Result = static_cast<uint32_t>(Value);
		return(Result);
	}

	inline  float safe_truncate_double(double Value)
	{
		Assert(Value <= 0xFFFFFFFF);
		float Result = static_cast<float>(Value);
		return(Result);
	}

	inline  float deg_to_rad(float degrees)
	{
		return safe_truncate_double((PI * degrees) / 180);
	}

	inline  float rad_to_deg(float radians)
	{
		return safe_truncate_double((180 * radians) / PI);
	}

	inline float round(float val)
	{
		return std::roundf(val * 1000000.0f) / 1000000.0f;
	}

	inline real Floor(const real &val)
	{
		return std::floor(val);
	}

	inline real Ceil(const real &val)
	{
		return std::ceil(val);
	}

	inline float float_chop(float val, float decimal_count)
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

	inline bool requal(const real &a, const real &b, const real &epsilon = FLOATING_POINT_ERROR_PRESCION)
	{
		return (abs(a - b) < epsilon) ? true : false;
	}

	inline bool rnequal(const real &a, const real &b, const real &epsilon = FLOATING_POINT_ERROR_PRESCION)
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
	
	inline real RandomUnillateral() // 0 to 1
	{
		real res = (real)rand() / (real)RAND_MAX;
		return res;
	}

	inline real RandomBillateral() // -1 to 1
	{
		real res = 2.0f * RandomUnillateral() - 1;
		return res;
	}
	
	inline real RandomReal(real min, real max) // min to max - 1
	{
		real c = max - min;
		return  c == 0 ? min : min + static_cast <real> (rand()) / (static_cast <real> (RAND_MAX / c));		
	}

	inline uint32 RandomUInt(uint32 min, uint32 max) // min to max - 1
	{
		uint32 c = max - min;
		return c == 0 ? min : rand() % (max - min) + min;
	}

	inline real Fit(real value, real min, real max)
	{		
		return (value - min) / (max - min);
	}
	
	inline real Lerp(real a, real b, real t)
	{
		return a *(1 - t) + b * t;
	}

	//============Functions Declerations============//

	//============        Other       ============//

	polar_coord  canonical(float r, float theta, float z);

	uint32 GetShaderDataTypeComponentCount(ShaderDataType type);
	
	uint32 GetShaderDataTypeSize(ShaderDataType type);
		
	Mat4  perspective(float fovy, float aspect, float near, float far);

	Mat4 Orthographic(float left, float right, float top, float bottom, float _near, float _far);
	   	 
	Mat4 LookAt(Vec3 position, Vec3 target, Vec3 up);

	//============        Vectors       ============//

	//=========Orientation

	Quat EulerToQuat(const Vec3 &euler_angle);
		
	Mat4 quat_to_mat4(const Quat &q);

	Quat conjugate(const Quat &a);

	float Mag(const Quat &a);

	Quat Normalize(const Quat &a);

	std::string toString(const Quat &a);

	Vec3 rotate(const Quat &r, const Vec3 &point);

	Vec3 rotate(const float &d_angle, const Vec3 &point, const Vec3 &axis);

	//========Coordinate Spaces

	Vec4 GetNormalisedDeviceCoordinates(const float &window_width, const float &window_height, const float &mouse_x, const float &mouse_y);

	Vec4 ToViewCoords(Mat4 &projection_matrix, Vec4 &viewCoords);

	Vec3 ToWorldCoords(Mat4 &view_matrix, Vec4 &viewCoords);

	//========Vector2

	float Mag(const Vec2 &a);

	Vec2 Normalize(const Vec2 &a);

	//========Vector3

	float  Mag(Vec3 a);
	
	float  Dot(Vec3 a, Vec3 b);

	float sqrd_distance(const Vec3 &a, const Vec3 &b);
	
	Vec3   Cross(Vec3 a, Vec3 b);
	
	Vec3   Normalize(Vec3 a);

	Vec3  vec4tovec3(Vec4 &a);	

	bool CompareVec(const Vec3 &a, const Vec3 &b, const real &epsilon = FLOATING_POINT_ERROR_PRESCION);

	std::string  toString(Vec3 a);

	//======Vector 4

	float  Mag(Vec4 a);
	
	float   Dot(Vec4 a, Vec4 b);
	
	Vec4   Normalize(Vec4 a);

	Vec4  vec3tovec4(Vec3 &a, float w);

	std::string  toString(Vec4 a);

	//========Mat3
	
	float  det(Mat3 a);

	Mat3  mat4tomat3(Mat4 &a);


	//========Mat4

	float  get(Mat4 a, int row, int col);
	
	Vec4  getColumn(Mat4 a, uint8 col);

	std::string  toString(Mat4 a);

	bool  check_orthogonal(Mat4 a, float tolerance = 0.01);

	Mat3  adjoint(Mat4 a, int row, int col);
	
	Mat4  Inverse(Mat4 a);

	Mat4  transpose(Mat4 a);

	float  det(Mat4 a);

	Mat4  mat3tomat4(Mat3 &a, Vec4 &b);

	Mat4  translate(Mat4 a, Vec3 translation);

	Mat4  translate(Mat4 a, float length, float d_angle, float z);

	Mat4  translate(Mat4 a, polar_coord p_coord);

	Mat4  rotate(Mat4 a, float dangle, Vec3 axis, bool should_normalize = true);

	Mat4  scale_direction(Mat4 a, float k, Vec3 unit_direction, bool should_normalize = true);

	Mat4  scale_cardinal(Mat4 a, Vec3 direction);
	
	Mat4 CalculateTransformMatrix(const Vec3 &position, const Vec3 &scale, const Quat &rotation);
	

	//============Struct Definitions============//

	//Vectors
	struct  Vec2
	{
		union
		{
			float arr[2];
			struct
			{
				float x;
				float y;
			};
		};
		Vec2()
		{
			x = 0; y = 0;
		}
		Vec2(float x, float y)
		{
			this->x = x; this->y = y;
		}
		Vec2(float all)
		{
			x = all;
			y = all;
		}
	};

	struct  Vec3
	{
		union
		{
			__m128 data;
			float arr[4];
			struct
			{
				float x;
				float y;
				float z;			
				float pad;
			};
		};
		Vec3()
		{
			data = _mm_set_ps(0, 0,0,0);
		}
		Vec3(float x, float y, float z)
		{
			data = _mm_set_ps(0, z, y, x);
		}
		Vec3(float all)
		{
			data = _mm_set_ps(0, all, all, all);
		}
		Vec3(__m128 _data)
		{
			data = _data;
		}
	};

	struct  Vec4
	{
		union
		{
			__m128 data;
			float arr[4];
			struct
			{
				union 
				{
					float x;
					float r;
				};
				union
				{
					float y;
					float g;
				};
				union
				{
					float z;
					float b;
				};
				union
				{
					float w;
					float a;
				};
			};
		};
		Vec4()
		{
			data = _mm_set_ps(0, 0, 0, 0);
		}
		Vec4(float x, float y, float z, float w)
		{
			data = _mm_set_ps(w, z, y, x);
		}
		Vec4(float all)
		{
			data = _mm_set_ps(all, all, all, all);
		}
		Vec4(Vec3 xyz, float _w)
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
	};
	struct Quat
	{
		union
		{
			Vec3 vec;
			__m128 data;
			float arr[4];
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
		};

		Quat()
		{
			data = _mm_set_ps(1, 0, 0, 0);
		}
		
		Quat(float x, float y, float z, float w)
		{
			data = _mm_set_ps(w, z, y, x);
		}

		Quat(Vec3 xyz, float _w)
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
	};

	//Matrices
	struct  Mat3
	{
		union
		{
			float arr[9];
			Vec3 data[3];
			struct
			{
				Vec3 row0;
				Vec3 row1;
				Vec3 row2;
			};
		};
		Mat3(float a)
		{
			row0 = Vec3(a, 0, 0);
			row1 = Vec3(0, a, 0);
			row2 = Vec3(0, 0, a);
		}
		Mat3(Vec3 _row0, Vec3 _row1, Vec3 _row2)
		{
			row0 = _row0;
			row1 = _row1;
			row2 = _row2;
		}
	};

	struct  Mat4
	{
		union
		{
			float arr[16];
			Vec4 data[4];
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

		Mat4(float a)
		{
			row0 = Vec4(a, 0, 0, 0);
			row1 = Vec4(0, a, 0, 0);
			row2 = Vec4(0, 0, a, 0);
			row3 = Vec4(0, 0, 0, a);
		}
		Mat4(Vec4 _row0, Vec4 _row1, Vec4 _row2, Vec4 _row3)
		{
			row0 = _row0;
			row1 = _row1;
			row2 = _row2;
			row3 = _row3;
		}
		
	};

	struct  polar_coord
	{
		real r;
		real theta;
		real z;		
	};

	struct Basis
	{	
		Vec3 forward = Vec3(0,0,1);
		Vec3 upward	 = Vec3(0,1,0);
		Vec3 right	 = Vec3(1,0,0);
	};

	class Transform //@TODO: Move defintions into own cpp file
	{
	public:
		Vec3 position = Vec3(0,0,0);
		Vec3 scale = Vec3(1, 1, 1);
		Quat rotation = Quat(0,0,0,1);
		Basis basis;

	public:
		Mat4 CalcTransformMatrix() const
		{
			return CalculateTransformMatrix(position, scale, rotation);
		}
	};


	struct Ray
	{
		Vec3 origin;
		Vec3 direction;
	};


	////============Vector2 Operators============////

	inline  Vec2 operator  +(const Vec2 &a, const Vec2 &b)
	{
		return Vec2(a.x + b.x, a.y + b.y);
	}

	inline  Vec2 operator  -(const Vec2 &a, const Vec2 &b)
	{
		return Vec2(a.x - b.x, a.y- b.y);
	}

	inline  Vec2 operator  /(const Vec2 &a, float b)
	{
		return Vec2(a.x / b, a.y / b);
	}

	inline  bool operator  ==(const Vec2 &a, const Vec2 &b)
	{
		return (a.x == b.x && a.y == b.y);
	}

	////============Vector3 Operators============////

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
		int *compared = reinterpret_cast<int*>(&_mm_cmpeq_ps(a.data, b.data));
		return(compared[0] & compared[1] & compared[2] & compared[3]);
	}

	inline  bool operator  !=(const Vec3 &a, const Vec3 &b)
	{
		int *compared = reinterpret_cast<int*>(&_mm_cmpneq_ps(a.data, b.data));
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

	////============Vector4 Operators============////

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

	inline  bool operator  ==(const Vec4 &a, const Vec4 &b)
	{
		int *compared = reinterpret_cast<int*>(&_mm_cmpeq_ps(a.data, b.data));
		return(compared[0] & compared[1] & compared[2] & compared[3]);
	}

	inline  bool operator  !=(const Vec4 &a, const Vec4 &b)
	{
		int *compared = reinterpret_cast<int*>(&_mm_cmpneq_ps(a.data, b.data));
		return(compared[0] | compared[1] | compared[2] | compared[3]);
	}

	////============Matrix Operators============////
	
	Mat4  operator /(Mat4 a, float b);

	Mat4  operator *(const Mat4 &a, const Mat4&b);

	Mat4 operator +(const Mat4 &a, const Mat4 &b);
	
	Vec4  operator *(const Vec4 &a, const Mat4 &b);
			
	////============Quaterion Operators============////

	inline Quat operator *(const Quat &q, const Quat &p)
	{
		float w = p.w * q.w		 - p.x * q.x		- p.y * q.y			- p.z * q.z;
		float x = p.w * q.x		 + p.x * q.w		+ p.y * q.z			- p.z * q.y;
		float y = p.w * q.y		 + p.y * q.w		+ p.z * q.x			- p.x * q.z;
		float z = p.w * q.z		 + p.z * q.w		+ p.x * q.y			- p.y * q.x;
		return Quat(x, y, z, w);
	}

	inline Quat operator *(const Quat &a, const Vec3 &b)
	{
		float w = -a.x * b.x - a.y - b.y - a.z * b.z;
		float x =  a.w * b.x + a.y *b.z - a.z * b.y;
		float y =  a.w * b.y + a.z *b.x - a.x * b.z;
		float z =  a.w * b.z + a.x *b.y - a.y * b.x;
		return Quat(x, w, z, w);
	}

	inline Vec3 Lerp(Vec3 a, Vec3 b, float t)
	{
		Vec3 r = Vec3(
			a.x + t * (b.x - a.x),
			a.y + t * (b.y - a.y),
			a.z + t * (b.z - a.z)
		);
		return r;
	}

	typedef Vec4 Colour;
}