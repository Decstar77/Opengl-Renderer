#pragma once
#include "Core.h"
#include <memory>
#include <fstream>

namespace cm
{

#pragma pack(2)
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

#pragma pack(2)
	struct BitmapFileHeader {
		char header[2]{ 'B', 'M' };
		int32_t fileSize;
		int32_t reserved{ 0 };
		int32_t dataOffset;
	};


	// TODO: Make nice
	class Bitmap
	{
	private:
		uint32 width = 0;
		uint32 height = 0;
		uint32 comps = 3;
		uint32 size_bytes;
		uint8 *pixels = nullptr;
		bool created = false;

	public:
		Bitmap();
		~Bitmap();
		void SetPixel(int32 x, int32 y, real32 r, real32 g, real32 b);
		bool Write(const std::string &filename);

		void Create(int32 width, int32 height);
		void Free();

	};

}