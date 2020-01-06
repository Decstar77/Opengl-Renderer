#pragma once
#include "Core.h"
#include "glew.h"
#include "BufferLayout.h"
#include <unordered_map>
namespace cm
{	
	//===============================================================//
	/*
		Renderbuffer/FrameBuffer are not so good and are subject to change when needed
		Cubemaps !

	*/
	//===============================================================//
	enum class VertexFlags : uint32
	{
		READ = GL_MAP_READ_BIT,
		WRITE = GL_MAP_WRITE_BIT,
		READ_WRITE = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT,
		READ_WRITE_COPY = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT

	};

	enum class BufferType
	{
		Array_buffer = GL_ARRAY_BUFFER,
		Index_buffer = GL_ELEMENT_ARRAY_BUFFER,
		Uniform_buffer = GL_UNIFORM_BUFFER,
		Frame_buffer = GL_FRAMEBUFFER,
		Shader_storage = GL_SHADER_STORAGE_BUFFER
	};

	enum class ShaderType : uint8
	{
		Rasterization = 1,
		Compute = 2
	};

	struct UniformBuffer
	{
		int32 binding_location = -1;
		uint32 object = 0;
		uint32 size_bytes = 0;
		BufferLayout lbo;
		BufferType type = BufferType::Uniform_buffer;
	};

	struct ShaderBuffer
	{
		int32 binding_location = -1;
		uint32 object = 0;
		uint32 size_bytes = 0;
		BufferLayout lbo;
		BufferType type = BufferType::Shader_storage;
	};

	struct VertexBuffer
	{
		uint32 object = 0;
		uint32 size_bytes = 0;
		BufferLayout lbo;
		BufferType type = BufferType::Array_buffer;
	};

	struct IndexBuffer
	{
		uint32 object;
		uint32 index_count;
		uint32 size_bytes;
		BufferType type = BufferType::Index_buffer;
	};
	
	struct VertexArray
	{
		uint32 object = 0;		
		DynaArray<VertexBuffer> vertex_buffers;
	};

	struct TextureConfig
	{
		uint32 type = GL_TEXTURE_2D;
		uint32 texture_format = GL_RGBA; // Or GL_RGBA32F for example // first
		uint32 pixel_format = GL_RGBA; //second
		uint32 data_type = GL_FLOAT; //GL_UNSIGNED_BYTE
		uint32 min_filter = GL_LINEAR;
		uint32 mag_filter = GL_LINEAR;
		uint32 wrap_s_mode = GL_REPEAT;
		uint32 wrap_t_mode = GL_REPEAT;
		uint32 wrap_r_mode = GL_REPEAT;
		uint32 width = 0;
		uint32 height = 0;
		std::string uniform_name = "";
	};

	struct CubeMapConfig
	{
		uint32 type = GL_TEXTURE_2D;
		uint32 texture_format = GL_RGBA; // Or GL_RGBA32F for example // first
		uint32 pixel_format = GL_RGBA; //second
		uint32 data_type = GL_FLOAT; //GL_UNSIGNED_BYTE
		uint32 min_filter = GL_LINEAR;
		uint32 mag_filter = GL_LINEAR;
		uint32 wrap_s_mode = GL_REPEAT;
		uint32 wrap_t_mode = GL_REPEAT;
		uint32 wrap_r_mode = GL_REPEAT;
		uint32 width = 0;
		uint32 height = 0;
		std::string uniform_name = "";
	};


	struct Texture
	{
		uint32 object = 0;
		TextureConfig config;
	};

	struct CubeMap
	{
		uint32 object = 0;
		CubeMapConfig config;
	};

	struct FrameBufferConfig
	{
		uint32 render_buffer_format = GL_DEPTH24_STENCIL8;
		uint32 render_buffer_attachment = GL_DEPTH_STENCIL_ATTACHMENT;
		
		uint32 buffer_attactment = GL_COLOR_ATTACHMENT0;
		uint32 buffer_draw = 36064; //Defualt apparently
		uint32 buffer_read = GL_COLOR_ATTACHMENT0;
	};

	struct FrameBuffer
	{
		BufferType type = BufferType::Frame_buffer;
		uint32 object = 0;
		uint32 render_object = 0;
		DynaArray<Texture> texture_attachments;		
		FrameBufferConfig config;
	};
		
	struct Shader
	{
		uint32 shader_program;
		ShaderType type;
		std::string name;
		std::unordered_map<std::string, uint32> uniform_cache;
	};



