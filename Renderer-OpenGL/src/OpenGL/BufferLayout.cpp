#include "BufferLayout.h"

namespace cm
{
	BufferLayout::BufferLayout() : current_offset(0),
		current_next(0), number_components(0), stride(0), attribute_divisor(0)
	{

	}

	BufferLayout::BufferLayout(std::vector<ShaderDataType> layout) : layout(layout), current_offset(0),
		current_next(0), number_components((uint32)layout.size()), attribute_divisor(0)
	{
		stride = 0;

		for (int i = 0; i < layout.size(); i++)
		{
			stride += GetShaderDataTypeSize(layout.at(i));
		}
	}

	BufferLayout::~BufferLayout()
	{

	}

	void BufferLayout::Next()
	{
		current_offset += GetShaderDataTypeSize(layout.at(current_next));
		current_next++;
		Assert(current_next <= layout.size());
	}

	void BufferLayout::SetAttributeDivisor(uint32 div)
	{
		attribute_divisor = div;
	}

	uint32 BufferLayout::Add(const BufferLayout &lbo)
	{
		layout.insert(layout.end(), lbo.layout.begin(), lbo.layout.end());
		return static_cast<uint32>(layout.size());
	}

	void BufferLayout::Reset()
	{
		current_next = current_offset = 0;
	}

	uint32 BufferLayout::GetCurrentOffSet() const
	{
		return current_offset;
	}

	uint32 BufferLayout::GetCurrentSize() const
	{
		return GetShaderDataTypeSize(layout.at(current_next));
	}

	uint32 BufferLayout::GetCurrentComponentAttribCount() const
	{
		return GetShaderDataTypeComponentCount(layout.at(current_next));
	}

	uint32 BufferLayout::GetComponentCount() const
	{
		return (uint32)layout.size();
	}

	uint32 BufferLayout::GetStride() const
	{
		return stride;
	}

	uint32 BufferLayout::GetTotalSize() const
	{
		return stride;
	}

	uint32 BufferLayout::GetSizeOf(uint32 index) const
	{
		return GetShaderDataTypeSize(layout.at(index));
	}

	uint32 BufferLayout::GetAttributeDivisor() const
	{
		return attribute_divisor;
	}

	ShaderDataType BufferLayout::GetCurrentShaderType() const
	{
		return layout.at(current_next);
	}

	uint32 BufferLayout::GetTotalAttributeCount() const
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