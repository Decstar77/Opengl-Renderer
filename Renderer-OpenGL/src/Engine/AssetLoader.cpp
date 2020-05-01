#include "AssetLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/std_image.h"
namespace cm
{
	String ReadFile(const String &file_directory)
	{
		std::ifstream file;
		file.open(file_directory);
		Assert(file.is_open());

		StringStream ss;
		ss << file.rdbuf();
		file.close();

		return ss.str();
	}

	bool LoadTexture(std::vector<uint8> *storage, TextureConfig *config, const String &file_directory)
	{
		int32 width = 0;
		int32 height = 0;
		int32 nrChannels = 0;
		uint8 *data = stbi_load(file_directory.c_str(), &width, &height, &nrChannels, 0);
		
		if (data != nullptr)
		{
			
			config->height = height;
			config->width = width;
			config->data_type = GL_UNSIGNED_BYTE;
			if (nrChannels == 4)
			{
				config->texture_format = GL_RGBA;
				config->pixel_format = GL_RGBA;
			}			
			else if (nrChannels == 3)
			{
				config->texture_format = GL_RGB;
				config->pixel_format = GL_RGB;
			}
			else
			{
				Assert(0); // @REASON: We have no support channel count
			}
			uint32 size_bytes = width * height * nrChannels * sizeof(uint8);
			storage->insert(storage->end(), &data[0], &data[size_bytes / sizeof(uint8)]);

			stbi_image_free(data);
			return true;
		}
		else
		{
			std::cout << "ERROR::STBIMG:: -> Could not load image"  << std::endl;
			return false;
		}		
	}

	void TextureImportMultiThread::DoLoad()
	{
		done.store(false);
		working.store(true);
		LoadTexture(&texture_data, &texture_config, texture_path);
		working.store(false);
		done.store(true);
	}

	void TextureImportMultiThread::Load()
	{
		Thread thread = Thread(&TextureImportMultiThread::DoLoad, this);
		thread.join();
	}

	bool TextureImportMultiThread::Free()
	{
		if (IsLoaded())
		{
			texture_data.clear();
			return true;
		}
		return false;
	}

	bool TextureImportMultiThread::IsLoaded()
	{
		return done.load();
	}

	bool TextureImportMultiThread::IsWorking()
	{
		return working.load();
	}

	bool TextureImportMultiThread::SetTexturePath(const String &path)
	{
		if (!IsWorking())
		{
			this->texture_path = path;
			return true;
		}
		return false;
	}

	cm::TextureConfig *TextureImportMultiThread::GetConfig()
	{
		if (IsLoaded())
		{
			return &texture_config;
		}
		return nullptr;
	}

	std::vector<uint8> * TextureImportMultiThread::GetData()
	{
		if (IsLoaded())
		{
			return &texture_data;
		}
		return nullptr;
	}

