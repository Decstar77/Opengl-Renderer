#pragma once
#include "Core.h"

namespace cm
{
	template<typename T>
	class Array
	{
	private:
		uint32 size = 0;
		T *data = nullptr;

	public:
		inline constexpr uint32 Size() const { 
			return size; 
		}

		inline constexpr T* Data() const { 
			return data; 
		}
		
		inline constexpr T* Get(const uint32 &index) { 
			Assert(index >= 0 && index < size); 
			return this->data[index]; 
		}
		inline constexpr void Set(const T &data, const uint32 &index) { 
			Assert(index >= 0 && index < size); 
			this->data[index] = data; 
		}

		void Free()
		{
			if (data)
			{
				delete[] data;
				size = 0;
				data = nullptr;
			}
		}

		void Resize(const uint32 &size)
		{
			uint32 copycount = this->size < size ? size : this->size;
			T *newdata = new T[copycount];
			if (data)
			{
				for (uint32 i = 0; i < copycount; i++)
				{
					newdata[i] = data[i];
				}
			}
			Free();
			this->data = newdata;
			this->size = size;
		}

		void Copy(T *data, const uint32 &size)
		{
			Assert(size <= this->size);
			for (uint32 i = 0; i < size; i++)
			{
				this->data[i] = data[i];
			}
		}

		Array<T> Clone() const
		{
			Array newarr;
			if (data)
			{
				newarr.data = new T[this->size];
				newarr.size = this->size;
				for (uint32 i = 0; i < this->size; i++)
				{
					newarr.data[i] = this->data[i];
				}
			}
			return newarr;
		}

		T& operator[](const uint32 &index)
		{
			Assert(index >= 0 && index < size);
			return data[index];
		}

		T operator[](const uint32 &index) const
		{
			Assert(index >= 0 && index < size);
			return data[index];
		}

		bool operator ==(const Array<T> &arr)
		{
			return (this->data == arr.data && this->size == arr.size);
		}

		void operator = (const Array<T> &arr) {
			this->data = arr.data;
			this->size = arr.size;
		}

	public:
		Array() {}

		Array(const Array<T> &arr) 
		{
			this->data = arr.data;
			this->size = arr.size;
		}

		Array(T *data, const uint32 &size) 
		{

			this->data = data;
			this->size = size;
		}

		Array(const uint32 &size) 
		{
			Resize(size);
		}

		Array(std::initializer_list<T> init_list) 
		{
			uint32 nsize = static_cast<uint32>(init_list.size());
			Resize(nsize);
			uint32 next = 0;
			for (const T &t : init_list)
			{
				this->data[next++] = t;
			}
		}

		~Array() {}
	};

}
