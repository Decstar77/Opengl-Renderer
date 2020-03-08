#include "CosmicNoise.h"


namespace cm
{

	real PerlinNoise::Fade(real t)
	{
		// 6t^5 - 15t^4 + 10t^3
		return t * t * t * (t * (t * 6 - 15) + 10);	
	}
	   
	real PerlinNoise::DotProd(int32 hash, real x, real y)
	{
		Vec3 v = Vec3(x, y, 0);
		int32 count = static_cast<int32>(cases2D.size());
		int32 vcase = hash & (count - 1);
		return Dot(cases2D[vcase], v);
	}

	real PerlinNoise::DotProd(int32 hash, real x, real y, real z)
	{
		Vec3 v = Vec3(x, y, z);
		int32 count = static_cast<int32>(cases3D.size());
		int32 vcase = hash & (count - 1);
		return Dot(cases3D[vcase], v);
	}

	int32 PerlinNoise::Inc(int32 value)
	{
		value++;
		return value & 255;
	}

	real PerlinNoise::Sample(real x, real y)
	{
		// Wraps the value bettween 0 and 255
		int32 yi = (int32)Floor(y) & 255;
		int32 xi = (int32)Floor(x) & 255;

		// Get the decimal part. Ie, where we are in the current unit squares we are.			
		real xf = x - Floor(x);
		real yf = y - Floor(y);

		// Get the index of the current unit unit square vertices
		// This is a nice cheat in that becomes a case in the dot function
		int32 g1 = p[p[xi] + yi];
		int32 g2 = p[p[xi + 1] + yi];
		int32 g3 = p[p[xi] + yi + 1];
		int32 g4 = p[p[xi + 1] + yi + 1];

		// Compute the dot products between the graident vecs and unit square dirs
		real d1 = DotProd(g1, xf, yf);
		real d2 = DotProd(g2, xf - 1, yf);
		real d3 = DotProd(g3, xf, yf - 1);
		real d4 = DotProd(g4, xf - 1, yf - 1);

		// Smooths out the noise. Instead of pure linear interpolation			
		real u = Fade(xf);
		real v = Fade(yf);

		// Find the final value by linearly intperolating the dot products
		real x1 = Lerp(d1, d2, u);
		real x2 = Lerp(d3, d4, u);
		real y1 = Lerp(x1, x2, v);

		// Normalize the result to a range of 0 - 1
		real result = (y1 + 1) / 2.f;

		// Due to floating point errors we clamp the normalized value. Ensuring the range. 
		result = Clamp(result, 0, 1);

		return result;
	}

	real PerlinNoise::Sample(real x, real y, real z)
	{
		// Wraps the value bettween 0 and 255
		int32 xi = static_cast<int32>(Floor(x)) & 255;
		int32 yi = static_cast<int32>(Floor(y)) & 255;
		int32 zi = static_cast<int32>(Floor(z)) & 255;

		// Get the decimal part. Ie, where we are in the current unit cube we are.			
		real xf = x - Floor(x);
		real yf = y - Floor(y);
		real zf = z - Floor(z);		

		// Get the index of the current unit unit cube vertices
		// This is a nice cheat in that becomes a case in the dot function
		int32 aaa, aba, aab, abb, baa, bba, bab, bbb;
		aaa = p[p[p[xi] + yi] + zi];
		aba = p[p[p[xi] + Inc(yi)] + zi];
		aab = p[p[p[xi] + yi] + Inc(zi)];
		abb = p[p[p[xi] + Inc(yi)] + Inc(zi)];
		baa = p[p[p[Inc(xi)] + yi] + zi];
		bba = p[p[p[Inc(xi)] + Inc(yi)] + zi];
		bab = p[p[p[Inc(xi)] + yi] + Inc(zi)];
		bbb = p[p[p[Inc(xi)] + Inc(yi)] + Inc(zi)];

		// Compute the dot products between the graident vecs and unit cube dirs
		real d1 = DotProd(aaa, xf, yf, zf);
		real d2 = DotProd(baa, xf - 1, yf, zf);
		real d3 = DotProd(aba, xf, yf - 1, zf);
		real d4 = DotProd(bba, xf - 1, yf - 1, zf);
		real d5 = DotProd(aab, xf, yf, zf - 1); 
		real d6 = DotProd(bab, xf - 1, yf, zf - 1);
		real d7 = DotProd(abb, xf, yf - 1, zf - 1); 
		real d8 = DotProd(bbb, xf - 1, yf - 1, zf - 1);
        
		// Smooths out the noise. Instead of pure linear interpolation	
		real u = Fade(xf);
		real v = Fade(yf);    
		real w = Fade(zf);

		// Find the final value by linearly intperolating the dot products
		real x1 = Lerp(d1, d2, u);
		real x2 = Lerp(d3, d4, u);
		real x3 = Lerp(d5, d6, u);
		real x4 = Lerp(d7, d8, u);

		real y1 = Lerp(x1, x2, v);
		real y2 = Lerp(x3, x4, v);
		
		real z1 = Lerp(y1, y2, w);

		// Normalize the result to a range of 0 - 1
		real result = (z1 + 1) / 2.f;

		// Due to floating point errors we clamp the normalized value. Ensuring the range. 
		result = Clamp(result, 0, 1);
		
		return result;
	}

