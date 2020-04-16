#include "Serialization.h"
namespace cm
{
	BitmapExport::BitmapExport() {

	}

	BitmapExport::~BitmapExport() {
		if (created)
		{
			LOG("WARNING: Freeing bmp, please free yourself");
			Free();
		}
	}


	bool BitmapExport::Write(const std::string &filename) {

		BitmapFileHeader fileHeader;
		BitmapInfoHeader infoHeader;

		fileHeader.fileSize = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + width * height * comps;
		fileHeader.dataOffset = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);

		infoHeader.width = width;
		infoHeader.height = height;

		std::ofstream file;
		file.open(filename, std::ios::out | std::ios::binary);
		if (!file)
		{
			LOG("Could not open/create bitmap")
				return false;
		}

		file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
		file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
		file.write(reinterpret_cast<char*>(pixels), width * height * comps);
		file.close();

		if (!file)
		{
			LOG("Could not close/complete bitmap")
				return false;
		}

		return true;
	}

	void BitmapExport::Create(int32 width, int32 height)
	{
		Assert(!created);
		pixels = new uint8[width * height * comps];
		created = true;
	}

	void BitmapExport::Free()
	{
		Assert(created);
		delete pixels;
		created = false;
	}

	void BitmapExport::SetPixel(int32 x, int32 y, real32 r, real32 g, real32 b)
	{
		uint8_t *pPixel = pixels;

		pPixel += (y * 3) * width + (x * 3);

		pPixel[0] = static_cast<uint8>(r * 255);
		pPixel[1] = static_cast<uint8>(r * 255);
		pPixel[2] = static_cast<uint8>(r * 255);
	}

	void MeshExport::Create(const GLMesh &mesh)
	{		
		uint32 vertex_bytes = mesh.vao.vertex_buffers[0].size_bytes;
		uint32 index_bytes = mesh.ibo.size_bytes;

		// @NOTE: Could be formated nicely with this: mesh.vao.vertex_buffers[0].lbo;

		Assert(vertex_bytes != 0);
		Assert(index_bytes != 0);

		vertex_count = vertex_bytes / sizeof(real32);
		index_count = index_bytes / sizeof(uint32);

		vertex_data = new real32[vertex_count];
		index_data = new uint32[index_count];
				
		ReadBufferData(mesh.vao.vertex_buffers[0], vertex_data, vertex_bytes, 0);
		ReadBufferData(mesh.ibo, index_data, index_bytes, 0);

		created = true;
	}

	void MeshExport::Write(const std::string &file_name)
	{
		std::ofstream file;
		file.open(file_name, std::ios::out);

		Assert(file.is_open());

		// @SPEED: Use file.write();
		file << "Vertex data: " << std::endl;
		for (uint32 i = 0; i < vertex_count; i++)
		{
			if (i % 10 == 0)
			{
				file << std::endl;
			}
			real32 vert = vertex_data[i];
			std::string svert = std::to_string(vert);
			file << svert << ", ";
		}

		file << std::endl << "Index data: " << std::endl;
		for (uint32 i = 0; i < index_count; i++)
		{
			if (i % 10 == 0)
			{
				file << std::endl;
			}
			real32 ind = index_data[i];
			std::string sind = std::to_string(ind);
			file << sind << ", ";
		}

		file.close();
	}

	void MeshExport::Free()
	{
		if (created)
		{
			delete vertex_data;
			delete index_data;
			created = false;
		}
	}

	MeshExport::MeshExport()
	{

	}

	MeshExport::~MeshExport()
	{

	}

}