	bool TextureImport::Load()
	{
		bool result = true;
		Assert(!multi_thread);
		for (int32 i = 0; i < texture_paths.size(); i++)
		{
			std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();

			int32 width = 0;
			int32 height = 0;
			int32 nrChannels = 0;
			
			const String &path = texture_paths[i];
			stbi_set_flip_vertically_on_load(flip);
			//real32 *data = stbi_loadf(path.c_str(), &width, &height, &nrChannels, 0);
			real32 *data = stbi_loadf(path.c_str(), &width, &height, &nrChannels, 0);

			std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::steady_clock::now();
			std::chrono::microseconds elapsedTime(std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime));
			float time = static_cast<float> (elapsedTime.count());
			LOG(time * 0.001f * 0.001f);
			LOG(width * height * nrChannels * 4);
			if (data)
			{
				TextureConfig texture_config;
				texture_config.height = height;
				texture_config.width = width;

				if (nrChannels == 4)
				{
					texture_config.texture_format = GL_RGBA32F;
					texture_config.pixel_format = GL_RGBA;
				}
				else if (nrChannels == 3)
				{
					texture_config.texture_format = GL_RGB32F;
					texture_config.pixel_format = GL_RGB;
				}
				else
				{
					Assert(0); // @REASON: We have no support channel count
				}

				texture_configs.push_back(texture_config);

				uint32 size = width * height * nrChannels;
				
				texture_data.push_back({});
				texture_data.back().insert(texture_data.back().end(), &data[0], &data[size]);
				
				stbi_image_free(data);		
				data = nullptr;
			}
			else
			{
				result = false;
				LOG("Could not load: " << path);
			}			
		}
		return result;
	}

	bool TextureImport::Free()
	{
		texture_configs.clear();
		texture_data.clear();
		texture_paths.clear();
		return true;
	}

	Mat4 ModelImport::ToMatrix4f(const aiMatrix4x4 *ai_mat)
	{

		uint32 size = sizeof(Mat4);


		Mat4 a;
		a.row0 = Vec4(ai_mat->a1, ai_mat->a2, ai_mat->a3, ai_mat->a4);
		a.row1 = Vec4(ai_mat->b1, ai_mat->b2, ai_mat->b3, ai_mat->b4);
		a.row2 = Vec4(ai_mat->c1, ai_mat->c2, ai_mat->c3, ai_mat->c4);
		a.row3 = Vec4(ai_mat->d1, ai_mat->d2, ai_mat->d3, ai_mat->d4);
		a = Transpose(a);
		return a;
	}


	int32 ModelImport::FindMinVertexWeight(Vertex vertex)
	{
		real32 min_weight = REAL_MAX;
		int32 min_index = -1;

		for (uint32 i = 0; i < MAX_VERTEX_BONE_COUNT; i++)
		{
			real32 current_weight = vertex.bone_weights[i];

			if (current_weight < min_weight)
			{
				min_index = i;
				min_weight = current_weight;
			}
		}

		return min_index;
	}

	void ModelImport::ProcessMeshCombine(const aiScene *scene, aiMesh *mesh, uint32 index)
	{
		// @NOTE: Get the meta data from the current mesh.
		bool positions = mesh->HasPositions();
		bool normals = mesh->HasNormals();
		bool texture_coords = mesh->HasTextureCoords(0);
		bool tanget_bitangets = mesh->HasTangentsAndBitangents();
		bool colours = mesh->HasVertexColors(0);
		bool faces = mesh->HasFaces();
		bool bones = mesh->HasBones();
		
		// @NOTE: This is the offest by which we combine this mesh to all the other meshes in emesh.
		//		: Meaning, indices and vertex bone weights.
		EditableMesh *emesh = &resulting_meshes.at(index);
		AnimationController *ac = &resulting_animation_controllers.at(index);
		uint32 mesh_indices_offset = SafeTruncateUint64(emesh->vertices.size());

		// @NOTE: Construct the vertices with the available data
		for (uint32 i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex = {};

			// Positions
			if (positions)
			{
				vertex.position.x = mesh->mVertices[i].x;
				vertex.position.y = mesh->mVertices[i].y;
				vertex.position.z = mesh->mVertices[i].z;
			}

			// Normals
			if (normals && import_vertex_normals)
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}

			// Texture coordinates			
			if (texture_coords && import_vertex_texture_coords)
			{
				vertex.texture_coord.x = mesh->mTextureCoords[0][i].x;
				vertex.texture_coord.y = mesh->mTextureCoords[0][i].y;
			}

			// Tangent and bitangents
			if (tanget_bitangets && import_vertex_binorms_tangents)
			{
				vertex.tangent.x = mesh->mTangents[i].x;
				vertex.tangent.y = mesh->mTangents[i].y;
				vertex.tangent.z = mesh->mTangents[i].z;

				vertex.bitangent.x = mesh->mBitangents[i].x;
				vertex.bitangent.y = mesh->mBitangents[i].y;
				vertex.bitangent.z = mesh->mBitangents[i].z;
			}

			// Vertex Colours
			if (colours && import_vertex_colours)
			{
				vertex.colour.x = mesh->mColors[0]->r;
				vertex.colour.y = mesh->mColors[0]->g;
				vertex.colour.z = mesh->mColors[0]->b;
			}

			emesh->vertices.push_back(vertex);
		}

		if (bones && import_animations)
		{
			for (uint32 i = 0; i < mesh->mNumBones; i++)
			{
				aiBone *b = mesh->mBones[i];
				String cur_bone_name = b->mName.C_Str();
				int32 bone_index = -1;

				// @NOTE: Find the bone index
				// @SPEED: Again a hash map will solve some of this.
				for (uint32 j = 0; j < ac->bones.size(); j++)
				{
					String bone_name = ac->bones.at(j).name;
					
					if (bone_name == cur_bone_name)
					{
						// @NOTE: Remember that the first entry is the dummy root bone;
						bone_index = j - 1;
						break;
					}
				}
								
				Assert(bone_index != -1); // @REASON: We couldn't find the bone index

				for (uint32 j = 0; j < b->mNumWeights; j++)
				{
					aiVertexWeight vertex_data = b->mWeights[j];

					uint32 vertex_index = vertex_data.mVertexId + mesh_indices_offset;
					real32 vertex_weight = vertex_data.mWeight;
					
					// @TODO: Support more than 4 weights
					Assert(vertex_boneweight_enforcement == false);

					real32 vwc = vertex_weight_cull;				
					// @NOTE: Finds the min vertex weight and store in index						
					//		: If current weight is greater than the lowest weight 
					//		: it is added else ignored
					if (vertex_weight >= vwc)
					{					
						int32 index = FindMinVertexWeight(emesh->vertices.at(vertex_index));
						real32 min_vertex_weight = emesh->vertices.at(vertex_index).bone_weights[index];
						
						if (vertex_weight > min_vertex_weight)
						{
							emesh->vertices.at(vertex_index).bone_index[index] = bone_index;
							emesh->vertices.at(vertex_index).bone_weights[index] = vertex_weight;
#if 0
							if (min_vertex_weight != 0)
							{
								LOG("Replaced weight: " << min_vertex_weight << " With: " << vertex_weight << " Delta " << vertex_weight - min_vertex_weight);
							}
#endif
						}
					}				
				}
			}
		}

		if (faces)
		{			
			for (uint32 i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (uint32 j = 0; j < face.mNumIndices; j++)
				{
					uint32 index = face.mIndices[j] + mesh_indices_offset;
					emesh->indices.push_back(index);
				}
			}
		}

		emesh->has_positions = positions;
		emesh->has_normals = normals;
		emesh->has_texture_coords = texture_coords;
		emesh->has_tanget_bitangets = tanget_bitangets;
		emesh->has_colours = colours;
		emesh->name = mesh->mName.C_Str();
	}

	void ModelImport::ProcessMesh(const aiScene * scene, aiNode *node, uint32 index)
	{
		for (uint32 i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			std::cout << "Mesh name " << mesh->mName.C_Str() << std::endl;
			
			ProcessMeshCombine(scene, mesh, index);
		}

		for (uint32 i = 0; i < node->mNumChildren; i++)
		{
			ProcessMesh(scene, node->mChildren[i], index);
		}
	}



	void ModelImport::StoreAllBones(const aiMesh * mesh, AnimationController *ac)
	{
		for (uint32 i = 0; i < mesh->mNumBones; i++)
		{
			aiBone *b = mesh->mBones[i];
			Bone bone;
			bone.name = b->mName.C_Str();
			bone.inverse_bind_transform = ToMatrix4f(&b->mOffsetMatrix);


			// @NOTE: We need to check if the bone has already been added.
			// @SPEED: Use a hash map probs
			bool add = true;
			for (uint32 j = 0; j < ac->bones.size(); j++)
			{
				if (ac->bones.at(j).name == bone.name)
				{
					add = false;
					break;
				}
			}
			if (add)
			{
				ac->bones.push_back(bone);
			}			
		}
	}

	const aiNode* ModelImport::FindRootNodeOfBones(const aiNode *node, AnimationController *ac)
	{
		// @NOTE: If anything this will give us problems
		String name = node->mName.C_Str();
		for (uint32 i = 1; i < ac->bones.size(); i++)
		{
			if (name == ac->bones.at(i).name)
			{
				return node->mParent;
			}
		}
		for (uint32 i = 0; i < node->mNumChildren; i++)
		{
			const aiNode *child = FindRootNodeOfBones(node->mChildren[i], ac);
			if (child != nullptr)
			{
				return child;
			}				
		}
		return nullptr;
	}

	aiMatrix4x4 ModelImport::CalcRootNodeTransformMatrix(const aiNode *node)
	{
		if (node->mParent == NULL)
		{
			return node->mTransformation;			
		}
		return CalcRootNodeTransformMatrix(node->mParent) * node->mTransformation;
	}

	void ModelImport::StoreNodeTransformMatrices(const aiNode *node, AnimationController *ac)
	{
		String name = node->mName.C_Str();
				
		for (uint32 i = 1; i < ac->bones.size(); i++)
		{
			if (name == ac->bones.at(i).name)
			{
				ac->bones[i].node_transform_matrix = ToMatrix4f(&node->mTransformation);
				break;
			}
		}
		
		for (uint32 i = 0; i < node->mNumChildren; i++)
		{
			StoreNodeTransformMatrices(node->mChildren[i], ac);
		}
	}

	void ModelImport::SortBoneParents(const aiNode *node, uint32 parent_index, AnimationController *ac)
	{
		String name = node->mName.C_Str();
		for (uint32 i = 1; i < ac->bones.size(); i++)
		{
			if (name == ac->bones.at(i).name)
			{
				ac->bones[i].parent_index = parent_index;				
				parent_index = i;				
				break; 
			}
		}

		for (uint32 i = 0; i < node->mNumChildren; i++)
		{
			SortBoneParents(node->mChildren[i], parent_index, ac);
		}
	}

	void ModelImport::SortBoneChildren(AnimationController *ac)
	{
		// @NOTE: This assumes the bone array's parents are correctly set and sorted
		//		: Start at one as we don't care about root
		for (uint32 i = 1; i < ac->bones.size(); i++)
		{
			int32 current_bone_parent = ac->bones.at(i).parent_index;

			// @NOTE: Check that the bone does indeed have a parent
			Assert(current_bone_parent != -1); 

			ac->bones.at(current_bone_parent).child_indices.push_back(i);
		}
	}

	void ModelImport::ProcessAnimationChannels(aiAnimation *anim, Animation *animation)
	{
		for (uint32 i = 0; i < anim->mNumChannels; i++)
		{
			aiNodeAnim *ai = anim->mChannels[i];
			AnimationFrames frame;

			// @NOTE: Add first to avoid coping 
			animation->frames.push_back(frame);

			animation->frames[i].bone_name = ai->mNodeName.C_Str();

			animation->frames[i].poskeys.resize(ai->mNumPositionKeys);
			animation->frames[i].postime.resize(ai->mNumPositionKeys);

			animation->frames[i].rotkeys.resize(ai->mNumRotationKeys);
			animation->frames[i].rottime.resize(ai->mNumRotationKeys);

			animation->frames[i].sclkeys.resize(ai->mNumScalingKeys);
			animation->frames[i].scltime.resize(ai->mNumScalingKeys);

			for (uint32 j = 0; j < ai->mNumPositionKeys; j++)
			{
				Vec3 pos = Vec3(ai->mPositionKeys[j].mValue.x, ai->mPositionKeys[j].mValue.y, ai->mPositionKeys[j].mValue.z);
				real32 time = SafeTruncateDouble(ai->mPositionKeys[j].mTime);
				animation->frames[i].poskeys[j] = pos;
				animation->frames[i].postime[j] = time;
			}

			for (uint32 j = 0; j < ai->mNumRotationKeys; j++)
			{
				Quat rot = Quat(ai->mRotationKeys[j].mValue.x, ai->mRotationKeys[j].mValue.y, ai->mRotationKeys[j].mValue.z, ai->mRotationKeys[j].mValue.w);
				real32 time = SafeTruncateDouble(ai->mRotationKeys[j].mTime);
				animation->frames[i].rotkeys[j] = rot;
				animation->frames[i].rottime[j] = time;
			}

			for (uint32 j = 0; j < ai->mNumScalingKeys; j++)
			{
				Vec3 pos = Vec3(ai->mScalingKeys[j].mValue.x, ai->mScalingKeys[j].mValue.y, ai->mScalingKeys[j].mValue.z);
				real32 time = SafeTruncateDouble(ai->mScalingKeys[j].mTime);
				animation->frames[i].sclkeys[j] = pos;
				animation->frames[i].scltime[j] = time;
			}						
		}
	}

	void ModelImport::ProcessBones(const aiScene *scene, AnimationController *ac)
	{
		// @NOTE: Order is important
		//		: Create a dummy root node. This bone doesn't actually exist in the mesh
		Bone root;
		ac->bones.push_back(root);

		// @NOTE: Store all the bones in the scene
		for (uint32 i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];
			StoreAllBones(mesh, ac);
		}

		// @NOTE: Find the root node of the bones found
		const aiNode *root_node = FindRootNodeOfBones(scene->mRootNode, ac);		

		// @NOTE: Calculate the root node transform of the bones
		aiMatrix4x4 root_node_transform_matrix = CalcRootNodeTransformMatrix(root_node);

		// @NOTE: Store the info
		ac->bones[0].name = root_node->mName.C_Str();
		ac->bones[0].name += "---ROOT_BONE---";
		ac->bones[0].node_transform_matrix = ToMatrix4f(&root_node_transform_matrix);
		ac->bones[0].inverse_bind_transform = Mat4(0);

		// @NOTE: Store all the bone's node's node transformation matrix in the bones
		StoreNodeTransformMatrices(root_node, ac);

		// @NOTE: Create the bone tree, 0 references the root node
		SortBoneParents(root_node, 0, ac);

		// @NOTE: This can only happen if the parents are set. See function for deets
		SortBoneChildren(ac);
	}
	
	void ModelImport::ProcessAnimations(const aiScene *scene, AnimationController *ac)
	{
		// @TODO: Loop throught the all the animations, Processing bones should be independt on this.
		Assert(scene->mNumAnimations <= 1);
		if (!scene->HasAnimations())
		{
			return;
		}
		// @NOTE: Get current animation
		aiAnimation *anim = scene->mAnimations[0];

		// @NOTE: Create new animation
		Animation animation;
		animation.duration = SafeTruncateDouble(anim->mDuration);
		animation.ticks_per_second = SafeTruncateDouble(anim->mTicksPerSecond);
		ProcessAnimationChannels(anim, &animation);
		
		// @NOTE: Get scene transform
		aiMatrix4x4 mat = scene->mRootNode->mTransformation;
		mat.Inverse();

		// @NOTE Update mesh
		ac->global_inverse_transform = ToMatrix4f(&mat);
		ac->animations.push_back(animation);

		// @NOTE: Construct bone tree
		ProcessBones(scene, ac);			   		 
	}

	void ProcessError(const String &err)
	{
		// @TODO: Log file ?
		LOG("ERROR:ASSIMP -> " << err);
	}

	bool ModelImport::Load()
	{		
		bool result = false;
		uint32 import_mesh_count = SafeTruncateUint64(model_paths.size());
		resulting_meshes.resize(import_mesh_count);
		resulting_animation_controllers.resize(import_mesh_count);

		for (uint32 i = 0; i < import_mesh_count; i++)
		{
			Assimp::Importer import;

			uint32 flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;
			flags = import_vertex_binorms_tangents ? flags | aiProcess_CalcTangentSpace : flags;


			const aiScene *scene = import.ReadFile(this->model_paths[i], flags);
			result = scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode;


			Assert(import_mesh_combine == true);
			Assert(multi_thread == false);

			if (result)
			{
				if (import_animations)
				{
					ProcessAnimations(scene, &this->resulting_animation_controllers[i]);
				}
				ProcessMesh(scene, scene->mRootNode, i);
			}
			else
			{
				ProcessError(import.GetErrorString());
			}

			Assert(this->resulting_animation_controllers[i].bones.size() <= 60); // @REASON: We have to many bones for the shader

			import.FreeScene();
		}

		return result;
	}	   

	bool ModelImport::Free()
	{
		resulting_meshes.clear();
		resulting_animation_controllers.clear();
		model_paths.clear();
		return true;
	}

	//bool LoadModel(ModeImport *model_import)
	//{
	//	Assimp::Importer import;
	//			
	//	// @TODO: Process flags
	//	const aiScene *scene = import.ReadFile(model_import->path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);
	//	bool result = scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode;
	//			
	//	// @TODO: We do not support multi meshes
	//	Assert(model_import->import_mesh_combine == true);

	//	// @TODO: I know I know, new..
	//	//model_import->mesh_count = 1;
	//	//model_import->resulting_mesh = new EditableMesh();
	//	//
	//	//model_import->animation_controller_count = 1;
	//	//model_import->animation_controller = new AnimationController();

	//	//delete model_import->animation_controller;
	//	model_import->resulting_meshes.resize(1);
	//	model_import->resulting_animation_controllers.resize(1);

	//	if (result)
	//	{
	//		if (model_import->import_animations)
	//		{
	//			ProcessAnimations(scene, &model_import->resulting_animation_controllers[0]);
	//		}			
	//		ProcessMesh(scene->mRootNode, model_import, scene);
	//	}
	//	else
	//	{
	//		ProcessError(import.GetErrorString());
	//	}

	//	import.FreeScene();
	//	return result;
	//}

	




	}

