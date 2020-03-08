#pragma once
#include "Core.h"
#include "Math/CosmicMath.h"
#include <fstream>


namespace cm
{
	template<typename T>
	struct FileDataTexture
	{
		DynaArray<T> data;
		uint32 width = 0;
		uint32 height = 0;
		uint32 channel = 0;
	};

	struct FileDataMesh
	{
		uint32 size_bytes;
		Vec3 min_vertex;
		Vec3 max_vertex;
		DynaArray<float> data;
		DynaArray<uint32> indices;
	};



	std::string ReadFile(const std::string &file_directory);
	
	FileDataTexture<uint8> LoadTexture(const std::string &file_directory);
	FileDataTexture<float> LoadHDRI(const std::string &file_directory);


	template<typename T>
	void ParseLineObj(DynaArray<T> &arr, std::string &line, char delim)
	{
		std::stringstream buffer(line);
		std::string value;
		if (delim == ' ')// @Hack just skips the first 
		{
			std::getline(buffer, value, delim);
		}
		while (std::getline(buffer, value, delim))
		{
			arr.push_back(static_cast<T>(std::stof(value)));
		}
	}

	void ParseObj(const std::string &file_directory, DynaArray<Vec3> *points, DynaArray<Vec3> *normals,
		DynaArray<Vec3> *texture_coords, DynaArray<uint32> *layout);

	FileDataMesh LoadMesh(const std::string &file_directory, bool tangents);
}



