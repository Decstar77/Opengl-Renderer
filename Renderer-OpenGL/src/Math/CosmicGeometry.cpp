#include "CosmicGeometry.h"
namespace cm
{
	bool Plane::CheckCollision(const Ray &r) const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool Sphere::CheckCollision(const Ray &r) const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void Aabb::SetFromCenterRaduis(const Vec3 &center, const Vec3 &raduis)
	{
		this->min = center - raduis;
		this->max = center + raduis;
		this->center = center;
		this->raduis = raduis;
	}

	bool Aabb::CheckCollision(const Ray &r) const
	{
		real32 tmin = (min.x - r.origin.x) / r.direction.x;
		real32 tmax = (max.x - r.origin.x) / r.direction.x;

		if (tmin > tmax) { ::std::swap(tmin, tmax); }

		real32 tymin = (min.y - r.origin.y) / r.direction.y;
		real32 tymax = (max.y - r.origin.y) / r.direction.y;

		if (tymin > tymax) { ::std::swap(tymin, tymax); }

		if ((tmin > tymax) || (tymin > tmax)) { return (false); }
			   
		if (tymin > tmin) { tmin = tymin; }

		if (tymax < tmax) { tmax = tymax; }

		real32 tzmin = (min.z - r.origin.z) / r.direction.z;
		real32 tzmax = (max.z - r.origin.z) / r.direction.z;

		if (tzmin > tzmax) { ::std::swap(tzmin, tzmax); }

		if ((tmin > tzmax) || (tzmin > tmax)) { return (false); }

		if (tzmin > tmin) { tmin = tzmin; }

		if (tzmax < tmax) { tmax = tzmax; }

		real32 txmin = (min.x - r.origin.x) / r.direction.x;
		real32 txmax = (max.x - r.origin.x) / r.direction.x;

		if (txmin > txmax) { ::std::swap(txmin, txmax); }
		
		return true;
	}


	bool OOB::CheckCollision(const Ray &r) const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}