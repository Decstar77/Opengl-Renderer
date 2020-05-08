#pragma once
#include "Core.h"
#include "Utility.h"
#include "Math/CosmicMath.h"

namespace cm
{
	class Bone
	{
	public:
		Mat4 current_transform;
		Mat4 inverse_bind_transform;
		Mat4 node_transform_matrix;
		
		// @NOTE: Assume there is only one parent. -1 is an invaild value
		int32 parent_index = -1;

		// @NOTE: Child bones indices. -1 is an invaild value
		String name = "Empty";
		std::vector<int32> child_indices;
	};

	class AnimationFrames
	{
	public:
		int32 bone_index = -1;
		String bone_name;
		std::vector<real32> postime;
		std::vector<Vec3f> poskeys;

		std::vector<real32> rottime;
		std::vector<Quatf> rotkeys;

		std::vector<real32> scltime;
		std::vector<Vec3f> sclkeys;
	};

	enum class AnimationFlags
	{
		Loop = 0,
		Once = 1
	};

	class Animation
	{
	public:
		real32 duration = 0;
		real32 ticks_per_second = 0;
		real32 animation_speed = 1;
		AnimationFlags flags = AnimationFlags::Loop;
		String name;
		std::vector<AnimationFrames> frames;
		std::vector<Bone> *working_bones;
	
	public:
		void Play(real time, std::vector<Bone> *bones);

	private:
		void AnimateBones(const real &animation_time, Bone *bone, const Mat4 &parent_transform);

		int32 GetAnimationChannel(const String &bone_name);

		Vec3f CalculateInterpolatedScaling(const real32 &animation_time, const int32 &keyset_index);
		Vec3f CalculateInterpolatedPosition(const real32 &animation_time, const int32 &keyset_index);
		Quatf CalculateInterpolatedRotation(const real32 &animation_time, const int32 &keyset_index);

		int32 FindPosition(const real32 &animation_time, const int32 &keyset_index);
		int32 FindRotation(const real32 &animation_time, const int32 &keyset_index);
		int32 FindScaling(const real32 &animation_time, const int32 &keyset_index);
	};

	class AnimationController
	{
	public:
		uint32 current_animation_index;
		real32 current_time;
		Mat4 global_inverse_transform;
		
		// @NOTE: We reserve the [0]/0th index to be the root bone. This root bone does not actually exist in the mesh
		//  	: It is simply a place holder to make all other calculations easiers
		std::vector<Bone> bones;	

		// @NOTE: All the animation key frames the corrapsond some of the bones
		std::vector<Animation> animations;

		void Play(const String &name);
		void Play(uint32 animation_index);

		void Update(real32 delta_time);		
	};











}

