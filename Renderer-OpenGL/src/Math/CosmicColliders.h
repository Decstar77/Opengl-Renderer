#pragma once
#include "CosmicMath.h"


namespace cm
{	   	

	


	//class Ray
	//{
	//public:
	//	Ray(Vec3 origin, Vec3 direction) : origin(origin), direction(direction) {}
	//	~Ray() {}
	//
	//	Vec3 origin;
	//	Vec3 direction;
	//
	//	inline Vec3 Travel(float length) { return origin + length * direction; }
	//
	//};




	//class GeometricPrmitive
	//{
	//
	//
	//
	//};
	//
	//
	//class Plane
	//{
	//public:
	//	float d;
	//	Vec3 n = Vec3(0);
	//};
	//
	//class Aabb
	//{
	//public:
	//	Vec3 min = Vec3(FLT_MIN);
	//	Vec3 max = Vec3(FLT_MAX);
	//	Vec3 center = Vec3(0);
	//	Vec3 raduis = Vec3(0);		
	//	
	//	void swap(float &a, float &b)
	//	{
	//		float t = a;
	//		a = b;
	//		b = t;
	//	}
	//
	//	bool CheckCollision(const Ray &r)
	//	{
	//		float tmin = (min.x - r.origin.x) / r.direction.x;
	//		float tmax = (max.x - r.origin.x) / r.direction.x;
	//
	//		if (tmin > tmax) { swap(tmin, tmax); }
	//
	//		float tymin = (min.y - r.origin.y) / r.direction.y;
	//		float tymax = (max.y - r.origin.y) / r.direction.y;
	//
	//		if (tymin > tymax) { swap(tymin, tymax); }
	//
	//		if ((tmin > tymax) || (tymin > tmax)) { return (false); }		
	//
	//		
	//		if (tymin > tmin) { tmin = tymin; }
	//
	//		if (tymax < tmax) { tmax = tymax; }
	//
	//		float tzmin = (min.z - r.origin.z) / r.direction.z;
	//		float tzmax = (max.z - r.origin.z) / r.direction.z;
	//
	//		if (tzmin > tzmax) { swap(tzmin, tzmax); }
	//
	//		if ((tmin > tzmax) || (tzmin > tmax)) { return (false); }
	//
	//		if (tzmin > tmin) {	tmin = tzmin; }
	//
	//		if (tzmax < tmax) { tmax = tzmax; }
	//
	//		float txmin = (min.x - r.origin.x) / r.direction.x;
	//		float txmax = (max.x - r.origin.x) / r.direction.x;
	//
	//		if (txmin > txmax) { swap(txmin, txmax); }
	//
	//		
	//		return (true);
	//	}
	//
	//
	//};


}



