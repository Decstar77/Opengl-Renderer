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

	struct CollisionInfo
	{
		bool hit = false;
		real32 dist = -1;
	};

	class Ray
	{
	public:
		Vec3 origin;
		Vec3 direction;

	public:
		Vec3 Travel(real32 dist) const;

	public:
		Ray();
		Ray(const Vec3 &origin, const Vec3 &direction);
		~Ray();
	};	   	  	
	
	class GeometricCollidable
	{
	public:
		virtual bool CheckCollision(const Ray &r) const = 0;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const = 0;
			
	};

	class Plane : public GeometricCollidable
	{
	public:
		Vec3 origin;
		Vec3 normal;
			   
	public:
		virtual bool CheckCollision(const Ray &r) const override;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const override;

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

	public:
		void SetFromCenterRaduis(const Vec3 &center, const Vec3 &raduis);
		void SetFromMinMax(const Vec3 &min, const Vec3 &max);

		virtual bool CheckCollision(const Ray &r) const override;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const override;

	public:
		Aabb();
		Aabb(const Vec3 &min, const Vec3 &max);
		~Aabb();
	};

	class OBB : public GeometricCollidable
	{
	public:
		Vec3 origin = Vec3(0);
		Vec3 extents = Vec3(0);
		Basis basis;

	public:
		virtual bool CheckCollision(const Ray &r) const override;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const override;

	public:
		OBB();
		OBB(const Vec3 &origin, const Vec3 &extents, const Basis &basis);
		~OBB();

	};




}





