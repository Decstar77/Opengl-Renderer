#include "Animation.h"



namespace cm
{	   	 
	void AnimationController::Play(const std::string &name)
	{
		for (uint32 i = 0; i < animations.size(); i++)
		{


		}
	}

	void AnimationController::Play(uint32 animation_index)
	{
		Assert(animation_index < animations.size());
		Assert(animation_index >= 0);

		Animation * anim = &animations[animation_index];
		anim->Play(current_time, &bones);

		for (int32 i = 0; i < bones.size(); i++)
		{
			//bones[i].current_transform = bones[i].current_transform * global_inverse_transform;
		}

	}

	void Animation::Play(real time, DynaArray<Bone> *bones)
	{
		// @TODO: Time calcs
		real32 time_in_ticks = time * ticks_per_second;
		real32 animation_time = fmod(time_in_ticks, duration);

		working_bones = bones;
		AnimateBones(animation_time, &bones->at(0), Mat4(1));
	}

	void Animation::AnimateBones(const real &animation_time, Bone *bone, const Mat4 &parent_transform)
	{
		std::string bone_name = bone->name;
		Mat4 node_transform = bone->node_transform_matrix;

		// @NOTE: Returns -1 if nothing is found
		int32 keyset_index = GetAnimationChannel(bone_name);

		if (keyset_index != -1)
		{
			Assert(keyset_index < frames.size());
			Assert(keyset_index >= 0);

			Vec3 pos = CalculateInterpolatedPosition(animation_time, keyset_index);
			Quat q = CalculateInterpolatedRotation(animation_time, keyset_index);
			Vec3 scl = CalculateInterpolatedScaling(animation_time, keyset_index);

			Mat4 scaleM = ScaleCardinal(Mat4(1), scl);
			Mat4 rotM = QuatToMat4(q);
			Mat4 tranM = Translate(Mat4(1), pos);

			node_transform = scaleM * rotM * tranM;
		}

		Mat4 global_transform = node_transform * parent_transform;

		bone->current_transform = bone->inverse_bind_transform * global_transform;

		//std::cout << bone_name << std::endl;
		//std::cout << "parent_transform: ";
		//Print(parent_transform);
		//std::cout << "node_transform: ";
		//Print(node_transform);
		//std::cout << "Global_transform: ";
		//Print(global_transform);
		//std::cout << "bind_transform: ";
		//Print(bone->inverse_bind_transform);
		//std::cout << "final_transform: ";
		//Print(bone->current_transform);
		//std::cout << std::endl;


		for (uint32 i = 0; i < bone->child_indices.size(); i++)
		{
			uint32 next_bone_index = bone->child_indices.at(i);
			Bone *next_bone = &working_bones->at(next_bone_index);
			AnimateBones(animation_time, next_bone, global_transform);
		}
	}

	int32 Animation::GetAnimationChannel(const std::string &bone_name)
	{
		for (uint32 i = 0; i < frames.size(); i++)
		{
			if (frames[i].name == bone_name)
				return i;
		}
		return -1;
	}


	cm::Vec3 Animation::CalculateInterpolatedPosition(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];

		// TODO: Make sure the keyset has more than one key

		int32 position_index = FindPosition(animation_time, keyset_index);
		int32 next_position_index = position_index + 1;

		real32 delta_time = keyset.postime[next_position_index] - keyset.postime[position_index];

		real32 factor = (animation_time - keyset.postime[position_index]) / delta_time;

		if (factor < 0 || factor >= 1)
		{
			//LOG("Warning clamping factor, factor = " << factor);
		}
		factor = Clamp(factor, 0.f, 1.f);


		Vec3 start = keyset.poskeys[position_index];
		Vec3 end = keyset.poskeys[next_position_index];

		Vec3 result = Lerp(start, end, factor);

		return result;
	}

	cm::Quat Animation::CalculateInterpolatedRotation(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];

		// TODO: Make sure the keyset has more than one key

		int32 rotation_index = FindRotation(animation_time, keyset_index);
		int32 next_rotation_index = rotation_index + 1;

		real32 delta_time = keyset.rottime[next_rotation_index] - keyset.rottime[rotation_index];

		real32 factor = (animation_time - keyset.rottime[rotation_index]) / delta_time;

		if (factor < 0 || factor >= 1)
		{
			//LOG("Warning clamping factor, factor = " << factor);
		}
		factor = Clamp(factor, 0.f, 1.f);

		Quat start = keyset.rotkeys[rotation_index];
		Quat end = keyset.rotkeys[next_rotation_index];

		Quat result = Slerp(start, end, factor);

		return result;
	}


	cm::Vec3 Animation::CalculateInterpolatedScaling(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];

		// TODO: Make sure the keyset has more than one key

		int32 scaling_index = FindScaling(animation_time, keyset_index);
		int32 next_scaling_index = scaling_index + 1;

		real32 delta_time = keyset.scltime[next_scaling_index] - keyset.scltime[scaling_index];

		real32 factor = (animation_time - keyset.scltime[scaling_index]) / delta_time;

		if (factor < 0 || factor >= 1)
		{
			//LOG("Warning clamping factor, factor = " << factor);
		}
		factor = Clamp(factor, 0.f, 1.f);


		Vec3 start = keyset.sclkeys[scaling_index];
		Vec3 end = keyset.sclkeys[next_scaling_index];

		Vec3 result = Lerp(start, end, factor);
		return result;
	}

	int32 Animation::FindPosition(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];

		for (uint32 i = 0; i < keyset.postime.size(); i++)
		{
			if (animation_time < keyset.postime[i + 1])
			{
				return i;
			}
		}

		Assert(0);
		return -1;
	}

	int32 Animation::FindRotation(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];

		for (uint32 i = 0; i < keyset.rottime.size(); i++)
		{
			if (animation_time < keyset.rottime[i + 1])
			{
				return i;
			}
		}

		Assert(0);
		return -1;
	}

	int32 Animation::FindScaling(const real32 &animation_time, const int32 &keyset_index)
	{
		const AnimationFrames &keyset = frames[keyset_index];

		for (uint32 i = 0; i < keyset.scltime.size(); i++)
		{
			if (animation_time < keyset.scltime[i + 1])
			{
				return i;
			}
		}

		Assert(0);
		return -1;
	}





}
