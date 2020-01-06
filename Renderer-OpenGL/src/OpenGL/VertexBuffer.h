#pragma once
#include "Core.h"
#include "glew.h"
#include "Math/CosmicMath.h"

namespace cm
{


	////class VertexBuffer
	////{
	////private:
	////	uint32 buffer_id;
	////	uint32 total_bytes;
	////public:
	////	VertexBuffer();
	////	VertexBuffer(const uint32 &reserved_size, VertexMode flags = VertexMode::CM_READ_WRITE);
	////	VertexBuffer(std::vector<float>& data, VertexMode flags = VertexMode::CM_READ_WRITE);
	////	~VertexBuffer();
	////

	////	template <typename T>
	////	void ReadData(std::vector<T>* data, uint32 size_byte, uint32 offset_bytes);
	////	template <typename T>
	////	void WriteData(const std::vector<T>& data, uint32 offset_bytes);

	////	void FreeBuffer();
	////	void Bind();
	////	void Unbind();
	////	void CopyInto(VertexBuffer &vbo, VertexMode mode = VertexMode::CM_READ_WRITE) const;

	////	inline uint32 Size() const { return total_bytes; }
	////};

	////template <typename T>
	////void VertexBuffer::ReadData(std::vector<T>* data, uint32 size_byte, uint32 offset_bytes)
	////{
	////	Assert(buffer_id != 0);
	////	size_byte = size_byte == 0 ? (total_bytes - offset_bytes) : size_byte;
	////	Assert(offset_bytes + size_byte <= total_bytes);

	////	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

	////	void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	////	ptr = static_cast<char*>(ptr) + offset_bytes;

	////	float *f_ptr = static_cast<T*>(ptr);

	////	data->insert(data->end(), &f_ptr[0], &f_ptr[size_byte / sizeof(T)]);

	////	glUnmapBuffer(GL_ARRAY_BUFFER);
	////	glBindBuffer(GL_ARRAY_BUFFER, 0);
	////}

	////template <typename T>
	////void VertexBuffer::WriteData(const std::vector<T>& data, uint32 offset_bytes)
	////{
	////	Assert(buffer_id != 0);
	////	Assert(offset_bytes + data.size() * sizeof(T) <= total_bytes);

	////	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

	////	void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	////	ptr = static_cast<char*>(ptr) + offset_bytes;

	////	const void * new_data = static_cast<const void *>(data.data());
	////	uint32 size = sizeof(T) * static_cast<uint32>(data.size());

	////	memcpy(ptr, new_data, size);

	////	glUnmapBuffer(GL_ARRAY_BUFFER);

	////	glBindBuffer(GL_ARRAY_BUFFER, 0);
	////}

}