	template<typename T>
	T CreateBuffer(uint32 buffer_size_bytes, VertexFlags flags)
	{
		T buffer;
		uint32 object;
		uint32 type = static_cast<uint32>(buffer.type);
		glGenBuffers(1, &object);
		glBindBuffer(type, object);	

		glBufferStorage(type, buffer_size_bytes, NULL, static_cast<uint32>(flags));

		glBindBuffer(type, 0);

		buffer.object = object;
		buffer.size_bytes = buffer_size_bytes;

		return buffer;
		
		//glBufferSubData(buffer.type, 0, sizeof(mat4), projection.arr);	//<-- ubo
	}

	template<typename T>	
	void BindBuffer(const T &buffer)
	{
		Assert(buffer.object != 0);
		glBindBuffer(static_cast<uint32>(buffer.type), buffer.object);
	}
	template<>
	void BindBuffer<FrameBuffer>(const FrameBuffer &buffer)
	{
		Assert(buffer.object != 0);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.object);
	}
	
	template<typename T>
	void UnbindBuffer(const T &buffer)
	{
		uint32 type = static_cast<uint32>(buffer.type);
		glBindBuffer(type, 0);
	}
	template<>
	void UnbindBuffer<FrameBuffer>(const FrameBuffer &buffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	template<typename T>
	void FreeBuffer(T *buffer)
	{
		if (buffer->object != 0)
		{
			glDeleteBuffers(1, &buffer->object);
			buffer->object = 0;
		}
	}
	template<>// @RedoL: Not nice. Write FreeFrameBuffer Fucntion
	void FreeBuffer<FrameBuffer>(FrameBuffer *buffer)
	{
		if (buffer->object != 0)
		{
			glDeleteFramebuffers(1, &buffer->object);
			buffer->object = 0;
		}
	}

	template <typename Q, typename T>
	void ReadBufferData(Q buffer, DynaArray<T>* data, uint32 size_byte, uint32 offset_bytes)
	{
		Assert(buffer.object != 0);
		size_byte = size_byte == 0 ? (buffer.size_bytes - offset_bytes) : size_byte;
		Assert(offset_bytes + size_byte <= buffer.size_bytes);

		uint32 type = static_cast<uint32>(buffer.type);

		glBindBuffer(type, buffer.object);

		void *ptr = glMapBuffer(type, GL_READ_ONLY);
		ptr = static_cast<char*>(ptr) + offset_bytes;

		T *f_ptr = static_cast<T*>(ptr);

		data->insert(data->end(), &f_ptr[0], &f_ptr[size_byte / sizeof(T)]);

		glUnmapBuffer(type);
		glBindBuffer(type, 0);
	}

	template <typename Q, typename T>
	void WriteBufferData(Q buffer, const DynaArray<T>& data, uint32 offset_bytes)
	{
		Assert(buffer.object != 0);
		Assert(offset_bytes + data.size() * sizeof(T) <= buffer.size_bytes);

		uint32 type = static_cast<uint32>(buffer.type);

		glBindBuffer(type, buffer.object);

		void *ptr = glMapBuffer(type, GL_WRITE_ONLY);
		ptr = static_cast<char*>(ptr) + offset_bytes;

		const void * new_data = static_cast<const void *>(data.data());
		uint32 size = sizeof(T) * static_cast<uint32>(data.size());

		memcpy(ptr, new_data, size);

		glUnmapBuffer(type);
		glBindBuffer(type, 0);
	}

	template<typename T>
	void SetBufferBindingPoint(T &buffer) //TODO: Maybe pass in the binding_point
	{
		Assert(buffer.object != 0);
		Assert(buffer.binding_location != -1)
		uint32 type = static_cast<uint32>(buffer.type);		
		glBindBufferRange(type, buffer.binding_location, buffer.object, 0, buffer.size_bytes);//<-- ubo		
	}


	void BindVertexArray(const VertexArray &vao)
	{
		glBindVertexArray(vao.object);
	}

	void UnbindVertexArray()
	{
		glBindVertexArray(0);
	}
	   
	VertexArray CreateVertexArray(BufferLayout &lbo, VertexBuffer vbo /*= VertexBuffer()*/)
	{
		VertexArray vao;
		uint32 buffer_id;
		glGenVertexArrays(1, &buffer_id);
		glBindVertexArray(buffer_id);
		if (vbo.object != 0)
		{	
			BindBuffer(vbo);
			lbo.Reset();
			uint32 stride = lbo.GetStride();
			for (uint32 i = 0; i < lbo.GetComponentCount(); i++)
			{
				uint32 offset = lbo.GetCurrentOffSet();
				uint32 compCount = lbo.GetCurrentComponentAttribCount();
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, compCount, GL_FLOAT, GL_FALSE, stride, (void*)(offset));
				glVertexAttribDivisor(i, lbo.GetAttributeDivisor());
				lbo.Next();
			}
			vao.vertex_buffers.push_back(vbo);
			
		}
		glBindVertexArray(0);


		vao.object = buffer_id;
		return vao;
	}

	BufferLayout GetTotalVertexBufferLayout(const VertexArray &vao)
	{
		BufferLayout lbo;
		for (int32 i = 0; i < vao.vertex_buffers.size(); i++)
		{
			BufferLayout l = vao.vertex_buffers.at(i).lbo;
			lbo.Add(l);
		}
		return lbo;
	}

	void VertexArrayAddBuffer(VertexArray *vao, VertexBuffer &vbo, BufferLayout &added_lbo)
	{
		uint32 buffer_id = vao->object;
		if (buffer_id == 0)
		{
			glGenVertexArrays(1, &buffer_id);
		}

		glBindVertexArray(buffer_id);
		BindBuffer(vbo);
		uint32 stride = added_lbo.GetStride();
		BufferLayout lbo = GetTotalVertexBufferLayout(*vao);
		uint32 current_attrib = lbo.GetTotalAttributeCount();
		for (uint32 i = 0; i < added_lbo.GetComponentCount(); i++)
		{
			uint32 offset = added_lbo.GetCurrentOffSet();
			uint32 compCount = added_lbo.GetCurrentComponentAttribCount();

			if (added_lbo.GetCurrentShaderType() == ShaderDataType::Mat4)
			{
				uint32 smol_offset = offset;
				for (int32 ii = 0; ii < 4; ii++)
				{
					glEnableVertexAttribArray(current_attrib);
					glVertexAttribPointer(current_attrib, 4, GL_FLOAT, GL_FALSE, stride, (void*)(smol_offset));
					glVertexAttribDivisor(current_attrib, added_lbo.GetAttributeDivisor());
					current_attrib++;
					smol_offset += sizeof(Vec4);
				}
			}
			else if (added_lbo.GetCurrentShaderType() == ShaderDataType::Mat3)
			{
				for (int32 ii = 0; ii < 3; ii++)
				{
					glEnableVertexAttribArray(current_attrib);
					glVertexAttribPointer(current_attrib, 3, GL_FLOAT, GL_FALSE, stride, (void*)(offset));
					glVertexAttribDivisor(current_attrib, added_lbo.GetAttributeDivisor());
					current_attrib++;
				}
			}
			else
			{
				glEnableVertexAttribArray(current_attrib);
				glVertexAttribPointer(current_attrib, compCount, GL_FLOAT, GL_FALSE, stride, (void*)(offset));
				glVertexAttribDivisor(current_attrib, added_lbo.GetAttributeDivisor());
				current_attrib++;
			}
			added_lbo.Next();
		}
		glBindVertexArray(0);
		UnbindBuffer(vbo);

		vao->vertex_buffers.push_back(vbo);
	}

	void FreeVertexArray(VertexArray *vao, bool clear_vbos)
	{
		if (clear_vbos)
		{
			uint32 vbo_count = (uint32)vao->vertex_buffers.size(); 
			for (uint32 i = 0; i <  vbo_count; i++)
			{
				FreeBuffer(&vao->vertex_buffers[i]); // Free the GPU memory
			}			
			vao->vertex_buffers.clear();
		}
		else
		{
			glDeleteVertexArrays(1, &vao->object);
			vao->vertex_buffers.clear();
			vao->object = 0;
		}
	}

	//void VertexArray::CopyInto(VertexArray *vao) const
	//{
	//	Assert(buffer_id != 0);
	//	Assert(lbos.size() == vbos.size());
	//	vao->FreeAllBuffers();
	//	for (int32 i = 0; i < lbos.size(); i++)
	//	{
	//		VertexBuffer vbo; BufferLayout lbo;
	//		vbos.at(i).CopyInto(vbo, VertexMode::CM_READ_WRITE_COPY);
	//		lbo = lbos.at(i);
	//		vao->AddVertexBuffer(vbo, lbo);
	//	}
	//}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

