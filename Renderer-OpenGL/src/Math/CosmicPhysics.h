#pragma once
#include "Core.h"
#include "CosmicMath.h"

namespace cm
{	
	//NOTE: Code is meant to be slow but understandable as this is a first attempt
	class Aabb
	{
	public:
		Vec3 min = Vec3(FLT_MIN);
		Vec3 max = Vec3(FLT_MAX);
		Vec3 center = Vec3(0);
		Vec3 raduis = Vec3(0);

		bool CheckCollision(const Ray &r)
		{
			float tmin = (min.x - r.origin.x) / r.direction.x;
			float tmax = (max.x - r.origin.x) / r.direction.x;

			if (tmin > tmax) { ::std::swap(tmin, tmax); }

			float tymin = (min.y - r.origin.y) / r.direction.y;
			float tymax = (max.y - r.origin.y) / r.direction.y;

			if (tymin > tymax) { ::std::swap(tymin, tymax); }

			if ((tmin > tymax) || (tymin > tmax)) { return (false); }

			float a = 2;
			float b = 23;

			if (tymin > tmin) { tmin = tymin; }

			if (tymax < tmax) { tmax = tymax; }

			float tzmin = (min.z - r.origin.z) / r.direction.z;
			float tzmax = (max.z - r.origin.z) / r.direction.z;

			if (tzmin > tzmax) { ::std::swap(tzmin, tzmax); }

			if ((tmin > tzmax) || (tzmin > tmax)) { return (false); }

			if (tzmin > tmin) { tmin = tzmin; }

			if (tzmax < tmax) { tmax = tzmax; }

			float txmin = (min.x - r.origin.x) / r.direction.x;
			float txmax = (max.x - r.origin.x) / r.direction.x;

			if (txmin > txmax) { ::std::swap(txmin, txmax); }


			return (true);
		}
	};

	//NOTE: Code is meant to be slow but understandable as this is a first attempt
	class PointMass
	{
	public:
		Vec3 position;
		Vec3 velocity;
		Vec3 acceleration;
		Vec3 force_accum;

		real mass;
		real inverse_mass;
		real damping;
		

		void SetMass(real mass)
		{
			this->mass = mass;
			if (mass == 0)
			{
				inverse_mass = 0;
			}
			else
			{
				inverse_mass = 1.f / mass;
			}
		}

		void Integrate(float delta)
		{
			if (inverse_mass <= 0.0f) { return; }

			//Assert(delta < 0.0);

			// Update linear position.		
			position += delta * velocity;
			//transform_applied->position = position;	
			
			
			// Work out the acceleration from the force
			Vec3 resulting_acc = acceleration;
			resulting_acc += inverse_mass * force_accum;

			// Update linear velocity from the acceleration.
			velocity += resulting_acc * delta;

			// Impose drag.

			velocity = velocity * pow(damping, delta);

			force_accum = 0;
		}		
	};
	//NOTE: Code is meant to be slow but understandable as this is a first attempt
	class ForceGenerator
	{
	public:
		virtual void ApplyForce(PointMass *ri, real delta) = 0; 
	};

	class DragForceGenerator : public ForceGenerator
	{
	public:
		DragForceGenerator() {}
		DragForceGenerator(real k1, real k2) : k1(k1), k2(k2) {}
		~DragForceGenerator() {}
		real k1, k2;

		void ApplyForce(PointMass *ri, real delta) override
		{			
			Vec3 force = ri->velocity;
			real v = mag(force);
			real coef = k1 * v + k2 * v * v;
			if (!requal(v, 0))
			{
				force = normalize(force) * coef * -1;
				ri->force_accum += force;
			}
			
			// Edge case where by the velocity is 0 
			
		}
	};

	//NOTE: Code is meant to be slow but understandable as this is a first attempt
	class AttractionForceGenerator : public ForceGenerator
	{
	public:
		Vec3 origin;
		real raduis;
		real strength;
		AttractionForceGenerator() {}
		~AttractionForceGenerator() {}
		AttractionForceGenerator(const Vec3 &origin, real strength, real raduis) : origin(origin), strength(strength), raduis(raduis) {}

