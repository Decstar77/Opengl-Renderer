#pragma once
#include "Core.h"
#include "Math/CosmicMath.h"
#include "OpenGL//OpenGL.h"



namespace cm
{
#pragma pack(push, 1)
	struct BitmapInfoHeader {
		int32 headerSize{ 40 };
		int32 width;
		int32 height;
		int16 planes{ 1 };
		int16 bitsPerPixel{ 24 };
		int32 compression{ 0 };
		int32 dataSize{ 0 };
		int32 horizontalResolution{ 2400 };
		int32 verticalResolution{ 2400 };
		int32 colors{ 0 };
		int32 importantColors{ 0 };
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct BitmapFileHeader {
		char header[2]{ 'B', 'M' };
		int32_t fileSize;
		int32_t reserved{ 0 };
		int32_t dataOffset;
	};
#pragma pack(pop)

	// @NOTE: Trying not to std::vector
	// NOTE: This is more the editable mesh but for textures. Maybe we have something like Creators.h ??? Editables.h ?
	class BitmapExport
	{
	private:
		uint32 width = 0;
		uint32 height = 0;
		uint32 comps = 3;
		uint32 size_bytes;
		uint8 *pixels = nullptr;
		bool created = false;

	public:
		BitmapExport();
		~BitmapExport();
		void SetPixel(int32 x, int32 y, real32 r, real32 g, real32 b);
		void ReadTexture(const Texture &texture);

		void Create(int32 width, int32 height);
		bool Write(const String &filename);
		void Free();

	};

	// @NOTE: Trying not to std::vector
	// TODO: Make this able to write some binary format
	class MeshExport
	{
	private:
		uint32 vertex_count = 0;
		real32 *vertex_data = nullptr;

		uint32 index_count = 0;
		uint32 *index_data = nullptr;

		bool created = false;

	public:
		void Create(const GLMesh &mesh);
		void Write(const String &file_name);
		void Free();

	public:
		MeshExport();
		~MeshExport();
	};


}