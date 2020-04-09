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




	void processMesh(aiMesh *mesh, EditableMesh *edit_mesh, const aiScene *scene)
	{
		// Walk through each of the mesh's vertices

		bool positions = mesh->HasPositions();
		bool normals = mesh->HasNormals();
		bool texture_coords = mesh->HasTextureCoords(0);
		bool tanget_bitangets = mesh->HasTangentsAndBitangents();
		bool colours = mesh->HasVertexColors(0);
		bool faces = mesh->HasFaces();
		bool hbone = mesh->HasBones();

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

	Mat4 ToMatrix4f(const aiMatrix4x4 *ai_mat)
	{
		Assert(sizeof(aiMatrix4x4) == sizeof(Matrix4f));
		uint32 size = sizeof(Mat4);

		
		Mat4 a;
		a.row0 = Vec4(ai_mat->a1, ai_mat->a2, ai_mat->a3, ai_mat->a4);
		a.row1 = Vec4(ai_mat->b1, ai_mat->b2, ai_mat->b3, ai_mat->b4);
		a.row2 = Vec4(ai_mat->c1, ai_mat->c2, ai_mat->c3, ai_mat->c4);
		a.row3 = Vec4(ai_mat->d1, ai_mat->d2, ai_mat->d3, ai_mat->d4);
		a = Transpose(a);
		return a;
	}

	Mat4 ToMatrix4f(const Matrix4f *ai_mat)
	{
		Assert(sizeof(aiMatrix4x4) == sizeof(Matrix4f));
		uint32 size = sizeof(Matrix4f);

		Matrix4f ai = ai_mat->Transpose();
		Mat4 a;
		memcpy((void*)&a, (void *)&ai, size);

		return a;
	}

	void doAnim(aiNode *node, Animation *anim, aiMatrix4x4 parent )
	{
		aiMatrix4x4 local = node->mTransformation;
		aiMatrix4x4 global_transform = parent * local;
		
		if (node->mName.C_Str() == anim->channels[0].name)
		{
			anim->bones[1].node_transform = ToMatrix4f(&global_transform);
			return;
		}

		for (uint32 i = 0; i < node->mNumChildren; i++)
		{
			doAnim(node->mChildren[i], anim, global_transform);
		}
	}


	bool LoadModelTest(DynaArray<EditableMesh> *meshes, const std::string &path)
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
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
				
		if (hanim)
		{			
			EditableMesh *emesh = &meshes->at(0);
			emesh->animation.global_inverse_transform = ToMatrix4f(&mat);
			
	

			std::cout << "Animation count: " << scene->mNumAnimations << std::endl;
			std::cout << "Mesh count: " << scene->mNumMeshes << std::endl;
			aiAnimation *anim = scene->mAnimations[0];
			std::cout << std::endl;
			std::cout << "Animation name: " << anim->mName.C_Str() << std::endl;
			std::cout << "Morph count: " << anim->mNumMorphMeshChannels << std::endl;
			std::cout << "Mesh count: " << anim->mNumMeshChannels << std::endl;
			std::cout << "Channel count: " << anim->mNumChannels << std::endl;
			std::cout << std::endl;
			for (int i = 0; i < anim->mNumChannels; i++)
			{
				aiNodeAnim *ai = anim->mChannels[i];
				
				std::cout << "Node name: " << ai->mNodeName.C_Str() << std::endl;
				std::cout << "Pos key count" << ai->mNumPositionKeys << std::endl;
				emesh->animation.channels[i].name = ai->mNodeName.C_Str();
				for (int j = 0; j < ai->mNumPositionKeys; j++)
				{

					Vec3 pos = Vec3(ai->mPositionKeys[j].mValue.x, ai->mPositionKeys[j].mValue.y, ai->mPositionKeys[j].mValue.z);
					float time = ai->mPositionKeys[j].mTime;
					emesh->animation.channels[i].poskeys[j] = pos;
					emesh->animation.channels[i].postime[j] = time;
					//std::cout << ai->mPositionKeys[j].mTime << " " << std::endl;
					//std::cout << ai->mPositionKeys[j].mValue.x << " ";
					//std::cout << ai->mPositionKeys[j].mValue.y << " ";
					//std::cout << ai->mPositionKeys[j].mValue.x << " ";
				}

				std::cout << "Rot key count" << ai->mNumRotationKeys << std::endl;
				for (int j = 0; j < ai->mNumRotationKeys; j++)
				{
					Quat rot = Quat(ai->mRotationKeys[j].mValue.x, ai->mRotationKeys[j].mValue.y, ai->mRotationKeys[j].mValue.z, ai->mRotationKeys[j].mValue.w);
					float time = ai->mRotationKeys[j].mTime;
					emesh->animation.channels[i].rotkeys[j] = rot;
					emesh->animation.channels[i].rottime[j] = time;
					//std::cout << "Time: "<< ai->mRotationKeys[j].mTime << " " << std::endl;
					//std::cout << ai->mRotationKeys[j].mValue.x << " ";
					//std::cout << ai->mRotationKeys[j].mValue.y << " ";
					//std::cout << ai->mRotationKeys[j].mValue.x << " ";
					//std::cout << std::endl;
				}
							   
				std::cout << "Scl key count" << ai->mNumScalingKeys << std::endl;
				for (int j = 0; j < ai->mNumScalingKeys; j++)
				{
					
					Vec3 pos = Vec3(ai->mScalingKeys[j].mValue.x, ai->mScalingKeys[j].mValue.y, ai->mScalingKeys[j].mValue.z);
					float time = ai->mScalingKeys[j].mTime;
					emesh->animation.channels[i].sclkeys[j] = pos;
					emesh->animation.channels[i].scltime[j] = time;					
					//std::cout << ai->mScalingKeys[j].mTime << " " << std::endl;
					//std::cout << ai->mScalingKeys[j].mValue.x << " ";
					//std::cout << ai->mScalingKeys[j].mValue.y << " ";
					//std::cout << ai->mScalingKeys[j].mValue.x << " ";
				}
			}

			aiMesh* mesh = scene->mMeshes[0];
			std::cout << std::endl;
			std::cout << "Bone count: " << mesh->mNumBones << std::endl;
			std::cout << "Vertex count: " << mesh->mNumVertices << std::endl;
			emesh->animation.bones[0].child = &emesh->animation.bones[1];
			emesh->animation.bones[1].child = &emesh->animation.bones[2];
			emesh->animation.bones[2].child = nullptr;

			for (int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone *b = mesh->mBones[i];
				std::cout << b->mName.C_Str() << std::endl;

				emesh->animation.bones[i].name = b->mName.C_Str();
				emesh->animation.bones[i].inverse_bind_pose = ToMatrix4f(&b->mOffsetMatrix);	
				Print(ToMatrix4f(&b->mOffsetMatrix));
				Matrix4f m = Matrix4f(b->mOffsetMatrix);
				
				for (int j = 0; j < b->mNumWeights; j++)
				{
					aiVertexWeight vertex_data = b->mWeights[j];

					uint32 vertex_index = vertex_data.mVertexId;
					float vertex_weight = vertex_data.mWeight;
					uint32 next = emesh->vertices.at(vertex_index).next;
					emesh->vertices.at(vertex_index).bone_index[next] = i;
					emesh->vertices.at(vertex_index).bone_weights[next] = vertex_weight;
					emesh->vertices.at(vertex_index).next++;
					Assert(emesh->vertices.at(vertex_index).next < 4);					
				}				
			}

			aiMatrix4x4 identity_matrix;
			doAnim(scene->mRootNode, &emesh->animation, identity_matrix);
			
			//for (int i = 0; i < emesh->vertices.size(); i++)
			//{
			//	Vertex v = emesh->vertices.at(i);
			//	for (int j = 0; j < 4; j++)
			//	{
			//		std::cout << "J = " << j << std::endl;
			//		std::cout << "Bone index: " << v.bone_index[j] << std::endl;
			//		std::cout << "Bone weights: " << v.bone_weights[j] << std::endl;
			//	}
			//	
			//}



		}



		// @TODO: free s
		return true;
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
		if (hanim)
		{
			std::cout << "Animation count: " << scene->mNumAnimations << std::endl;
			aiAnimation *anim = scene->mAnimations[0];
			std::cout << "Channel count: " << anim->mNumChannels << std::endl;

			for (int i = 0; i < anim->mNumChannels; i++)
			{
				aiNodeAnim *ai = anim->mChannels[i];
				std::cout << ai->mNodeName.C_Str() << std::endl;
				std::cout << ai->mNumPositionKeys << std::endl;
				std::cout << ai->mNumRotationKeys << std::endl;
				std::cout << ai->mNumScalingKeys << std::endl;
			}
			aiMesh* mesh = scene->mMeshes[0];

			for (int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone *b = mesh->mBones[i];
				std::cout << b->mName.C_Str() << std::endl;
			}

		}
		
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

