#pragma once
#include "Core.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>

namespace cm
{

	template<typename T>
	class DynaArray : public std::vector<T>
	{
	public:
		DynaArray() : std::vector<T>{}
		{

		}

		~DynaArray()
		{
			std::vector<T>::clear();
		}

		DynaArray(uint32 size) : std::vector<T>(size)
		{

		}

		DynaArray(std::initializer_list<T> init) : std::vector<T>{ init }
		{

		}

		inline void CopyFromPtr(T *ptr, uint32 size_bytes)
		{
			std::vector<T>::insert(std::vector<T>::end(), &ptr[0], &ptr[size_bytes / sizeof(T)]);
		}
	};


	/* @NOTES:
			1) The equal and copy functions are shallow clones
			2) Expects you to manually free the memory
			3) When you resize the array, all the old data is copied into the new buffer.
			   If the resize is smaller than the current size all the data that doesn't fit into
			   the new buffer will be lost.
			4) 
	*/
	template<typename T>
	class Array
	{		
	public:
		uint32 count = 0;
		T *data = nullptr;
		
	public:
		void Resize(uint32 count);
		void Free();

		T Get(uint32 index) const;
		void Set(uint32 index, const T &t);

		void ShallowClone(const Array<T> &src);
		void DeepClone(const Array<T> &src);

	public:
		Array();
		Array(const uint32 &size_bytes);
		~Array();
		// @TODO: Overload operators
		void operator = (const Array<T> &d) {
			DeepClone(d);
		}
	};


	//////////////////////
	// Array
	//////////////////////
	   	 
	template<typename T>
	cm::Array<T>::Array(const uint32 &size_bytes)
	{
		Resize(size_bytes);
	}

	template<typename T>
	cm::Array<T>::Array()
	{

	}

	template<typename T>
	cm::Array<T>::~Array()
	{
		Free();
	}


	template<typename T>
	void cm::Array<T>::DeepClone(const Array<T> &src)
	{
		Free(); 
		Resize(src.count);
		for (uint32 i = 0; i < src.count; i++)
		{
			this->data[i] = src.data[i];
		}
		this->count = src.count;
	}

	template<typename T>
	void cm::Array<T>::ShallowClone(const Array<T> &src)
	{
		Free();
		this->data = src.data;
		this->count = src.count;
	}

	template<typename T>
	void cm::Array<T>::Set(uint32 index, const T &t)
	{
		Assert(index < count);
		//memcpy((void *)(&data[index]), (const void *)(&t), sizeof(T));
		data[index] = t;
	}

	template<typename T>
	T cm::Array<T>::Get(uint32 index) const
	{
		Assert(index < count);
		return data[index];
	}

	template<typename T>
	void cm::Array<T>::Free()
	{		
		if (data)
		{
			count = 0;			
			delete data;
			data = nullptr;
		}		
	}

	template<typename T>
	void cm::Array<T>::Resize(uint32 count)
	{		
		uint32 size = count * sizeof(T);
		
		T* new_data = new T[count];	
		
		uint32 copy_count = this->count <= count ? this->count : count;
	
		for (uint32 i = 0; i < copy_count; i++)
		{			
			new_data[i] = data[i];
		}


		
		Free();
		data = new_data;
		this->count = count;
	}

}
