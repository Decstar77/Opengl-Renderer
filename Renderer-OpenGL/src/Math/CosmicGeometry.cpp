#include "CosmicGeometry.h"
namespace cm
{


	Ray::Ray()
	{

	}

	Ray::Ray(const Vec3 &origin, const Vec3 &direction)
	{
		this->origin = origin;
		this->direction = Normalize(direction);
	}

	Ray::~Ray()
	{

	}
	
	Vec3 Ray::Travel(real32 dist)
	{
		return origin + dist * direction;
	}

	Plane::Plane()
	{

	}

	Plane::Plane(const Vec3 &origin, const Vec3 &normal)
	{
		this->origin = origin;
		this->normal = Normalize(normal);
	}

	Plane::~Plane()
	{

	}

	bool Plane::CheckCollision(const Ray &r) const
	{
		// @NOTE: Assumes that everything is normalized
		real32 demon = Dot(r.direction, normal);
		if (Abs(demon) >= 0.001)
		{
			Vec3 to_origin = origin - r.origin;
			real32 t = Dot(to_origin, normal) / demon;
			return (t >= 0);
		}
		return false;
	}
	   
	real32 Plane::CheckDistance(Ray &r) const
	{
		real32 demon = Dot(r.direction, normal);
		if (Abs(demon) >= 0.001)
		{
			Vec3 to_origin = origin - r.origin;
			real32 t = Dot(to_origin, normal) / demon;
			return (t);
		}
		return -1;
	}

