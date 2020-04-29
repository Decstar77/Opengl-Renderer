#pragma once
#include "Core.h"
#include "CosmicMath.h"
namespace cm
{
	//===============================================================//
	/*

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
	

	enum class GeometricColliderType
	{
		plane = 0,
		axis_aligned_bounding_box,
		object_bounding_box 
	};

	class GeometricCollider
	{
	public:
		virtual GeometricColliderType GetColliderType() const = 0;
		virtual bool CheckCollision(const Ray &r) const = 0;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const = 0;
		virtual void Update(const Transform &t) { Assert(0); }
		virtual void Update(const Transform *t) { Assert(0); }
	};

	class Plane : public GeometricCollider
	{
	public:
		Vec3 origin;
		Vec3 normal;

	public:
		virtual GeometricColliderType GetColliderType() const override;
		virtual bool CheckCollision(const Ray &r) const override;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const override;

		virtual void Update(const Transform &t) override;
		virtual void Update(const Transform *t) override;



	public:
		Plane();
		Plane(const Vec3 &origin, const Vec3 &normal);
		~Plane();
	};

	class Aabb : public GeometricCollider
	{
	public:
		Vec3 min = Vec3(0);
		Vec3 max = Vec3(0);
		Vec3 center = Vec3(0);
		Vec3 radius = Vec3(0);

	public:
		virtual GeometricColliderType GetColliderType() const override;

		void SetFromCenterRaduis(const Vec3 &center, const Vec3 &raduis);
		void SetFromMinMax(const Vec3 &min, const Vec3 &max);

		virtual bool CheckCollision(const Ray &r) const override;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const override;

		virtual void Update(const Transform &t) override;
		virtual void Update(const Transform *t) override;

	public:
		Aabb();
		Aabb(const Vec3 &min, const Vec3 &max);
		~Aabb();
	};

	class OBB : public GeometricCollider
	{
	public:
		Vec3 origin = Vec3(0);
		Vec3 extents = Vec3(0);
		Basis basis;

	public:
		virtual GeometricColliderType GetColliderType() const override;

		virtual bool CheckCollision(const Ray &r) const override;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const override;

		virtual void Update(const Transform &t) override;
		virtual void Update(const Transform *t) override;

	public:
		OBB();
		OBB(const Vec3 &origin, const Vec3 &extents, const Basis &basis);
		~OBB();

	};




}





