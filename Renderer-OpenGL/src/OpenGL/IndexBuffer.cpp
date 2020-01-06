#include "IndexBuffer.h"




namespace cm
{
	IndexBuffer::IndexBuffer() : buffer_id(0), index_count(0)
	{

	}

	IndexBuffer::IndexBuffer(std::vector<uint32>& data) : buffer_id(0), index_count(data.size())
	{
		glGenBuffers(1, &buffer_id);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<uint32>(data.size()) * sizeof(uint32), data.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	IndexBuffer::IndexBuffer(uint32 *data, uint32 size) : buffer_id(0), index_count(size/sizeof(uint32))
	{
		glGenBuffers(1, &buffer_id);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	IndexBuffer::~IndexBuffer()
	{

	}

	void IndexBuffer::SetIndices(std::vector<uint32>& data)
	{
		if (buffer_id == 0)
		{			
			glGenBuffers(1, &buffer_id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(uint32), data.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			index_count = data.size();
		}
		else
		{
			
			glDeleteBuffers(1, &buffer_id);

			glGenBuffers(1, &buffer_id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(uint32), data.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			index_count = data.size();
		}
	}

	void IndexBuffer::FreeBuffer()
	{
		if (buffer_id != 0)
		{
			glDeleteBuffers(1, &buffer_id);
		}		
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}
