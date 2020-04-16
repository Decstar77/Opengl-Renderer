#include "BufferLayout.h"

namespace cm
{
	LayoutBuffer::LayoutBuffer() : current_offset(0),
		current_next(0), number_components(0), stride(0), attribute_divisor(0)
	{

	}

	LayoutBuffer::LayoutBuffer(std::vector<ShaderDataType> layout) : layout(layout), current_offset(0),
		current_next(0), number_components((uint32)layout.size()), attribute_divisor(0)
	{
		stride = 0;

		for (int i = 0; i < layout.size(); i++)
		{
			stride += GetShaderDataTypeSize(layout.at(i));
		}
	}

	LayoutBuffer::~LayoutBuffer()
	{

	}

	void LayoutBuffer::Next()
	{
		current_offset += GetShaderDataTypeSize(layout.at(current_next));
		current_next++;
		Assert(current_next <= layout.size());
	}

	void LayoutBuffer::SetAttributeDivisor(uint32 div)
	{
		attribute_divisor = div;
	}

	uint32 LayoutBuffer::Add(const LayoutBuffer &lbo)
	{
		layout.insert(layout.end(), lbo.layout.begin(), lbo.layout.end());
		return static_cast<uint32>(layout.size());
	}

	void LayoutBuffer::Reset()
	{
		current_next = current_offset = 0;
	}

	uint32 LayoutBuffer::GetCurrentOffSet() const
	{
		return current_offset;
	}

	uint32 LayoutBuffer::GetCurrentSize() const
	{
		return GetShaderDataTypeSize(layout.at(current_next));
	}

	uint32 LayoutBuffer::GetCurrentComponentAttribCount() const
	{
		return GetShaderDataTypeComponentCount(layout.at(current_next));
	}

	uint32 LayoutBuffer::GetComponentCount() const
	{
		return (uint32)layout.size();
	}

	uint32 LayoutBuffer::GetStride() const
	{
		return stride;
	}

	uint32 LayoutBuffer::GetTotalSize() const
	{
		return stride;
	}

	uint32 LayoutBuffer::GetSizeOf(uint32 index) const
	{
		return GetShaderDataTypeSize(layout.at(index));
	}

	uint32 LayoutBuffer::GetAttributeDivisor() const
	{
		return attribute_divisor;
	}

	ShaderDataType LayoutBuffer::GetCurrentShaderType() const
	{
		return layout.at(current_next);
	}

	uint32 LayoutBuffer::GetTotalAttributeCount() const
	{
		uint32 count = 0;
		for (int32 i = 0; i < layout.size(); i++)
		{
			if (layout.at(i) == ShaderDataType::Mat4)
			{
				count += 4;
			}
			else if (layout.at(i) == ShaderDataType::Mat3)
			{
				count += 3;
			}
			else
			{
				count += 1;
			}
		}
		return count;
	}

}