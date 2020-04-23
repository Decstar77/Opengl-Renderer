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
		virtual bool CheckCollision(const Ray &r) const override;

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
		void SetFromMinMax();

		virtual bool CheckCollision(const Ray &r) const override;

	};

	class OOB : public GeometricCollidable
	{

	public:
		virtual bool CheckCollision(const Ray &r) const override;

	};




}





