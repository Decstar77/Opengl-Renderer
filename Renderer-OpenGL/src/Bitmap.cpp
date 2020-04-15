#include "Bitmap.h"
namespace cm
{
	Bitmap::Bitmap() {

	}

	Bitmap::~Bitmap() {
		if (created)
		{
			LOG("WARNING: Freeing bmp, please free yourself");
			Free();
		}
	}


	bool Bitmap::Write(const std::string &filename) {

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

	void Bitmap::Create(int32 width, int32 height)
	{
		Assert(!created);
		pixels = new uint8[width * height * comps];
		created = true;
	}

	void Bitmap::Free()
	{
		Assert(created);
		delete pixels;
		created = false;
	}

	void Bitmap::SetPixel(int32 x, int32 y, real32 r, real32 g, real32 b)
	{
		uint8_t *pPixel = pixels;

		pPixel += (y * 3) * width + (x * 3);

		pPixel[0] = static_cast<uint8>(r * 255);
		pPixel[1] = static_cast<uint8>(r * 255);
		pPixel[2] = static_cast<uint8>(r * 255);
	}
}

