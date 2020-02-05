#include "CosmicPhysics.h"

namespace cm
{
	void PointMass::SetMass(real mass)
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

	void PointMass::SetDamping(real damp)
	{
		damping = Clamp(damp, 0, 1);
	}

	void PointMass::Integrate(float delta)
	{
		if (inverse_mass <= 0.0f) { return; }

		//Assert(delta < 0.0);

		// Update linear position. 
		// We omit the acceration term due to it have such a small impact of the result
		// However this is based off the equation of motion below. Equatiom 3.
		// Dx = vi * t + 0.5 * a * t^2.
		position += delta * velocity;
		

		// Work out the acceleration from the force. F = ma;
		Vec3 resulting_acc = acceleration;
		resulting_acc += inverse_mass * force_accum;

		// Update linear velocity from the acceleration.		
		// This is based off the equation of motion below. Equatiom 1.
		// vf = vi + a * t
		velocity += resulting_acc * delta;

		// Impose drag.

		velocity = velocity * pow(damping, delta);

		force_accum = 0;
	}

	void DragForceGenerator::ApplyForce(PointMass *ri, real delta)
	{
		Vec3 force = ri->velocity;
		real v = Mag(force);
		real coef = k1 * v + k2 * v * v;
		// Edge case where by the velocity is 0 
		if (!requal(v, 0))
		{
			force = Normalize(force) * coef * -1;
			ri->force_accum += force;
		}
	}

	void AttractionForceGenerator::ApplyForce(PointMass *ri, real delta)
	{
		Vec3 ri_pos = ri->position;

		Vec3 dir = origin - ri_pos;
		if (Mag(dir) <= raduis)
		{
			dir = Normalize(origin - ri_pos);
			Vec3 force = dir * strength;
			ri->force_accum += force;
		}

	}

	void PartricelSpringForce::ApplyForce(PointMass *ri, real delta)
	{
		Vec3 force = ri->position - other->position;
		real m = Mag(force);
		Vec3 dir = Normalize(force);
		real length = abs(m) - l;
		Vec3 applied = -k * (length)* dir;
		ri->force_accum += applied;
		other->force_accum -= applied * (float)apply_to_other;
	}

	void AnchoredSpringForce::ApplyForce(PointMass *ri, real delta)
	{
		Vec3 force = ri->position - anchor;
		real m = Mag(force);
		Vec3 dir = Normalize(force);
		Vec3 applied = -k * (abs(m) - l) * dir;
		ri->force_accum += applied;
	}

	void OvercrowdingSpringForceGenerator::ApplyForce(PointMass *ri, real delta)
	{
		for (uint32 i = 0; i < bodies.size(); i++)
		{
			if (ri == bodies[i])
			{
				continue;
			}
			Vec3 force = ri->position - bodies[i]->position;
			real length = Mag(force);
			if (length < collision_length)
			{
				Vec3 dir = Normalize(force);
				Vec3 applied = -k * (abs(length) - l) * dir;
				ri->force_accum += applied;
			}
		}
	}

}

