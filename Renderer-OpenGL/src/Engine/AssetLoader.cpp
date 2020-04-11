#include "AssetLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/std_image.h"
namespace cm
{
	std::string ReadFile(const std::string &file_directory)
	{
		std::ifstream file;
		file.open(file_directory);
		Assert(file.is_open());

		std::stringstream ss;
		ss << file.rdbuf();
		file.close();

		return ss.str();
	}

	bool LoadTexture(DynaArray<uint8> *storage, TextureConfig *config, const std::string &file_directory)
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
			storage->CopyFromPtr(data, size_bytes);
			stbi_image_free(data);
			return true;
		}
		else
		{
			std::cout << "ERROR::STBIMG:: -> Could not load image"  << std::endl;
			return false;
		}		
	}

	Mat4 ToMatrix4f(const aiMatrix4x4 *ai_mat)
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

	void processMesh(aiMesh *mesh, EditableMesh *edit_mesh, const aiScene *scene)
	{
		// Walk through each of the mesh's vertices

		bool positions = mesh->HasPositions();
		bool normals = mesh->HasNormals();
		bool texture_coords = mesh->HasTextureCoords(0);
		bool tanget_bitangets = mesh->HasTangentsAndBitangents();
		bool colours = mesh->HasVertexColors(0);
		bool faces = mesh->HasFaces();
		bool bones = mesh->HasBones();

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
			if (normals)
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}

			// Texture coordinates			
			if (texture_coords)
			{
				vertex.texture_coord.x = mesh->mTextureCoords[0][i].x;
				vertex.texture_coord.y = mesh->mTextureCoords[0][i].y;
			}

			// Tangent and bitangents
			if (tanget_bitangets)
			{
				vertex.tanget.x = mesh->mTangents[i].x;
				vertex.tanget.y = mesh->mTangents[i].y;
				vertex.tanget.z = mesh->mTangents[i].z;

				vertex.bitanget.x = mesh->mBitangents[i].x;
				vertex.bitanget.y = mesh->mBitangents[i].y;
				vertex.bitanget.z = mesh->mBitangents[i].z;
			}

			// Vertex Colours
			if (colours)
			{
				Assert(0); // @TODO: Check before use
				vertex.colour.x = mesh->mColors[0]->r;
				vertex.colour.y = mesh->mColors[0]->g;
				vertex.colour.z = mesh->mColors[0]->b;
			}

			edit_mesh->vertices.push_back(vertex);
		}

		if (bones)
		{
			for (int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone *b = mesh->mBones[i];
				for (int j = 0; j < b->mNumWeights; j++)
				{
					aiVertexWeight vertex_data = b->mWeights[j];
					uint32 vertex_index = vertex_data.mVertexId;
					float vertex_weight = vertex_data.mWeight;
					uint32 next = edit_mesh->vertices.at(vertex_index).next;
					edit_mesh->vertices.at(vertex_index).bone_index[next] = i;
					edit_mesh->vertices.at(vertex_index).bone_weights[next] = vertex_weight;
					edit_mesh->vertices.at(vertex_index).next++;
					Assert(edit_mesh->vertices.at(vertex_index).next < 4);
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
					edit_mesh->indices.push_back(face.mIndices[j]);
				}

			}
		}

		edit_mesh->has_positions = positions;
		edit_mesh->has_normals = normals;
		edit_mesh->has_texture_coords = texture_coords;
		edit_mesh->has_tanget_bitangets = tanget_bitangets;
		edit_mesh->has_colours = colours;
		edit_mesh->name = mesh->mName.C_Str();
	}

	void processNode(aiNode *node, DynaArray<EditableMesh> *meshes, const aiScene *scene)
	{		
		for (uint32 i = 0; i < node->mNumMeshes; i++)
		{					
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			
			EditableMesh edit_mesh;
			processMesh(mesh, &edit_mesh, scene);

			meshes->push_back(edit_mesh);
		}

		for (uint32 i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], meshes,  scene);
		}
	}

	int32 FindMinVertexWeight(Vertex vertex)
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

	void ProcessMeshCombine(aiMesh *mesh, ModeImport *model_import, const aiScene *scene)
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
		EditableMesh *emesh = &model_import->resulting_meshes.at(0);
		AnimationController *ac = &model_import->resulting_animation_controllers.at(0);
		uint32 mesh_indices_offset = emesh->vertices.size();

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
			if (normals && model_import->vertex_normals)
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}

			// Texture coordinates			
			if (texture_coords && model_import->vertex_texture_coords)
			{
				vertex.texture_coord.x = mesh->mTextureCoords[0][i].x;
				vertex.texture_coord.y = mesh->mTextureCoords[0][i].y;
			}

			// Tangent and bitangents
			if (tanget_bitangets && model_import->vertex_binorms_tangents)
			{
				vertex.tanget.x = mesh->mTangents[i].x;
				vertex.tanget.y = mesh->mTangents[i].y;
				vertex.tanget.z = mesh->mTangents[i].z;

				vertex.bitanget.x = mesh->mBitangents[i].x;
				vertex.bitanget.y = mesh->mBitangents[i].y;
				vertex.bitanget.z = mesh->mBitangents[i].z;
			}

			// Vertex Colours
			if (colours && model_import->vertex_colours)
			{
				Assert(0); // @TODO: Check before use
				vertex.colour.x = mesh->mColors[0]->r;
				vertex.colour.y = mesh->mColors[0]->g;
				vertex.colour.z = mesh->mColors[0]->b;
			}

			emesh->vertices.push_back(vertex);
		}

		if (bones && model_import->import_animations)
		{
			for (int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone *b = mesh->mBones[i];
				std::string cur_bone_name = b->mName.C_Str();
				int32 bone_index = -1;

				// @NOTE: Find the bone index
				// @SPEED: Again a hash map will solve some of this.
				for (uint32 j = 0; j < ac->bones.size(); j++)
				{
					std::string bone_name = ac->bones.at(j).name;
					
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
					Assert(model_import->vertex_weight_enforcement == false);

					real32 vertex_weight_cull = model_import->vertex_weight_cull;				
					// @NOTE: Finds the min vertex weight and store in index						
					//		: If current weight is greater than the lowest weight 
					//		: it is added else ignored
					if (vertex_weight >= vertex_weight_cull)
					{					
						int32 index = FindMinVertexWeight(emesh->vertices.at(vertex_index));
						real32 min_vertex_weight = emesh->vertices.at(vertex_index).bone_weights[index];
						
						if (vertex_weight > min_vertex_weight)
						{
							emesh->vertices.at(vertex_index).bone_index[index] = bone_index;
							emesh->vertices.at(vertex_index).bone_weights[index] = vertex_weight;
							if (min_vertex_weight != 0)
							{
								LOG("Replaced weight: " << min_vertex_weight << " With: " << vertex_weight << " Delta " << vertex_weight - min_vertex_weight);
							}							
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

	void ProcessMesh(aiNode *node, ModeImport *model_import, const aiScene * scene)
	{
		for (uint32 i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			std::cout << "Mesh name " << mesh->mName.C_Str() << std::endl;
			
			ProcessMeshCombine(mesh, model_import, scene);
		}

		for (uint32 i = 0; i < node->mNumChildren; i++)
		{
			ProcessMesh(node->mChildren[i], model_import, scene);
		}
	}

	void StoreAllBones(const aiMesh * mesh, AnimationController *ac)
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

	const aiNode* FindRootNodeOfBones(const aiNode *node, AnimationController *ac)
	{
		// @NOTE: If anything this will give us problems
		std::string name = node->mName.C_Str();
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

	aiMatrix4x4 CalcRootNodeTransformMatrix(const aiNode *node)
	{
		if (node->mParent == NULL)
		{
			return node->mTransformation;			
		}
		return CalcRootNodeTransformMatrix(node->mParent) * node->mTransformation;
	}

	void StoreNodeTransformMatrices(const aiNode *node, AnimationController *ac)
	{
		std::string name = node->mName.C_Str();
				
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

	void SortBoneParents(const aiNode *node, uint32 parent_index, AnimationController *ac)
	{
		std::string name = node->mName.C_Str();
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

	void SortBoneChildren(AnimationController *ac)
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

	void ProcessAnimationChannels(aiAnimation *anim, Animation *animation)
	{
		for (int i = 0; i < anim->mNumChannels; i++)
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

			for (int j = 0; j < ai->mNumPositionKeys; j++)
			{
				Vec3 pos = Vec3(ai->mPositionKeys[j].mValue.x, ai->mPositionKeys[j].mValue.y, ai->mPositionKeys[j].mValue.z);
				float time = ai->mPositionKeys[j].mTime;
				animation->frames[i].poskeys[j] = pos;
				animation->frames[i].postime[j] = time;
			}

			for (int j = 0; j < ai->mNumRotationKeys; j++)
			{
				Quat rot = Quat(ai->mRotationKeys[j].mValue.x, ai->mRotationKeys[j].mValue.y, ai->mRotationKeys[j].mValue.z, ai->mRotationKeys[j].mValue.w);
				float time = ai->mRotationKeys[j].mTime;
				animation->frames[i].rotkeys[j] = rot;
				animation->frames[i].rottime[j] = time;
			}

			for (int j = 0; j < ai->mNumScalingKeys; j++)
			{
				Vec3 pos = Vec3(ai->mScalingKeys[j].mValue.x, ai->mScalingKeys[j].mValue.y, ai->mScalingKeys[j].mValue.z);
				float time = ai->mScalingKeys[j].mTime;
				animation->frames[i].sclkeys[j] = pos;
				animation->frames[i].scltime[j] = time;
			}						
		}
	}

	void ProcessBones(const aiScene *scene, AnimationController *ac)
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

	void ProcessAnimations(const aiScene *scene, AnimationController *ac)
	{
		// @NTODO: Loop throught the all the animations, Processing bones should be independt on this.
		Assert(scene->mNumAnimations <= 1);
		if (!scene->HasAnimations())
		{
			return;
		}
		// @NOTE: Get current animation
		aiAnimation *anim = scene->mAnimations[0];

		// @NOTE: Create new animation
		Animation animation;
		animation.duration = anim->mDuration;
		animation.ticks_per_second = anim->mTicksPerSecond;
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

	void ProcessError(const std::string &err)
	{
		// @TODO: Log file ?
		LOG("ERROR:ASSIMP -> " << err);
	}






	bool LoadModel(ModeImport *model_import)
	{
		Assimp::Importer import;
				
		// @TODO: Process flags
		const aiScene *scene = import.ReadFile(model_import->path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);
		bool result = scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode;
				
		// @TODO: We do not support multi meshes
		Assert(model_import->import_mesh_combine == true);

		// @TODO: I know I know, new..
		//model_import->mesh_count = 1;
		//model_import->resulting_mesh = new EditableMesh();
		//
		//model_import->animation_controller_count = 1;
		//model_import->animation_controller = new AnimationController();

		//delete model_import->animation_controller;
		model_import->resulting_meshes.resize(1);
		model_import->resulting_animation_controllers.resize(1);

		if (result)
		{
			if (model_import->import_animations)
			{
				ProcessAnimations(scene, &model_import->resulting_animation_controllers[0]);
			}			
			ProcessMesh(scene->mRootNode, model_import, scene);
		}
		else
		{
			ProcessError(import.GetErrorString());
		}

		import.FreeScene();
		return result;
	}

	bool LoadModel(DynaArray<EditableMesh> *meshes, const std::string &path)
	{
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);
		aiMatrix4x4 mat = scene->mRootNode->mTransformation.Inverse();
		
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
		{
			std::cout << "ERROR::ASSIMP:: " << import.GetErrorString() << std::endl;
			return false;
		}

		processNode(scene->mRootNode, meshes, scene);
		bool hanim = scene->HasAnimations();
		//if (hanim)
		//{
		//	std::cout << "Animation count: " << scene->mNumAnimations << std::endl;
		//	aiAnimation *anim = scene->mAnimations[0];
		//	std::cout << "Channel count: " << anim->mNumChannels << std::endl;

		//	for (int i = 0; i < anim->mNumChannels; i++)
		//	{
		//		aiNodeAnim *ai = anim->mChannels[i];
		//		std::cout << ai->mNodeName.C_Str() << std::endl;
		//		std::cout << ai->mNumPositionKeys << std::endl;
		//		std::cout << ai->mNumRotationKeys << std::endl;
		//		std::cout << ai->mNumScalingKeys << std::endl;
		//	}
		//	aiMesh* mesh = scene->mMeshes[0];

		//	for (int i = 0; i < mesh->mNumBones; i++)
		//	{
		//		aiBone *b = mesh->mBones[i];
		//		std::cout << b->mName.C_Str() << std::endl;
		//	}

		//}
		
		EditableMesh* mesh = &meshes->at(0);
		for (int i = 1; i < meshes->size(); i++)
		{
			EditableMesh next = meshes->at(i);

			for (int j = 0; j < next.indices.size(); j++)
			{
				uint32 index = (mesh->vertices.size()) + next.indices.at(j);
				mesh->indices.push_back(index);

			}

			for (int j = 0; j < next.vertices.size(); j++)
			{
				mesh->vertices.push_back(next.vertices.at(j));
			}
					   
		}
	
		// @TODO: free s
		return true;
	}




}

