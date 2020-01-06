#pragma once
#include "Core.h"
#include "glew.h"
#include "Math/CosmicMath.h"

namespace cm
{
	class IndexBuffer
	{
	private:				
		uint32 buffer_id;
		uint32 index_count;
	public:
		IndexBuffer();
		IndexBuffer(std::vector<uint32>& data);
		IndexBuffer(uint32 *data, uint32 size);
		~IndexBuffer();

		void SetIndices(std::vector<uint32>& data);

		void FreeBuffer();
		void Bind() const;
		void Unbind() const;

		inline uint32 GetCount() const { return index_count; }
		
	};


}