#if _DEBUG
#define CHECK(type, loc) if (loc == -1) {std::cout << "ERROR::SHADER::UNIFORM::" << type << "LOCATION -> " << uniform_name  << " Shader name: " << shader.name << std::endl; return;}
#else
#define CHECK(type, loc)
#endif

	Shader CreateShader(std::string vertex_source, std::string fragment_source)
	{
		std::string vertex_code_str = vertex_source;
		std::string fragment_code_str = fragment_source;


		const char* vertex_code = vertex_code_str.c_str();
		const char * fShaderCode = fragment_code_str.c_str();

		// 2. compile shaders
		uint32 vertex, fragment, geometry;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertex_code, NULL);
		glCompileShader(vertex);
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		uint32 shader_program;
		// shader Program		
		shader_program = glCreateProgram();
		glAttachShader(shader_program, vertex);
		glAttachShader(shader_program, fragment);



		glLinkProgram(shader_program);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		Shader shader;
		shader.type = ShaderType::Compute;
		shader.name = "No name";
		shader.shader_program = shader_program;
		return shader;
	}


	Shader CreateComputeShader(std::string source)
	{
		const char *shader_ = source.c_str();

		uint32 ray_shader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(ray_shader, 1, &shader_, NULL);
		glCompileShader(ray_shader);
		uint32 shader_program = glCreateProgram();
		glAttachShader(shader_program, ray_shader);
		glLinkProgram(shader_program);
		glDeleteShader(ray_shader);
		Shader shader;
		shader.type = ShaderType::Rasterization;
		shader.name = source;
		shader.shader_program = shader_program;
		return shader;
	}


	Texture CreateTexture(const TextureConfig &config, const void* data)
	{
		uint32 text;

		glGenTextures(1, &text);
		glBindTexture(config.type, text);


		glTexParameteri(config.type, GL_TEXTURE_MIN_FILTER, config.min_filter);
		glTexParameteri(config.type, GL_TEXTURE_MAG_FILTER, config.mag_filter);
		glTexParameteri(config.type, GL_TEXTURE_WRAP_S, config.wrap_s_mode);
		glTexParameteri(config.type, GL_TEXTURE_WRAP_T, config.wrap_t_mode);
		glTexParameteri(config.type, GL_TEXTURE_WRAP_R, config.wrap_r_mode);

		glTexImage2D(config.type, 0, config.texture_format, config.width,
			config.height, 0, config.pixel_format, config.data_type, data == nullptr ? NULL : data);
		
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glBindTexture(config.type, 0);

		Texture t;
		t.config = config;
		t.object = text;
		return t;
	}

	template<typename T>
	void CreateCubeMap(CubeMap *map, const DynaArray<T> *data)
	{
		uint32 obj;
		glGenTextures(1, &obj);
		glBindTexture(GL_TEXTURE_CUBE_MAP, obj);
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP , GL_TEXTURE_MIN_FILTER, map->config.min_filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP , GL_TEXTURE_MAG_FILTER, map->config.mag_filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP , GL_TEXTURE_WRAP_S, map->config.wrap_s_mode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP , GL_TEXTURE_WRAP_T, map->config.wrap_t_mode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP , GL_TEXTURE_WRAP_R, map->config.wrap_r_mode);

		if (data == nullptr)
		{
			for (int32 i = 0; i < 6; i++)
			{				
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, map->config.texture_format,
					map->config.width, map->config.height, 0, map->config.pixel_format, map->config.data_type, 
					nullptr);
			}
		}
		else
		{
			for (int32 i = 0; i < 6; i++)
			{
				const T *a = data[i].data();
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, map->config.texture_format,
					map->config.width, map->config.height, 0, map->config.pixel_format, map->config.data_type, 
					a);
			}
		}
		

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		map->object = obj;
	}

	void FreeShader(Shader *shader)
	{
		glDeleteProgram(shader->shader_program);
		shader->shader_program = 0;
		shader->name = "NIks";
		shader->uniform_cache.clear();
	}

	void BindShader(const Shader &shader)
	{
		Assert(shader.shader_program != 0);
		glUseProgram(shader.shader_program);
	}

	uint32 GetUniformLocation(Shader *shader, const std::string &uniform_name)
	{
		if (shader->uniform_cache.find(uniform_name) != shader->uniform_cache.end())
		{
			return shader->uniform_cache[uniform_name];
		}
		else
		{
			uint32 loc = glGetUniformLocation(shader->shader_program, uniform_name.c_str());
			shader->uniform_cache[uniform_name] = loc;
			return loc;
		}
	}

	void ShaderSetInt32(Shader &shader, const std::string &uniform_name, int x)
	{
		uint32 loc = GetUniformLocation(&shader, uniform_name);
		CHECK("INT32", loc);
		glUniform1i(loc, x);
	}

	void ShaderSetFloat(Shader &shader, const std::string &uniform_name, float x)
	{
		uint32 loc = GetUniformLocation(&shader, uniform_name);
		CHECK("FLOAT", loc);
		glUniform1f(loc, x);
	}

	void ShaderSetVec3(Shader &shader, const std::string &uniform_name, float x, float y, float z)
	{
		uint32 loc = GetUniformLocation(&shader, uniform_name);
		CHECK("VEC3", loc);
		glUniform3f(loc, x, y, z);
	}

	void ShaderSetVec3(Shader &shader, const std::string &uniform_name, float* data)
	{
		uint32 loc = GetUniformLocation(&shader, uniform_name);
		CHECK("VEC3", loc);
		glUniform3fv(loc, 1, data);
	}

	void ShaderSetMat4(Shader &shader, const std::string &uniform_name, float* data)
	{
		uint32 loc = GetUniformLocation(&shader, uniform_name);
		CHECK("MAT4", loc);
		glUniformMatrix4fv(loc, 1, false, data);
	}

	void ShaderBindUniformBuffer(Shader &shader, uint32 binding_point, const std::string &uniform_name)
	{
		uint32 uniformBlockIndexRed = glGetUniformBlockIndex(shader.shader_program, uniform_name.c_str());
		glUniformBlockBinding(shader.shader_program, uniformBlockIndexRed, binding_point);
	}

	void ShaderBindTexture(Shader &shader, Texture texture, uint8 texture_slot, const std::string &uniform_name)
	{
		Assert(texture_slot < 4);
		uint32 loc = -1;
		std::string name = uniform_name;
		if (uniform_name == "")
		{
			name = texture.config.uniform_name;
		}
		loc = GetUniformLocation(&shader, name);
		Assert(loc != -1);
		glUniform1i(loc, texture_slot);
		glActiveTexture(GL_TEXTURE0 + texture_slot);
		glBindTexture(GL_TEXTURE_2D, texture.object);
	}


	void CreateFrameBuffer(FrameBuffer *fbo)
	{		
		Assert(fbo->object == 0);
		glGenFramebuffers(1, &fbo->object);	
	}	

	void AddFrameBufferTextureAttachtment(FrameBuffer *buffer, Texture texture)
	{
		Assert(buffer->object != 0);
		Assert(texture.config.width != 0);
		Assert(texture.config.height != 0);

		BindBuffer(*buffer);

		buffer->texture_attachments.push_back(texture);

		//if (config.texture.type == GL_TEXTURE_CUBE_MAP)
		//{
		//	glFramebufferTexture(GL_FRAMEBUFFER, config.buffer_attactment, tbos.back().GetId(), 0);
		//}


		glFramebufferTexture2D(GL_FRAMEBUFFER, buffer->config.buffer_attactment, 
			texture.config.type, texture.object, 0);
		
		glDrawBuffer(buffer->config.buffer_draw);
		glReadBuffer(buffer->config.buffer_read);

		UnbindBuffer(*buffer);
	}

	void AddFrameBufferRenderAttachtment(FrameBuffer *buffer, uint32 width, uint32 height)
	{
		Assert(buffer->object != 0);
		Assert(buffer->render_object == 0);
		Assert(buffer)
		uint32 rbo;

		BindBuffer(*buffer);
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		
		glRenderbufferStorage(GL_RENDERBUFFER, buffer->config.render_buffer_format,	width, height);
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, buffer->config.render_buffer_attachment, 
			GL_RENDERBUFFER, rbo);
		buffer->render_object = rbo;
		
		UnbindBuffer(*buffer);
	}

	bool CheckFrameBuffer(FrameBuffer buffer)
	{
		bool res = true;
		BindBuffer(buffer);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");			
			res = false;
		}

		UnbindBuffer(buffer);
		return res;
	}



}


