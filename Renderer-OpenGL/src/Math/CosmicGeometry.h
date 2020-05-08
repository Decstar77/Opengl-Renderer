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
		Vec3f origin;
		Vec3f direction;

	public:
		Vec3f Travel(real32 dist) const;

	public:
		Ray();
		Ray(const Vec3f &origin, const Vec3f &direction);
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
		Vec3f origin;
		Vec3f normal;

	public:
		virtual GeometricColliderType GetColliderType() const override;
		virtual bool CheckCollision(const Ray &r) const override;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const override;

		virtual void Update(const Transform &t) override;
		virtual void Update(const Transform *t) override;



	public:
		Plane();
		Plane(const Vec3f &origin, const Vec3f &normal);
		~Plane();
	};

	class Aabb : public GeometricCollider
	{
	public:
		Vec3f min = Vec3f(0);
		Vec3f max = Vec3f(0);
		Vec3f center = Vec3f(0);
		Vec3f radius = Vec3f(0);

	public:
		virtual GeometricColliderType GetColliderType() const override;

		void SetFromCenterRaduis(const Vec3f &center, const Vec3f &raduis);
		void SetFromMinMax(const Vec3f &min, const Vec3f &max);

		virtual bool CheckCollision(const Ray &r) const override;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const override;

		virtual void Update(const Transform &t) override;
		virtual void Update(const Transform *t) override;

	public:
		Aabb();
		Aabb(const Vec3f &min, const Vec3f &max);
		~Aabb();
	};

	class OBB : public GeometricCollider
	{
	public:
		Basis basis;
		Vec3f origin = Vec3f(0);

	public:
		
		Vec3f GetExtents() const;
		void SetExtents(const Vec3f &extents);
		
		virtual GeometricColliderType GetColliderType() const override;

		virtual bool CheckCollision(const Ray &r) const override;
		virtual bool CheckCollision(const Ray &r, CollisionInfo *collision_info) const override;

		virtual void Update(const Transform &t) override;
		virtual void Update(const Transform *t) override;
				
	public:
		OBB();
		OBB(const Vec3f &origin, const Vec3f &extents, const Basis &basis);
		~OBB();


	private:
		Vec3f extents = Vec3f(0);
		Vec3f scale_extent_offset = Vec3f(0);
		


	};




}





