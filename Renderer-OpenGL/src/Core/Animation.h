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
		std::string name = "Empty";
		DynaArray<int32> child_indices;
	};

	class AnimationFrames
	{
	public:
		int32 bone_index = -1;
		std::string bone_name;
		DynaArray<real32> postime;
		DynaArray<Vec3> poskeys;

		DynaArray<real32> rottime;
		DynaArray<Quat> rotkeys;

		DynaArray<real32> scltime;
		DynaArray<Vec3> sclkeys;
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
		std::string name;
		DynaArray<AnimationFrames> frames;
		DynaArray<Bone> *working_bones;
	
	public:
		void Play(real time, DynaArray<Bone> *bones);
		void AnimateBones(const real &animation_time, Bone *bone, const Mat4 &parent_transform);

		int32 GetAnimationChannel(const std::string &bone_name);

		Vec3 CalculateInterpolatedScaling(const real32 &animation_time, const int32 &keyset_index);
		Vec3 CalculateInterpolatedPosition(const real32 &animation_time, const int32 &keyset_index);
		Quat CalculateInterpolatedRotation(const real32 &animation_time, const int32 &keyset_index);

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
		DynaArray<Bone> bones;	

		// @NOTE: All the animation key frames the corrapsond some of the bones
		DynaArray<Animation> animations;

		void Play(const std::string &name);
		void Play(uint32 animation_index);

		void Update(real32 delta_time);		
	};











}

