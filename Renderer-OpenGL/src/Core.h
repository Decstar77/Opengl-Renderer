#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

#ifdef _DEBUG
#define Assert(Value) if (!(Value)) {*(int *)0 = 0;}
#else
#define Assert()
#endif
#ifdef _DEBUG

#define LOG(...)	printf(__FUNCTION__); \
					printf(" Line: %d ", __LINE__); \
					std::cout << __VA_ARGS__ << '\n';

#define LOGC(str)	printf(__FUNCTION__); \
					printf(" Line: %d ", __LINE__); \
					printf(str); printf(" \n");

#define LOGC(str, ...) printf(__FUNCTION__); \
					  printf(" Line: %d ", __LINE__); \
					  printf(str, __VA_ARGS__); printf(" \n");
#else
	#define LOG(...)
	#define LOGC(str)
	#define LOGC(str, ...)
#endif

#define PNT_VBO_LAYOUT BufferLayout(std::vector<ShaderDataType>({ ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::Float2 }))
#define PNTTB_VBO_LAYOUT BufferLayout(std::vector<ShaderDataType>({ ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::Float2, ShaderDataType::Float3, ShaderDataType::Float3 }))
#define BUFFER_LAYOUT(...) BufferLayout(std::vector<ShaderDataType>({__VA_ARGS__}))

#define IDENTITY_TEXTURE 0



#define BYTES_TO_MEGABYTES 0.000001f
#define BIT(x) (1 << x)
#define REAL_MAX FLT_MAX
#define	REAK32_MAX FLT_MAX
#define FLOATING_POINT_ERROR_PRESCION 0.0001f


#define MOUSE_KEY_AMOUNT 32
#define KEY_CODE_AMOUNT 1024

typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float real;
typedef float real32;
typedef double real64;

typedef std::string String;
