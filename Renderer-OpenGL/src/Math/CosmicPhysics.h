#pragma once
#include "Core.h"
#include "CosmicMath.h"
#include "Utility.h"
#include <list>
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
	   

	
	class PointMass
	{		
	public:
		Vec3 position;
		Vec3 velocity;
		Vec3 acceleration;
		Vec3 force_accum;

		uint32 rid = 0;

		real mass;
		real inverse_mass;
		real damping;

	public:
		void SetMass(real mass);
		void SetDamping(real damp);
		void Integrate(float delta);
	

	};

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

		void ApplyForce(PointMass *ri, real delta) override;
	};

	
	class AttractionForceGenerator : public ForceGenerator
	{
	public:
		Vec3 origin;
		real raduis;
		real strength;
		AttractionForceGenerator() {}
		~AttractionForceGenerator() {}
		AttractionForceGenerator(const Vec3 &origin, real strength, real raduis) : origin(origin), strength(strength), raduis(raduis) {}

		void ApplyForce(PointMass *ri, real delta) override;
	
	};
	
	class PartricelSpringForce : public ForceGenerator
	{
	public:
		PointMass *other;
		//Spring constant
		real k; 
		//Rest length
		real l;	
		//Limit of elasticity
		real limit_of_elas = 0;
		bool apply_to_other;
		void ApplyForce(PointMass *ri, real delta) override;
	};
	
	
	class AnchoredSpringForce : public ForceGenerator
	{
	public:
		Vec3 anchor;
		//Spring constant
		real k; 
		//Rest length
		real l;	
		
		void ApplyForce(PointMass *ri, real delta) override;
	};

	
	class OvercrowdingSpringForceGenerator : public ForceGenerator
	{		
	public:
		std::vector<PointMass*> bodies;
		//Spring constant <--- make this large
		real k; 
		//Rest length <--- make this larger than the collision_lenth else we are extending the spring mathmaticly
		real l;	
		real collision_length;

		void ApplyForce(PointMass *ri, real delta) override;
	};



	//@Note: Closing velo = Seperating velo
	class PointMassContact
	{
	public:
		Vec3 pms_movement[2];
		PointMass *pms[2];
		Vec3 contant_normal;
		real restitution = 0;
		real penetration = 0;
		
		void resolve(float delta)
		{
			resolveVelocity(delta);
			ResolveInterpenetration(delta);
		}
		real CalculateClosingVelocity()
		{
			return Dot(pms[0]->velocity - pms[1]->velocity, contant_normal);
		}

	private:
		void resolveVelocity(float delta)
		{
			real closing_velo = Dot((pms[0]->velocity - pms[1]->velocity), contant_normal); 
			if (closing_velo <= 0)
			{
				real new_closing_velo = -closing_velo * restitution;

				Vec3 accCausedVelocity = pms[0]->acceleration - pms[1]->acceleration;
				real accCausedSepVelocity = Dot(accCausedVelocity, contant_normal) * delta;

				// If we've got a closing velocity due to acceleration build-up,
				// remove it from the new separating velocity
				if (accCausedSepVelocity < 0)
				{
					new_closing_velo += restitution * accCausedSepVelocity;

					// Make sure we haven't removed more than was
					// there to remove.
					if (new_closing_velo < 0) new_closing_velo = 0;
				}

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
			pms_movement[1] = move_per_mass * -pms[1]->inverse_mass;

			pms[0]->position = pms[0]->position + pms_movement[0];
			pms[1]->position = pms[1]->position + pms_movement[1];
		}

	};

	class PointMassContactResolver
	{
	public:
		uint32 iterations;
		//Everything that has pass in here IS colliding in some way
		void Resolve(std::vector<PointMassContact> contacts, float delta)
		{
			if (contacts.size() == 0)
			{
				return;
			}
			// Find the contact with the largest closing velocity;
			for (uint32 p = 0; p < iterations; p++)
			{
				real max = REAL_MAX;
				unsigned maxIndex = (unsigned) contacts.size();
				for (int32 i = 0; i < contacts.size(); i++)
				{
					real sepVel = contacts[i].CalculateClosingVelocity();
					if (sepVel < max &&
						(sepVel < 0 || contacts[i].penetration > 0))
					{
						max = sepVel;
						maxIndex = i;
					}
				}

				// Do we have anything worth resolving?
				if (maxIndex == contacts.size())
				{
					break;
				}

				// Resolve this contact
				contacts[maxIndex].resolve(delta);

				// Update the interpenetrations for all particles
				Vec3 *move = contacts[maxIndex].pms_movement;
				for (int32 i = 0; i < contacts.size(); i++)
				{
					if (contacts[i].pms[0] == contacts[maxIndex].pms[0])
					{
						contacts[i].penetration -= Dot(move[0], contacts[i].contant_normal);
					}
					else if (contacts[i].pms[0] == contacts[maxIndex].pms[1])
					{
						contacts[i].penetration -= Dot(move[1] , contacts[i].contant_normal);
					}
					if (contacts[i].pms[1])
					{
						if (contacts[i].pms[1] == contacts[maxIndex].pms[0])
						{
							contacts[i].penetration += Dot(move[0], contacts[i].contant_normal);
						}
						else if (contacts[i].pms[1] == contacts[maxIndex].pms[1])
						{
							contacts[i].penetration += Dot(move[1], contacts[i].contant_normal);
						}
					}
				}			
			}
		}
	};


	class PointMassCable
	{
	public:
		real max_length;
		real cur_length;
		real restitution;
		PointMass *pms[2];

		PointMassContact CheckContact()
		{
			PointMassContact pmc;
			PointMass m1 = *pms[0];
			PointMass m2 = *pms[1];
			
			real length = Mag(m1.position - m2.position);
			if (length < max_length)
			{
				return {};
			}

			Vec3 normal = Normalize(m2.position - m1.position);

			pmc.pms[0] = pms[0];
			pmc.pms[1] = pms[1];
					
			pmc.penetration = length - max_length;
			pmc.restitution = restitution;
			pmc.contant_normal = normal;

			pmc.restitution = 0;
			return pmc;
		}


	};

	class PointMassRod
	{
	public:
		real max_length;
		real cur_length;
		PointMass *pms[2];

		PointMassContact CheckContact()
		{
			PointMassContact pmc;
			PointMass m1 = *pms[0];
			PointMass m2 = *pms[1];

			real length = Mag(m1.position - m2.position);
			if (requal(length, max_length))
			{
				return {};
			}

			Vec3 normal = Normalize(m2.position - m1.position);

			pmc.pms[0] = pms[0];
			pmc.pms[1] = pms[1];

			if (length > max_length)
			{
				pmc.penetration = length - max_length;
				pmc.contant_normal = normal;
			}
			else
			{
				pmc.penetration = max_length - length;
				pmc.contant_normal = normal * -1;
			}
			pmc.restitution = 0;
			return pmc;
		}
	};


	class MassAggregateEngine 
	{		
		
	private:		
		std::list<PointMass> point_masses;

		std::list<ForceGenerator*> force_generators;

	public:

		uint32 CreatePointMass(const Vec3 &pos, real mass, real damping)
		{
			PointMass pm_test;
			pm_test.damping = damping;
			pm_test.SetMass(mass);
			pm_test.position = pos;			
			pm_test.rid = 1;
			point_masses.push_back(pm_test);
		}
		

		void DeletePointMass(PointMass *pm)
		{
			//point_masses.remove(*pm);
		}

		void DeleteForceGenerator(ForceGenerator *fg)
		{
			//force_generators.remove(fg);
		}

		template<typename T>
		void CreateForceGenerator(const T &fg)
		{
			T * nfg = (T*)malloc(sizeof(T));
			memcpy(nfg, &fg, sizeof(T));
			ForceGenerator* force_gen = (ForceGenerator *)nfg;
			force_generators.push_back(force_gen);
		}


		//template<typename T>
		//T* DoForceThingy()
		//{
		//	OvercrowdingSpringForceGenerator over;
		//	//Push this back.
		//	//The return a pointer.
		//	
		//}

		void Update(float delta)
		{			
			for (PointMass &pm : point_masses)
			{
				if (pm.position.y > 0.25f)
				{
					pm.acceleration.y = -1;
				}
				else
				{
					pm.acceleration = 0;
					pm.velocity = 0;
				}
				pm.Integrate(delta);
			}
		}

		void ResolveCollisions()
		{

		}

		void ApplyForceGenerators(real delta)
		{
			//for (int32 i = 0; i < force_generators.size(); i++)
			//{
			//	for (int32 ii = 0; ii < force_generators[i].registered_point_masses.size(); ii++)
			//	{
			//		int32 index = force_generators[i].registered_point_masses[ii];
			//		PointMass pm = point_masses[index];
			//		force_generators[ii].force_generator->ApplyForce(&pm, delta);
			//	}
			//}
		}
	};

}