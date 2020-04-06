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
	}

	void processNode(aiNode *node, DynaArray<EditableMesh> *meshes, const aiScene *scene)
	{
		// process each mesh located at the current node
		for (uint32 i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			
			EditableMesh edit_mesh;
			processMesh(mesh, &edit_mesh, scene);

			meshes->push_back(edit_mesh);
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (uint32 i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], meshes,  scene);
		}
	}


	bool LoadModel(DynaArray<EditableMesh> *meshes, const std::string &path)
	{
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		// @TODO: This assert is extreme.
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::cout << "ERROR::ASSIMP:: " << import.GetErrorString() << std::endl;
			return false;
		}

		processNode(scene->mRootNode, meshes, scene);

		return true;
	}




}

