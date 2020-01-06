#include "Particle.h"




namespace cm
{	
	Particel::Particel() : inverse_mass(1), position(0), velocity(0), acceleration(0), force_accum(0)
	{

	}

	Particel::~Particel()
	{

	}

	void Particel::SetMass(const real &mass)
	{
		Assert(mass != 0);
		inverse_mass = ((real)1.0) / mass;
	}

	void Particel::SetDampning(const real &damp)
	{
		damping = clamp(damp, 0, 1);
	}

	void Particel::AddForce(const vec3 &force)
	{
		force_accum += force;
	}

	void Particel::ClearAccumulator()
	{

	}



	void Particel::Intergrate(real delta)
	{		
		if (inverse_mass <= 0.0f) { return; }

		Assert(delta > 0.0);

		// Update linear position.
		position += delta * velocity;

		// Work out the acceleration from the force
		vec3 resulting_acc = acceleration;
		resulting_acc += inverse_mass * force_accum;

		// Update linear velocity from the acceleration.
		velocity += resulting_acc * delta;

		// Impose drag.
		
		velocity = velocity * pow(damping, delta);

		// Clear the forces.
		ClearAccumulator();
	}

}