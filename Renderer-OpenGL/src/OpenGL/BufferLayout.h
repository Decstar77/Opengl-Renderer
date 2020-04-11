#pragma once
#include "Core.h"
#include "Utility.h"
#include "Math/CosmicMath.h"


namespace cm
{
	//Make this data orientated 
	class BufferLayout
	{
		uint32 number_components;
		uint32 stride;

		DynaArray<ShaderDataType> layout;
		uint32 current_offset;
		uint32 current_next;
		uint32 attribute_divisor;

	public:
		BufferLayout();
		BufferLayout(DynaArray<ShaderDataType> layout);		
		~BufferLayout();

		void Reset();
		void Next();
		void SetAttributeDivisor(uint32 div);
		uint32 Add(const BufferLayout &lbo);

		ShaderDataType GetCurrentShaderType() const;
		uint32 GetTotalAttributeCount() const;
		uint32 GetCurrentOffSet() const;
		uint32 GetCurrentSize() const;
		uint32 GetCurrentComponentAttribCount() const;
		uint32 GetComponentCount() const;
		uint32 GetStride() const;
		uint32 GetTotalSize() const;
		uint32 GetSizeOf(uint32 index) const;
		uint32 GetAttributeDivisor() const;
	};



}


