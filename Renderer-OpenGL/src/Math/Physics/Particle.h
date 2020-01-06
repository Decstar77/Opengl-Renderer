#pragma once
#include "Core.h"
#include "Math/CosmicMath.h"


namespace cm
{	   
	class Particel
	{
	private:

		vec3 force_accum;		
		real inverse_mass;
		real damping;

	public:
		Particel();
		~Particel();
		
		vec3 position;
		vec3 velocity;
		vec3 acceleration;


		void SetMass(const real &mass);
		void SetDampning(const real &damp);
		
		void AddForce(const vec3 &force);
		void Intergrate(real delta);

		void ClearAccumulator();

		inline real GetMass() const { real a = (inverse_mass == 0) ? FLT_MAX : ((real)1.0) / inverse_mass; return a; }
		inline real GetDampning() const { return damping; }
	};




}