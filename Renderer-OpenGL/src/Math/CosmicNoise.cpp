#include "CosmicNoise.h"


namespace cm
{

	real32 PerlinNoise::Fade(real32 t)
	{
		// 6t^5 - 15t^4 + 10t^3
		return t * t * t * (t * (t * 6 - 15) + 10);	
	}
	   
	real32 PerlinNoise::DotProd(int32 hash, real32 x, real32 y)
	{
		Vec3f v = Vec3f(x, y, 0);
		int32 count = static_cast<int32>(8);
		int32 vcase = hash & (count - 1);
		return Dot(cases2D[vcase], v);
	}

	real32 PerlinNoise::DotProd(int32 hash, real32 x, real32 y, real32 z)
	{
		Vec3f v = Vec3f(x, y, z);
		int32 count = static_cast<int32>(12);
		int32 vcase = hash & (count - 1);
		return Dot(cases3D[vcase], v);
	}

	int32 PerlinNoise::Inc(int32 value)
	{
		value++;
		return value & 255;
	}

	real32 PerlinNoise::Sample(real32 x, real32 y)
	{
		// Wraps the value bettween 0 and 255
		int32 yi = (int32)Floor(y) & 255;
		int32 xi = (int32)Floor(x) & 255;

		// Get the decimal part. Ie, where we are in the current unit squares we are.			
		real32 xf = x - Floor(x);
		real32 yf = y - Floor(y);

		// Get the index of the current unit unit square vertices
		// This is a nice cheat in that becomes a case in the dot function
		int32 g1 = p[p[xi] + yi];
		int32 g2 = p[p[xi + 1] + yi];
		int32 g3 = p[p[xi] + yi + 1];
		int32 g4 = p[p[xi + 1] + yi + 1];

		// Compute the dot products between the graident vecs and unit square dirs
		real32 d1 = DotProd(g1, xf, yf);
		real32 d2 = DotProd(g2, xf - 1, yf);
		real32 d3 = DotProd(g3, xf, yf - 1);
		real32 d4 = DotProd(g4, xf - 1, yf - 1);

		// Smooths out the noise. Instead of pure linear interpolation			
		real32 u = Fade(xf);
		real32 v = Fade(yf);

		// Find the final value by linearly intperolating the dot products
		real32 x1 = Lerp(d1, d2, u);
		real32 x2 = Lerp(d3, d4, u);
		real32 y1 = Lerp(x1, x2, v);

		// Normalize the result to a range of 0 - 1
		real32 result = (y1 + 1) / 2.f;

		// Due to floating point errors we clamp the normalized value. Ensuring the range. 
		result = Clamp(result, 0.0f, 1.0f);

		return result;
	}

	real32 PerlinNoise::Sample(real32 x, real32 y, real32 z)
	{
		// Wraps the value bettween 0 and 255
		int32 xi = static_cast<int32>(Floor(x)) & 255;
		int32 yi = static_cast<int32>(Floor(y)) & 255;
		int32 zi = static_cast<int32>(Floor(z)) & 255;

		// Get the decimal part. Ie, where we are in the current unit cube we are.			
		real32 xf = x - Floor(x);
		real32 yf = y - Floor(y);
		real32 zf = z - Floor(z);		

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
		real32 d1 = DotProd(aaa, xf, yf, zf);
		real32 d2 = DotProd(baa, xf - 1, yf, zf);
		real32 d3 = DotProd(aba, xf, yf - 1, zf);
		real32 d4 = DotProd(bba, xf - 1, yf - 1, zf);
		real32 d5 = DotProd(aab, xf, yf, zf - 1); 
		real32 d6 = DotProd(bab, xf - 1, yf, zf - 1);
		real32 d7 = DotProd(abb, xf, yf - 1, zf - 1); 
		real32 d8 = DotProd(bbb, xf - 1, yf - 1, zf - 1);
        
		// Smooths out the noise. Instead of pure linear interpolation	
		real32 u = Fade(xf);
		real32 v = Fade(yf);    
		real32 w = Fade(zf);

		// Find the final value by linearly intperolating the dot products
		real32 x1 = Lerp(d1, d2, u);
		real32 x2 = Lerp(d3, d4, u);
		real32 x3 = Lerp(d5, d6, u);
		real32 x4 = Lerp(d7, d8, u);

		real32 y1 = Lerp(x1, x2, v);
		real32 y2 = Lerp(x3, x4, v);
		
		real32 z1 = Lerp(y1, y2, w);

		// Normalize the result to a range of 0 - 1
		real32 result = (z1 + 1) / 2.f;

		// Due to floating point errors we clamp the normalized value. Ensuring the range. 
		result = Clamp(result, 0.0f, 1.0f);
		
		return result;
	}

	void PerlinNoise::Shuffle ()
	{
		//@TODO: Finish
		Assert(0);
		//uint32 seed = RandomUInt(0, 10000);
		//	std::shuffle(std::begin(p), std::end(p) , seed);
	}
}
