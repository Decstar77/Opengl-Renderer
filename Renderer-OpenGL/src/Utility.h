#pragma once
#include "Core.h"

namespace cm
{
	template<typename T>
	class Array
	{
	private:
		uint32 count = 0;
		T *data = nullptr;

	public:
		inline constexpr uint32 Count() const { 
			return count; 
		}

		inline constexpr T* Data() const { 
			return data; 
		}
		
		inline constexpr T* Get(const uint32 &index) { 
			Assert(index >= 0 && index < count); 
			return this->data[index]; 
		}
		inline constexpr void Set(const T &data, const uint32 &index) { 
			Assert(index >= 0 && index < count); 
			this->data[index] = data; 
		}

		void Free()
		{
			if (data)
			{
				delete[] data;
				count = 0;
				data = nullptr;
			}
		}

		void Resize(const uint32 &count)
		{
			uint32 copycount = this->count < count ? count : this->count;
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
			this->count = count;
		}

		Array<T> Clone() const
		{
			Array newarr;
			if (data)
			{
				newarr.data = new T[this->count];
				newarr.count = this->count;
				for (uint32 i = 0; i < this->count; i++)
				{
					newarr.data[i] = this->data[i];
				}
			}
			return newarr;
		}

		T& operator[](uint32 index)
		{
			Assert(index >= 0 && index < count);
			return data[index];
		}

		bool operator ==(const Array<T> &arr)
		{
			return (this->data == arr.data && this->count == arr.count);
		}

		void operator = (const Array<T> &arr) {
			this->data = arr.data;
			this->count = arr.count;
		}

	public:
		Array() {}

		Array(const Array<T> &arr) {
			this->data = arr.data;
			this->count = arr.count;
		}

		Array(T *data, const uint32 &count) {

			this->data = data;
			this->count = count;
		};

		Array(const uint32 &count) {
			Resize(count);
		};

		~Array() {}
	};

}