	void PerlinNoise::Shuffle ()
	{
		//@TODO: Finish
		Assert(0);
		//uint32 seed = RandomUInt(0, 10000);
		//	std::shuffle(std::begin(p), std::end(p) , seed);
	}



	VectorField::VectorField()
	{

	}

	VectorField::~VectorField()
	{
		FreeMemory();
	}

	void VectorField::Populate(const Vec3 &center, const Vec3 &dimension, uint32 flow, uint32 turbulence)
	{
		size_x = dimension.x;
		size_y = dimension.y;
		size_z = dimension.z;
		this->center = center;
		if (field != nullptr)
		{
			FreeMemory();
		}
		CreateMemory();		

		PerlinNoise pn;

		for (int32 x = 0; x < size_x; x++)
		{
			for (int32 y = 0; y < size_y; y++)
			{
				for (int32 z = 0; z < size_z; z++)
				{
					uint32 frq = flow;
					float xx = ((float)x / (float)size_x) * frq;
					float yy = ((float)y / (float)size_y) * frq;
					float zz = ((float)z / (float)size_z) * frq;
					real pno = pn.Sample(xx, yy, zz);

					
					float angle = pno * 2 * PI;
					Vec3 dir = Vec3(cos(angle), sin(angle), cos(angle));
					field[x][y][z] = dir;
				}
			}
		}

	}

	Vec3 VectorField::Sample(real x, real y, real z)
	{
		int32 xl = (static_cast<int32>(Floor(x)) + size_x / 2);
		int32 xu = (static_cast<int32>(Ceil(x)) + size_x / 2);
		Assert(xl > 0); Assert(xu < size_x);

		int32 yl = (static_cast<int32>(Floor(y)) + size_y / 2);
		int32 yu = (static_cast<int32>(Ceil(y)) + size_y / 2);
		Assert(yl > 0); Assert(yu < size_y);

		int32 zl = (static_cast<int32>(Floor(z)) + size_z / 2);
		int32 zu = (static_cast<int32>(Ceil(z)) + size_z / 2);
		Assert(zl > 0); Assert(zu < size_z);

		real xi = x - Floor(x);
		real yi = x - Floor(y);
		real zi = x - Floor(z);

		Vec3 d1 = field[xl][yl][zl];
		Vec3 d2 = field[xu][yl][zl];
		Vec3 d3 = field[xl][yu][zl];
		Vec3 d4 = field[xu][yu][zl];
		Vec3 d5 = field[xl][yl][zu];
		Vec3 d6 = field[xu][yl][zu];
		Vec3 d7 = field[xl][yu][zu];
		Vec3 d8 = field[xu][yu][zu];

		Vec3 x1 = Lerp(d1, d2, xi);
		Vec3 x2 = Lerp(d3, d4, xi);
		Vec3 x3 = Lerp(d5, d6, xi);
		Vec3 x4 = Lerp(d7, d8, xi);

		Vec3 y1 = Lerp(x1, x2, yi);
		Vec3 y2 = Lerp(x3, x4, yi);

		Vec3 z1 = Lerp(y1, y2, zi);
		
		Vec3 result = Normalize(z1);

		return result;
	}

	void VectorField::CreateMemory()
	{
		field = new Vec3 **[size_x];
		for (int32 y = 0; y < size_y; y++)
		{
			field[y] = new Vec3 *[size_z];
			for (int32 z = 0; z < size_z; z++)
			{
				field[y][z] = new Vec3[size_z];
			}
		}
	}

	void VectorField::FreeMemory()
	{
		if (field != nullptr)
		{
			for (int32 i = 0; i < size_y; i++)
			{
				for (int32 j = 0; j < size_z; j++)
				{
					delete[] field[i][j];
				}
				delete[] field[i];
			}
			delete[] field;
		}
	}



}