	bool Sphere::CheckCollision(const Ray &r) const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}
	
	Aabb::Aabb()
	{

	}

	Aabb::Aabb(const Vec3 &min, const Vec3 &max)
	{
		SetFromMinMax(min, max);
	}

	Aabb::~Aabb()
	{

	}

	void Aabb::SetFromCenterRaduis(const Vec3 &center, const Vec3 &raduis)
	{
		this->min = center - raduis;
		this->max = center + raduis;
		this->center = center;
		this->raduis = raduis;
	}

	void Aabb::SetFromMinMax(const Vec3 &min, const Vec3 &max)
	{
		this->min = min;
		this->max = max;
		this->center = (min + max) / 2.0;
		this->raduis = Abs(max - this->center);
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


	bool OBB::CheckCollision(const Ray &r) const
	{
		real32 tmin = -REAL_MAX;
		real32 tmax	= REAL_MAX;

		for (int32 i = 0; i < 3; i++)
		{
			real32 demon = Dot(r.direction, basis.mat.data[i]);
			if (Abs(demon) < 0.001)
			{
				// @NOTE: Ray parallel to a plane
				Vec3 to_origin = origin - r.origin;
				real32 r = Dot(basis.mat.data[i], to_origin);
				if (-r - extents.arr[i] > 0 || -r + extents.arr[i] > 0)
				{
					return false;
				}
			}

			Vec3 to_origin = origin - r.origin;
			real32 q = Dot(basis.mat.data[i], to_origin);
			real32 s = Dot(basis.mat.data[i], r.direction);


			real32 t0 = (q + extents.arr[i]) / s;
			real32 t1 = (q - extents.arr[i]) / s;

			if (t0 > t1) { ::std::swap(t0, t1); }
			if (t0 > tmin) { tmin = t0; }
			if (t1 < tmax) { tmax = t1; }

			if (tmin > tmax) { return false; }
			if (tmax < 0) { return false; }
		}


		// @TODO: t is the intersection point from the ray. o + t * d;
		real32 t = 0;
		// @NOTE: At this point they definitely intersect
		if (tmin > 0)
		{
			t = tmin;
		}
		else
		{
			t = tmax;
		}
		
		return true;
	}


	//void BoundingVolume::UpdateAABB(const mat4 &transform, const vec3 &pos)
	//{
	//	aabb box;
	//	for (int i = 0; i < 3; i++)
	//	{
	//		box.center.arr[i] = pos.arr[i];
	//		box.raduis.arr[i] = 0;
	//		for (int j = 0; j < 3; j++)
	//		{
	//			box.center.arr[i] += transform.data[j].arr[i] * original_aabb.center.arr[j];
	//			box.raduis.arr[i] += abs(transform.data[j].arr[i]) * original_aabb.raduis.arr[j];
	//		}
	//	}

	//	box.min = box.center - box.raduis;
	//	box.max = box.center + box.raduis;
	//	current_aabb = box;
	//}

	//Transform::Transform() : position(0), scale(1)
	//{

	//}


	//Transform::~Transform()
	//{

	//}

	//cm::mat4 Transform::GetTrasformMatrix() const
	//{
	//	mat4 model_transform(1);
	//	//Apparently scale, rotate, translate. Here is rotate, translate, scale
	//	model_transform = translate(model_transform, position);
	//	model_transform = quat_to_mat4(conjugate(quaterion)) * model_transform;
	//	model_transform = scale_cardinal(model_transform, scale);
	//	return model_transform;
	//}

	//void Transform::Translate(vec3 vec, Space relative)
	//{
	//	vec3 dir = vec3(0);
	//	if (relative == Space::Object)
	//	{
	//		vec = rotate((quaterion), vec);
	//		position += vec;
	//	}
	//	else if (relative == Space::World)
	//	{
	//		position += vec;
	//	}

	//}

	//void Transform::Rotate(float d_angle, vec3 axis)
	//{
	//	float sh = sin(deg_to_rad(d_angle / 2));
	//	float ch = cos(deg_to_rad(d_angle / 2));

	//	quat r(axis.x * sh,
	//		axis.y * sh,
	//		axis.z * sh,
	//		ch);

	//	quaterion = (r * quaterion);
	//}

	//void Transform::Rotate(quat r)
	//{
	//	quaterion = (r * quaterion);
	//}

	//void Transform::Rotate(vec3 d_angles, Space relative)
	//{
	//	Assert(0); //Not complete
	//}

	//vec3 Transform::GetEulerRotation() const
	//{
	//	vec3 euler_angle(0);
	//	float sp = -2.f * (quaterion.y *quaterion.z - quaterion.w * quaterion.x);
	//	if (fabs(sp) > 0.999f)
	//	{
	//		euler_angle.y = 1.570796f * sp;

	//		euler_angle.x = atan2(-quaterion.x * quaterion.z + quaterion.w * quaterion.y, 0.5f - quaterion.y * quaterion.y - quaterion.z *quaterion.z);
	//		euler_angle.z = 0;
	//	}
	//	else
	//	{
	//		euler_angle.y = asin(sp);
	//		euler_angle.x = atan2(-quaterion.x * quaterion.z + quaterion.w * quaterion.y, 0.5f - quaterion.x * quaterion.x - quaterion.y *quaterion.y);
	//		euler_angle.z = atan2(-quaterion.x * quaterion.y + quaterion.w * quaterion.z, 0.5f - quaterion.x * quaterion.x - quaterion.z *quaterion.z);
	//	}
	//	return vec3(rad_to_deg(euler_angle.x), rad_to_deg(euler_angle.y), rad_to_deg(euler_angle.z));
	//}

	//void Transform::SetRotation(const vec3 &new_rotation)
	//{
	//	quaterion = euler_to_quat(new_rotation);
	//}

	//void Transform::SetRotation(const quat &q)
	//{
	//	quaterion = q;
	//}

	//cm::basis Transform::GetBasisVectors() const
	//{
	//	basis basis_vector;
	//	basis_vector.forward = normalize(rotate(quaterion, vec3(0, 0, 1)));
	//	basis_vector.upward = normalize(rotate(quaterion, vec3(0, 1, 0)));
	//	basis_vector.right = normalize(rotate(quaterion, vec3(1, 0, 0)));
	//	return basis_vector;
	//}






}