		void ApplyForce(PointMass *ri, real delta) override
		{
			Vec3 ri_pos = ri->position;
			
			Vec3 dir = origin - ri_pos;
			if (mag(dir) <= raduis)
			{
				dir = normalize(origin - ri_pos);
				Vec3 force = dir * strength;
				ri->force_accum += force;
			}
			
		}
	};
	//NOTE: Code is meant to be slow but understandable as this is a first attempt
	class PartricelSpringForce : public ForceGenerator
	{
	public:
		PointMass *other;
		real k; //Spring constant
		real l;	//Rest length
		real limit_of_elas = 0; //Limit of elasticity
		bool apply_to_other;
		void ApplyForce(PointMass *ri, real delta) override
		{
			Vec3 force = ri->position - other->position;
			real m = mag(force);			
			Vec3 dir = normalize(force);
			real length = abs(m) - l;
			Vec3 applied = -k * (length) * dir;
			ri->force_accum += applied;
			other->force_accum -= applied * (int32)apply_to_other;
		}
	};
	
	//NOTE: Code is meant to be slow but understandable as this is a first attempt	
	class AnchoredSpringForce : public ForceGenerator
	{
	public:
		Vec3 anchor;
		real k; //Spring constant
		real l;	//Rest length
		
		void ApplyForce(PointMass *ri, real delta) override
		{
			Vec3 force = ri->position - anchor;
			real m = mag(force);
			Vec3 dir = normalize(force);
			Vec3 applied = -k * (abs(m) - l) * dir;
			ri->force_accum += applied;
		}
	};

	//NOTE: Code is meant to be slow but understandable as this is a first attempt
	class OvercrowdingSpringForceGenerator : public ForceGenerator
	{		
	public:
		DynaArray<PointMass*> bodies;
		real k; //Spring constant <--- make this large
		real l;	//Rest length <--- make this larger than the collision_lenth else we are extending the spring mathmaticly
		real collision_length;

		void ApplyForce(PointMass *ri, real delta) override
		{		
			for (uint32 i = 0; i < bodies.size(); i++)
			{
				if (ri == bodies[i])
				{
					continue;
				}
				Vec3 force = ri->position - bodies[i]->position;
				real length = mag(force);
				if (length < collision_length)
				{
					Vec3 dir = normalize(force);
					Vec3 applied = -k * (abs(length) - l) * dir;
					ri->force_accum += applied;
				}
			}
		}
	};

	class PointMassContact
	{
	public:
		PointMass *pms[2];
		Vec3 pms_movement[2];
		Vec3 contant_normal;
		real restitution;
		real penetration;
		void resolve(float delta)
		{
			resolveVelocity(delta);
		}
	private:
		void resolveVelocity(float delta)
		{
			real closing_velo = dot((pms[0]->velocity - pms[1]->velocity), contant_normal); 
			if (closing_velo <= 0)
			{
				real new_closing_velo = -closing_velo * restitution;
				real delta_velo = new_closing_velo - closing_velo;

				real total_inverse_mass = pms[0]->inverse_mass + pms[1]->inverse_mass;

				if (total_inverse_mass <= 0)
				{
					return;
				}

				real impluse = delta_velo / total_inverse_mass; //@NOTE: p = v*m 

				Vec3 impluse_per_invmass = contant_normal * impluse;

				pms[0]->velocity += impluse_per_invmass * pms[0]->inverse_mass;
				pms[1]->velocity -= impluse_per_invmass * pms[1]->inverse_mass;
			}
		}
		void ResolveInterpenetration(float delta)
		{
			real total_inverse_mass = pms[0]->inverse_mass + pms[1]->inverse_mass;
			if (penetration <= 0 || total_inverse_mass <= 0)
			{
				return;
			}
			//This is code is a sneaky and clever work around to get inverse mass to work
			//Real forumla is pa = mb * d / (ma + mb)
			//But because we use inverse mass we have total_inverse_mass = (ma + mb) / ma * mb
			Vec3 move_per_mass = contant_normal * (penetration / total_inverse_mass);

			pms_movement[0] = move_per_mass * pms[0]->inverse_mass;
			pms_movement[1] = move_per_mass * pms[1]->inverse_mass;

			pms[0]->position = pms[0]->position + pms_movement[0];
			pms[1]->position = pms[1]->position + pms_movement[1];
		}

	};

	//NOTE: Code is meant to be slow but understandable as this is a first attempt
	class ForceRegistery
	{
	private:
		struct ForceRegistration
		{
			ForceGenerator *fg;
			PointMass *ri;
		};
		DynaArray<ForceRegistration> res;
	public:

		void RegisterForce(ForceGenerator *fg, PointMass *ri)
		{
			ForceRegistration r;
			r.fg = fg;
			r.ri = ri;
			res.push_back(r);
		}

		void UpdateForces(real delta)
		{
			for (int32 i = 0; i < res.size(); i++)
			{
				res[i].fg->ApplyForce(res[i].ri, delta);
			}
		}

	};

}