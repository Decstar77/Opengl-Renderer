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
}
