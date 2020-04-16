#pragma once
#include "Core.h"
#include "Utility.h"
#include "Math/CosmicMath.h"


namespace cm
{
	//Make this data orientated 
	class LayoutBuffer
	{
		uint32 number_components;
		uint32 stride;

		std::vector<ShaderDataType> layout;
		uint32 current_offset;
		uint32 current_next;
		uint32 attribute_divisor;

	public:
		LayoutBuffer();
		LayoutBuffer(std::vector<ShaderDataType> layout);		
		~LayoutBuffer();

		void Reset();
		void Next();
		void SetAttributeDivisor(uint32 div);
		uint32 Add(const LayoutBuffer &lbo);

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


