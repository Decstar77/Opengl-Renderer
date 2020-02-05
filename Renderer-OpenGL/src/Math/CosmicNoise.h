#pragma once
#include "Core.h"
#include "CosmicMath.h"

namespace cm
{
	//Many thanks to this dude
	//https://www.youtube.com/watch?v=MJ3bvCkHJtE
	//https://www.youtube.com/watch?v=iW4nFygKAjw
	class PerlinNoise
	{
	public:
		real Sample(real x, real y);
		real Sample(real x, real y, real z);
		void Shuffle();
	private:
		// Random numbers 0 - 255 inclusive
		int p[256] = { 151,160,137,91,90,15,
			   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
			   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
			   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
			   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
			   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
			   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
			   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
			   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
			   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
			   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
		};

		// Possible directions vectors for unit cubes
		DynaArray<Vec3> cases2D = {
			Vec3(1, 1, 0),
			Vec3(-1, 1, 0),
			Vec3(1, -1, 0),
			Vec3(-1, -1, 0),
			Vec3(sqrt(2), 0, 0),
			Vec3(0, sqrt(2), 0),
			Vec3(-sqrt(2), 0, 0),
			Vec3(0, -sqrt(2), 0)
		};
		
		DynaArray<Vec3> cases3D =
		{
			Vec3(1,1,0),
			Vec3(-1,1,0),
			Vec3(1,-1,0),
			Vec3(-1,-1,0),
			Vec3(1,0,1),
			Vec3(-1,0,1),
			Vec3(1,0,-1),
			Vec3(-1,0,-1),
			Vec3(0,1,1),
			Vec3(0,-1,1),
			Vec3(0,1,-1),
			Vec3(0,-1,-1)
		};

	private:
		real Fade(real t);
		real DotProd(int32 hash, real x, real y);
		real DotProd(int32 hash, real x, real y, real z);
		int32 Inc(int32 value);
	};


	class VectorField
	{
	public:
		VectorField();
		~VectorField();

		void Populate(const Vec3 &center, const Vec3 &dimension, uint32 flow, uint32 turbulence);
		Vec3 Sample(real x, real y, real z);
	private:		
		// Properties
		uint32 size_x = 0;
		uint32 size_y = 0;
		uint32 size_z = 0;
		Vec3 center = Vec3(0);

		// Used for infinite size
		PerlinNoise *perlin_noise;
		
		// 3D Array
		Vec3 ***field = nullptr;

	private:
		void CreateMemory();
		void FreeMemory();
	};




}