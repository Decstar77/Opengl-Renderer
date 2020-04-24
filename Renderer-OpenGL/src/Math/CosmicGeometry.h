#pragma once
#include "Core.h"
#include "CosmicMath.h"
namespace cm
{
	//===============================================================//
	/*
		Code is slow compared to other but understandable as this is a first attempt
	*/
	//===============================================================//

	class Ray
	{
	public:
		Vec3 origin;
		Vec3 direction;

	public:
		Vec3 Travel(real32 dist);

	public:
		Ray();
		Ray(const Vec3 &origin, const Vec3 &direction);
		~Ray();
	};
	   	  	
	
	class GeometricCollidable
	{
		// @NOTE: Think of a better name lol
	public:
		virtual bool CheckCollision(const Ray &r) const = 0;
	};


	class Plane : public GeometricCollidable
	{
	public:
		Vec3 origin;
		Vec3 normal;

	public:
		virtual bool CheckCollision(const Ray &r) const override;
		real32 CheckDistance(Ray &r) const;

	public:
		Plane();
		Plane(const Vec3 &origin, const Vec3 &normal);
		~Plane();
	};

	class Sphere : public GeometricCollidable
	{

	public:
		virtual bool CheckCollision(const Ray &r) const override;

	};

	class Aabb : public GeometricCollidable
	{
	public:
		Vec3 min = Vec3(0);
		Vec3 max = Vec3(0);
		Vec3 center = Vec3(0);
		Vec3 raduis = Vec3(0);

		void SetFromCenterRaduis(const Vec3 &center, const Vec3 &raduis);
		void SetFromMinMax(const Vec3 &min, const Vec3 &max);

		virtual bool CheckCollision(const Ray &r) const override;

	public:
		Aabb();
		Aabb(const Vec3 &min, const Vec3 &max);
		~Aabb();
	};

	class OBB : public GeometricCollidable
	{

	public:
		Vec3 origin;
		Vec3 extents;
		Basis basis;

	public:
		virtual bool CheckCollision(const Ray &r) const override;

	};




}





