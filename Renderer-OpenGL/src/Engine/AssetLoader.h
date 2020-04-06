#pragma once
#include "Core.h"
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Core/EditableMesh.h"

namespace cm
{
	std::string ReadFile(const std::string &file_directory);
	
	bool LoadTexture(DynaArray<uint8> *storage, TextureConfig *config, const std::string &file_directory);

	bool LoadModel(DynaArray<EditableMesh> *meshes, const std::string &path);

}



