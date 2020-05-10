#pragma once
#include "CosmicMath.h"
namespace cm
{
	//===============================================================//
	/*

	*/
	//===============================================================//

	//************************************
	// Basis
	//************************************

	struct Basis
	{
		union
		{
			Mat3f mat;
			struct
			{
				Vec3f right;
				Vec3f upward;
				Vec3f forward;
			};
		};

		Basis()
		{
			mat = Mat3f(1);
		}

		Basis(const Mat3f &basis)
		{
			this->mat = basis;
		}

		Basis(const Vec3f &right, const Vec3f &upward, const Vec3f &forward)
		{
			this->right = right;
			this->upward = upward;
			this->forward = forward;
		}
	};

	struct CollisionInfo
	{
		bool hit = false;
		real32 dist = -1;
	};

	class Transform
	{
	public:

		Vec3f position = Vec3f(0, 0, 0);
		Vec3f scale = Vec3f(1, 1, 1);
		Quatf rotation = Quatf(0, 0, 0, 1);
		Basis basis;

	public:

		Mat4f CalcTransformMatrix() const
		{
			return CalculateTransformMatrix(position, scale, rotation);
		}
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





