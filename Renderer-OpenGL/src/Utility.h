#pragma once
#include "Core.h"

namespace cm
{
	template<typename T, typename Q = uint32>
	class Array
	{
	private:

		Q size		= static_cast<Q>(0);
		T *data		= nullptr;

	public:

		inline constexpr Q Size() const noexcept 
		{ 
			return this->size; 
		}

		inline constexpr T* Data() const noexcept 
		{
			return this->data; 
		}
		
		inline constexpr T* Get(const Q &index) noexcept 
		{
			Assert(index >= static_cast<Q>(0) && index < this->size);

			return this->data[index]; 
		}
		
		inline constexpr void Set(const T &data, const Q &index) noexcept 
		{
			Assert(index >= static_cast<Q>(0) && index < this->size);

			this->data[index] = data; 
		}

		void Free() 
		{
			if (this->data)
			{
				delete[] this->data;

				this->size = static_cast<Q>(0.0);
				this->data = nullptr;
			}
		}

		void Resize(const Q &size)
		{
			Q copycount = this->size < size ? size : this->size;
			T *newdata = new T[copycount];
			
			if (this->data)
			{
				for (Q i = static_cast<Q>(0); i < copycount; i++)
				{
					newdata[i] = this->data[i];
				}
			}

			Free();

			this->data = newdata;
			this->size = size;
		}

		void Copy(T *data, const Q &size)
		{
			Assert(size <= this->size);

			for (Q i = static_cast<Q>(0); i < size; i++)
			{
				this->data[i] = data[i];
			}
		}

		Array<T, Q> Clone() const
		{
			Array<T, Q> newarr;

			if (this->data)
			{
				newarr.data = new T[this->size];
				newarr.size = this->size;

				for (Q i = static_cast<Q>(0); i < this->size; i++)
				{
					newarr.data[i] = this->data[i];
				}
			}

			return newarr;
		}

		inline T& operator[](const Q &index)
		{
			Assert(index >= static_cast<Q>(0) && index < this->size);

			return data[index];
		}

		inline T operator[](const Q &index) const
		{
			Assert(index >= static_cast<Q>(0) && index < this->size);

			return data[index];
		}

		inline bool32 operator ==(const Array<T, Q> &arr)
		{
			bool32 result = this->data == arr.data && this->size == arr.size;
			
			return result;
		}

		inline void operator =(const Array<T, Q> &arr)
		{
			this->data = arr.data;
			this->size = arr.size;
		}

	public:

		Array() 
		{

		}

		Array(const Array<T> &arr) 
		{
			this->data = arr.data;
			this->size = arr.size;
		}

		Array(T *data, const Q &size) 
		{
			this->data = data;
			this->size = size;
		}

		Array(const Q &size) 
		{
			Resize(size);
		}

		Array(std::initializer_list<T> init_list) 
		{
			Q nsize = static_cast<Q>(init_list.size());
			Q next = static_cast<Q>(0);
			
			Resize(nsize);
			
			for (const T &t : init_list)
			{
				this->data[next] = t;
				next++;
			}
		}

		~Array() 
		{

		}
	};


	template<typename T, typename Q = uint32>
	class Stack
	{
	private:
		
		Q size		= static_cast<Q>(0);
		Q current	= static_cast<Q>(0);
		T *data		= nullptr;

	public:
		
		inline constexpr Q Size() const noexcept 
		{
			return this->size;
		}

		inline constexpr T* Data() const noexcept 
		{
			return this->data;
		}

		inline constexpr T* Get(const Q &index) noexcept 
		{
			Assert(index >= static_cast<Q>(0) && index < this->size);
			
			return this->data[index];
		}

		inline constexpr void Set(const T &data, const Q &index) noexcept 
		{
			Assert(index >= static_cast<Q>(0) && index < this->size);
			
			this->data[index] = data;
		}

		inline constexpr void Push(const T &data) noexcept
		{
			Assert(this->current < this->size);
			
			this->data[current] = data;
			this->current++;
		}

		inline constexpr T Pop() noexcept
		{
			Assert(this->current > static_cast<Q>(0));
			
			this->current--;

			return this->data[current + static_cast<Q>(1)];
		}

		inline constexpr T Peek() noexcept
		{
			Assert(this->current >= static_cast<Q>(0));

			return this->data[current];
		}

		void Free()
		{
			if (data)
			{
				delete[] data;
			
				size = static_cast<Q>(0);
				current = static_cast<Q>(0);
				
				data = nullptr;
			}
		}

		void Resize(const Q &size)
		{
			Q copycount = this->size < size ? size : this->size;
			
			T *newdata = new T[copycount];
			
			if (data)
			{
				for (Q i = static_cast<Q>(0); i < copycount; i++)
				{
					newdata[i] = data[i];
				}
			}
			
			Free();

			this->data = newdata;
			this->size = size;
		}

		void Copy(T *data, const Q &size)
		{
			Assert(size <= this->size);
			
			for (Q i = static_cast<Q>(0); i < size; i++)
			{
				this->data[i] = data[i];
			}

			current += size;
		}

		Stack<T, Q> Clone() const
		{
			Stack<T, Q> newstk;
			if (data)
			{
				newstk.data = new T[this->size];
				newstk.size = this->size;
				newstk.current = this->current;
				for (Q i = static_cast<Q>(0); i < this->size; i++)
				{
					newstk.data[i] = this->data[i];
				}
			}

			return newstk;
		}

		inline T& operator[](const Q &index)
		{
			Assert(index >= static_cast<Q>(0) && index < size);

			return data[index];
		}

		inline T operator[](const Q &index) const
		{
			Assert(index >= static_cast<Q>(0) && index < size);

			return data[index];
		}

		inline bool32 operator ==(const Stack<T, Q> &arr)
		{
			return (this->data == arr.data && this->size == arr.size);
		}

		inline void operator =(const Stack<T, Q> &arr) {
			this->data = arr.data;
			this->size = arr.size;
		}

	public:

		Stack()
		{

		}

		Stack(const Stack<T> &arr)
		{
			this->data = arr.data;
			this->size = arr.size;
			this->current = arr.current;	
		}

		Stack(T *data, const Q &size)
		{
			this->data = data;
			this->size = size;
			this->current = size - static_cast<Q>(1);
		}

		Stack(const Q &size)
		{
			Resize(size);
		}

		Stack(std::initializer_list<T> init_list)
		{
			Q nsize = static_cast<Q>(init_list.size());
			Q next = static_cast<Q>(0);

			Resize(nsize);
			
			for (const T &t : init_list)
			{
				this->data[next] = t;
				this->next++;
				this->current++;
			}
		}	

		~Stack()
		{

		}
	};

}
