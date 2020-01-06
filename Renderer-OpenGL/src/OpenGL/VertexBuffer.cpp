#include "VertexBuffer.h"


namespace cm
{

	//VertexBuffer::VertexBuffer() : buffer_id(0), total_bytes(0)
	//{

	//}

	//VertexBuffer::VertexBuffer(std::vector<float>& data, VertexMode flags) : buffer_id(0), total_bytes(data.size() * sizeof(float))
	//{
	//	glGenBuffers(1, &buffer_id);
	//	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);		
	//	glBufferStorage(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), static_cast<uint32>(flags));		
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//}

	//VertexBuffer::VertexBuffer(const uint32 &reserved_size, VertexMode flags)
	//{
	//	glGenBuffers(1, &buffer_id);
	//	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);		
	//	glBufferStorage(GL_ARRAY_BUFFER, reserved_size, NULL, static_cast<uint32>(flags));		
	//}

	//VertexBuffer::~VertexBuffer()
	//{

	//}

	//void VertexBuffer::CopyInto(VertexBuffer &vbo, VertexMode flags) const
	//{
	//	Assert(buffer_id != 0);
	//	vbo.FreeBuffer();
	//	vbo = VertexBuffer(total_bytes, flags);
	//	glBindBuffer(GL_COPY_READ_BUFFER, buffer_id);
	//	glBindBuffer(GL_COPY_WRITE_BUFFER, vbo.buffer_id);

	//	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, total_bytes);
	//	vbo.total_bytes = total_bytes;
	//}

	//void VertexBuffer::FreeBuffer()
	//{
	//	if (buffer_id != 0)
	//	{
	//		glDeleteBuffers(1, &buffer_id);
	//	}
	//}

	//void VertexBuffer::Bind()
	//{
	//	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	//}

	//void VertexBuffer::Unbind()
	//{
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//}



}