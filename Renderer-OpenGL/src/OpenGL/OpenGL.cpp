#include "OpenGL.h"

namespace cm
{
	cm::GLMesh StandardMeshes::quad;

	cm::GLMesh StandardMeshes::plane;

	cm::GLMesh StandardMeshes::cube;

	cm::GLMesh StandardMeshes::sphere;

	cm::GLMesh StandardMeshes::cone;

	uint32 OpenGlState::current_viewport_width;

	uint32 OpenGlState::current_viewport_height;

	uint32 OpenGlState::window_width;

	uint32 OpenGlState::window_height;

	cm::Texture StandardTextures::zero_texture;

	cm::Texture StandardTextures::one_texture;

	cm::Texture StandardTextures::invalid_texture;

	bool StandardTextures::created = false;

	bool StandardMeshes::created = false;

	void StandardTextures::Initilize()
	{
		Assert(!created)

		zero_texture.config.texture_format = GL_RGBA32F;
		zero_texture.config.pixel_format = GL_RGBA;
		zero_texture.config.wrap_t_mode = GL_REPEAT;
		zero_texture.config.wrap_s_mode = GL_REPEAT;
		zero_texture.config.min_filter = GL_LINEAR;
		zero_texture.config.mag_filter = GL_LINEAR;
		zero_texture.config.width = 2;
		zero_texture.config.height = 2;
		real32 zero_data[] = 
		{	0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 0.0 
		};
		
		CreateTexture(&zero_texture, zero_data);

		one_texture.config.texture_format = GL_RGBA32F;
		one_texture.config.pixel_format = GL_RGBA;
		one_texture.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		one_texture.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		one_texture.config.min_filter = GL_LINEAR;
		one_texture.config.mag_filter = GL_LINEAR;
		one_texture.config.width = 2;
		one_texture.config.height = 2;
		real32 one_data[] = { 
			1.0, 1.0, 1.0, 1.0,
			1.0, 1.0, 1.0, 1.0,
			1.0, 1.0, 1.0, 1.0,
			1.0, 1.0, 1.0, 1.0 
		};
		
		CreateTexture(&one_texture, (void*)&one_data[0]);


		invalid_texture.config.texture_format = GL_RGBA32F;
		invalid_texture.config.pixel_format = GL_RGBA;
		invalid_texture.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		invalid_texture.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		invalid_texture.config.min_filter = GL_LINEAR;
		invalid_texture.config.mag_filter = GL_LINEAR;
		invalid_texture.config.width = 2;
		invalid_texture.config.height = 2;
		real32 invalid_texture_data[] = {
			1.0, 0.0, 1.0, 1.0,
			1.0, 0.0, 1.0, 1.0,
			1.0, 0.0, 1.0, 1.0,
			1.0, 0.0, 1.0, 1.0
		};

		CreateTexture(&invalid_texture, (void*)&invalid_texture_data[0]);

		
		created = true;
	}

	void StandardMeshes::Initilize()
	{
		Assert(!created);

		real32 cube_vertex_data[] =
		{
			1.000000, 1.000000, -1.000000, 0.000000, 1.000000, 0.000000, 0.625000, 0.500000, -1.000000,
			1.000000, -1.000000, 0.000000, 1.000000, 0.000000, 0.875000, 0.500000, -1.000000, 1.000000,
			1.000000, 0.000000, 1.000000, 0.000000, 0.875000, 0.250000, 1.000000, 1.000000, 1.000000, 0.000000,
			1.000000, 0.000000, 0.625000, 0.250000, 1.000000, -1.000000, 1.000000, 0.000000, 0.000000,
			1.000000, 0.375000, 0.250000, 1.000000, 1.000000, 1.000000, 0.000000, 0.000000, 1.000000,
			0.625000, 0.250000, -1.000000, 1.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.625000,
			0.000000, -1.000000, -1.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.375000, 0.000000,
			-1.000000, -1.000000, 1.000000, -1.000000, 0.000000, 0.000000, 0.375000, 1.000000, -1.000000,
			1.000000, 1.000000, -1.000000, 0.000000, 0.000000, 0.625000, 1.000000, -1.000000, 1.000000,
			-1.000000, -1.000000, 0.000000, 0.000000, 0.625000, 0.750000, -1.000000, -1.000000, -1.000000,
			-1.000000, 0.000000, 0.000000, 0.375000, 0.750000, -1.000000, -1.000000, -1.000000, 0.000000,
			-1.000000, 0.000000, 0.125000, 0.500000, 1.000000, -1.000000, -1.000000, 0.000000, -1.000000,
			0.000000, 0.375000, 0.500000, 1.000000, -1.000000, 1.000000, 0.000000, -1.000000, 0.000000,
			0.375000, 0.250000, -1.000000, -1.000000, 1.000000, 0.000000, -1.000000, 0.000000, 0.125000,
			0.250000, 1.000000, -1.000000, -1.000000, 1.000000, 0.000000, 0.000000, 0.375000, 0.500000,
			1.000000, 1.000000, -1.000000, 1.000000, 0.000000, 0.000000, 0.625000, 0.500000, 1.000000,
			1.000000, 1.000000, 1.000000, 0.000000, 0.000000, 0.625000, 0.250000, 1.000000, -1.000000,
			1.000000, 1.000000, 0.000000, 0.000000, 0.375000, 0.250000, -1.000000, -1.000000, -1.000000,
			0.000000, 0.000000, -1.000000, 0.375000, 0.750000, -1.000000, 1.000000, -1.000000, 0.000000,
			0.000000, -1.000000, 0.625000, 0.750000, 1.000000, 1.000000, -1.000000, 0.000000, 0.000000,
			-1.000000, 0.625000, 0.500000, 1.000000, -1.000000, -1.000000, 0.000000, 0.000000, -1.000000,
			0.375000, 0.500000
		};
		
		uint32 cube_index_data[] =
		{
			0, 1, 2, 0, 2, 3, 4, 5, 6, 4,
			6, 7, 8, 9, 10, 8, 10, 11, 12, 13,
			14, 12, 14, 15, 16, 17, 18, 16, 18,
			19, 20, 21, 22, 20, 22, 23,
		};

		real32 plane_vertex_data[] =
		{
			1.00000000,- 1.00000000,0.000000000,0.000000000,0.000000000,1.00000000,1.00000000,1.00000000,
			- 1.00000000,1.00000000,0.000000000,0.000000000,0.000000000,1.00000000,0.000000000,0.000000000,
			- 1.00000000,- 1.00000000,0.000000000,0.000000000,0.000000000,1.00000000,0.000000000,1.00000000,
			1.00000000,1.00000000,0.000000000,0.000000000,0.000000000,1.00000000,1.00000000,0.000000000
		};
		uint32 plane_index_data[] =
		{
			0,1,2,0,3,1
		};
		
		real32 quad_vertex_data[] =
		{
			-1.00000000,-1.00000000, 0.000000000, 0.000000000,-0.000000000, 1.00000000, 0.000000000, 0.000000000, 1.00000000,
			-1.00000000,0.000000000, 0.000000000,-0.000000000, 1.00000000,1.00000000,0.000000000,-1.00000000,1.00000000,0.000000000,
			0.000000000,-0.000000000,1.00000000,0.000000000,1.00000000,-1.00000000,1.00000000,0.000000000,-0.000000000,0.000000000,
			1.00000000,0.000000000,1.00000000,1.00000000,-1.00000000,0.000000000,-0.000000000,0.000000000,1.00000000,1.00000000,
			0.000000000,1.00000000,1.00000000,0.000000000,-0.000000000,0.000000000,1.00000000,1.00000000,1.00000000
		};

		uint32 quad_index_data[] =
		{
			0, 1, 2, 3, 4, 5
		};

		CreateGLMesh(&plane, PNT_VBO_LAYOUT, plane_vertex_data, 32 * sizeof(real32), plane_index_data, 6 * sizeof(uint32));
		CreateGLMesh(&quad, PNT_VBO_LAYOUT, quad_vertex_data, 48 * sizeof(real32), quad_index_data, 6 * sizeof(uint32));
		CreateGLMesh(&cube, PNT_VBO_LAYOUT, cube_vertex_data, 192 * sizeof(real32), cube_index_data, 36 * sizeof(uint32));
		created = true;
	}

	uint32 GetShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}
		Assert(0);
		return 0;
	}

	uint32 GetShaderDataTypeComponentCount(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 1;
		case ShaderDataType::Float2:   return 1 * 2;
		case ShaderDataType::Float3:   return 1 * 3;
		case ShaderDataType::Float4:   return 1 * 4;
		case ShaderDataType::Mat3:     return 1 * 3 * 3;
		case ShaderDataType::Mat4:     return 1 * 4 * 4;
		case ShaderDataType::Int:      return 1;
		case ShaderDataType::Int2:     return 1 * 2;
		case ShaderDataType::Int3:     return 1 * 3;
		case ShaderDataType::Int4:     return 1 * 4;
		case ShaderDataType::Bool:     return 1;
		}
		Assert(0);
		return 0;
	}

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

	void CreateVertexBuffer(VertexBuffer *vbo)
	{
		Assert(vbo->object == 0);
		Assert(vbo->size_bytes > 0);
		
		uint32 object;
		uint32 type = static_cast<uint32>(BufferType::Array_buffer);

		glGenBuffers(1, &object);
		glBindBuffer(type, object);

		glBufferStorage(type, vbo->size_bytes, NULL, static_cast<uint32>(vbo->flags));

		glBindBuffer(type, 0);

		vbo->object = object;
	}

	void BindVertexBuffer(const VertexBuffer &vbo)
	{
		Assert(vbo.object != 0);
		uint32 type = static_cast<uint32>(BufferType::Array_buffer);
		glBindBuffer(type, vbo.object);
	}

	void UnbindVertexBuffer()
	{
		uint32 type = static_cast<uint32>(BufferType::Array_buffer);
		glBindBuffer(type, 0);
	}

	void FreeVerteBuffer(VertexBuffer *vbo)
	{
		if (vbo->object != 0)
		{
			glDeleteBuffers(1, &vbo->object);
			vbo->object = 0;
		}
		else
		{
			LOGC("WARNING::FREEING_VERTEX_BUFFER:: -> buffer was already freed");
		}
	}

	void CreateIndexBuffer(IndexBuffer *ibo)
	{
		Assert(ibo->object == 0);
		Assert(ibo->size_bytes > 0);
		uint32 object;
		uint32 type = static_cast<uint32>(BufferType::Index_buffer);
		glGenBuffers(1, &object);
		glBindBuffer(type, object);

		glBufferStorage(type, ibo->size_bytes, NULL, static_cast<uint32>(ibo->flags));

		glBindBuffer(type, 0);

		ibo->object = object;
	}

	void BindIndexBuffer(const IndexBuffer &ibo)
	{
		Assert(ibo.object != 0);
		uint32 type = static_cast<uint32>(BufferType::Index_buffer);
		glBindBuffer(type, ibo.object);
	}

	void UnbindIndexBuffer()
	{
		uint32 type = static_cast<uint32>(BufferType::Index_buffer);
		glBindBuffer(type, 0);
	}

	void FreeIndexBuffer(IndexBuffer *ibo)
	{
		if (ibo->object != 0)
		{
			glDeleteBuffers(1, &ibo->object);
			ibo->object = 0;
		}
		else
		{
			LOGC("WARNING::FREEING_INDEX_BUFFER:: -> buffer was already freed");
		}
	}

	void CreateUniformBuffer(UniformBuffer *buffer)
	{		
		Assert(buffer->object == 0);
		uint32 buff;
		glGenBuffers(1, &buff);
		glBindBuffer(GL_UNIFORM_BUFFER, buff);
		
		uint32 size_bytes = buffer->size_bytes;

		glBufferStorage(GL_UNIFORM_BUFFER, size_bytes, NULL, static_cast<uint32>(VertexFlags::READ_WRITE));

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		buffer->object = buff;
	}

	void UniformBufferSetBindPoint(const UniformBuffer &ubo, uint32 binding_point)
	{
		Assert(ubo.object != 0);
		Assert(ubo.size_bytes != 0);
		Assert(binding_point >= 0);			
		glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, ubo.object, 0, ubo.size_bytes);		
	}


	void BindVertexArray(const VertexArray &vao)
	{
		glBindVertexArray(vao.object);
	}

	void UnbindVertexArray()
	{
		glBindVertexArray(0);
	}

	void CreateVertexArray(VertexArray *vao)
	{
		uint32 buffer_id;
		glGenVertexArrays(1, &buffer_id);
		glBindVertexArray(buffer_id);
		uint32 vbo_count = (uint32)vao->vertex_buffers.size();

		uint32 attrib_counter = 0;
		for (uint32 i = 0; i < vbo_count; i++)
		{
			VertexBuffer current_vbo = vao->vertex_buffers[i];
			LayoutBuffer lbo = current_vbo.lbo;
			BindVertexBuffer(current_vbo);
			current_vbo.lbo.Reset();
			uint32 stride = lbo.GetStride();
			for (uint32 i = 0; i < lbo.GetComponentCount(); i++)
			{
				uint64 offset = lbo.GetCurrentOffSet();
				uint32 compCount = lbo.GetCurrentComponentAttribCount();
				if (lbo.GetCurrentShaderType() == ShaderDataType::Mat4)
				{
					uint64 smol_offset = offset;
					for (int32 ii = 0; ii < 4; ii++)
					{
						glEnableVertexAttribArray(attrib_counter);
						glVertexAttribPointer(attrib_counter, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(smol_offset));
						glVertexAttribDivisor(attrib_counter, lbo.GetAttributeDivisor());
						attrib_counter++;
						smol_offset += sizeof(Vec4);
					}
				}
				else if (lbo.GetCurrentShaderType() == ShaderDataType::Mat3)
				{
					for (int32 ii = 0; ii < 3; ii++)
					{
						glEnableVertexAttribArray(attrib_counter);
						glVertexAttribPointer(attrib_counter, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offset));
						glVertexAttribDivisor(attrib_counter, lbo.GetAttributeDivisor());
						attrib_counter++;
					}
				}
				else
				{
					glEnableVertexAttribArray(attrib_counter);
					glVertexAttribPointer(attrib_counter, compCount, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offset));
					glVertexAttribDivisor(attrib_counter, lbo.GetAttributeDivisor());
					attrib_counter++;
				}
				lbo.Next();
			}
		}
		glBindVertexArray(0);
		vao->object = buffer_id;
	}

	void FreeVertexArray(VertexArray *vao, bool clear_vbos)
	{
		if (clear_vbos)
		{
			// Free the GPU memory
			uint32 vbo_count = static_cast<uint32>(vao->vertex_buffers.size());
			for (uint32 i = 0; i < vbo_count; i++)
			{
				FreeVerteBuffer(&vao->vertex_buffers[i]);
			}
			glDeleteVertexArrays(1, &vao->object); 
			
			// Free the local memory
			vao->vertex_buffers.clear();
			vao->object = 0;
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


#if _DEBUG
#define CHECK(type, loc) if (loc == -1) {std::cout << "ERROR::SHADER::UNIFORM::" << type << "LOCATION -> " << uniform_name  << " Shader name: " << shader->name << std::endl; return;}
#else
#define CHECK(type, loc)
#endif

	Shader CreateShader(String vertex_source, String fragment_source)
	{
		String vertex_code_str = vertex_source;
		String fragment_code_str = fragment_source;


		const char* vertex_code = vertex_code_str.c_str();
		const char * fShaderCode = fragment_code_str.c_str();

		// 2. compile shaders
		uint32 vertex;
		uint32 fragment;
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
		shader.type = ShaderType::Rasterization;
		shader.name = "No name";
		shader.shader_program = shader_program;
		return shader;
	}


	Shader CreateShader(Shader *shader)
	{			   
		Assert(shader->config.src_geom == ""); // REASON: We don't have geom support
		const char* vertex_code = shader->config.src_vert.c_str();
		const char * fShaderCode = shader->config.src_frag.c_str();

		uint32 vertex, fragment;
		
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertex_code, NULL);
		glCompileShader(vertex);
		
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		uint32 shader_program;
		
		shader_program = glCreateProgram();
		glAttachShader(shader_program, vertex);
		glAttachShader(shader_program, fragment);
		

		glLinkProgram(shader_program);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		shader->type = ShaderType::Compute;
		shader->shader_program = shader_program;

		return {};
	}

	Shader CreateComputeShader(String source)
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


	void CreateTexture(Texture *texture, const void* data)
	{
		uint32 text;
		Assert(texture->object == 0);
		Assert(texture->config.width != 0);
		Assert(texture->config.height != 0);

		glGenTextures(1, &text);
		glBindTexture(texture->config.type, text);

		glTexParameteri(texture->config.type, GL_TEXTURE_MIN_FILTER, texture->config.min_filter);
		glTexParameteri(texture->config.type, GL_TEXTURE_MAG_FILTER, texture->config.mag_filter);
		glTexParameteri(texture->config.type, GL_TEXTURE_WRAP_S, texture->config.wrap_s_mode);
		glTexParameteri(texture->config.type, GL_TEXTURE_WRAP_T, texture->config.wrap_t_mode);
		glTexParameteri(texture->config.type, GL_TEXTURE_WRAP_R, texture->config.wrap_r_mode);

		glTexImage2D(texture->config.type, 0, texture->config.texture_format, texture->config.width,
			texture->config.height, 0, texture->config.pixel_format, texture->config.data_type,
			data == nullptr ? NULL : data);
		
		if (texture->config.mipmaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(texture->config.type, 0);
		texture->object = text;
	}

	void BindTexture(Texture *texture)
	{
		Assert(texture->object != 0);
		glBindTexture(texture->config.type, texture->object);
	}

	void BindTexture(const Texture &texture)
	{
		Assert(texture.object != 0);
		glBindTexture(texture.config.type, texture.object);
	}

	void UnbindTexure()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void CopyTexture(Texture *src, Texture *dst)
	{
		Assert(src != nullptr);
		Assert(src->object != 0);
		Assert(dst != nullptr);
		Assert(dst->object != 0);

		Assert(src->config.width == dst->config.width);
		Assert(src->config.height == dst->config.height);

		glCopyImageSubData(src->object, GL_TEXTURE_2D, 0, 0, 0, 0,
						   dst->object, GL_TEXTURE_2D, 0, 0, 0, 0, 
						   dst->config.width, dst->config.height, 1);
	}

	void TextureSetBorder(Texture *texture, float *border_colour)
	{
		BindTexture(texture);		
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour);
		UnbindTexure();
	}

	void FreeTexture(Texture *texture)
	{
		if (texture->object != 0)
		{
			glDeleteTextures(1, &texture->object);
			texture->object = 0;
		}		
	}

	void CreateCubeMap(CubeMap *cube_map, const void **data)
	{
		Assert(cube_map->object == 0);
		Assert(cube_map->config.width != 0);
		Assert(cube_map->config.height != 0);
		
		uint32 obj;
		glGenTextures(1, &obj);
		glBindTexture(GL_TEXTURE_CUBE_MAP, obj);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, cube_map->config.min_filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, cube_map->config.mag_filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, cube_map->config.wrap_s_mode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, cube_map->config.wrap_t_mode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, cube_map->config.wrap_r_mode);

		if (data == nullptr)
		{
			for (int32 i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, cube_map->config.texture_format,
					cube_map->config.width, cube_map->config.height, 0, cube_map->config.pixel_format, 
					cube_map->config.data_type,	nullptr);
			}
		}
		else
		{
			for (int32 i = 0; i < 6; i++)
			{
				const void *a = data[i];
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, cube_map->config.texture_format,
					cube_map->config.width, cube_map->config.height, 0, cube_map->config.pixel_format,
					cube_map->config.data_type,	a);
			}
		}

		if (cube_map->config.mipmaps)
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		cube_map->object = obj;
	}

	void CreateCubeMapFrom6(CubeMap *cubemap, std::vector<Texture> textures)
	{
		Assert(0);
		//DynaArray<uint8> cubemap_data[6] = {};
		//cubemap->config.type = GL_TEXTURE_CUBE_MAP;
		//cubemap->config.texture_format = GL_RGB;
		//cubemap->config.pixel_format = GL_RGB;
		//cubemap->config.data_type = GL_UNSIGNED_BYTE;

		//for (int32 i = 0; i < 6; i++)
		//{
		//	FileDataTexture<uint8> texture_data = LoadTexture(cubemap_faces_directories[i]);
		//	cubemap->config.width = texture_data.width;
		//	cubemap->config.height = texture_data.height;
		//	cubemap_data[i] = texture_data.data;
		//}
		//CreateCubeMap(cubemap, cubemap_data);
	}

	void FreeShader(Shader *shader)
	{
		glDeleteProgram(shader->shader_program);
		shader->shader_program = 0;
		shader->name = "Niks";
		shader->uniform_cache.clear();
	}

	void BindShader(const Shader &shader)
	{
		Assert(shader.shader_program != 0);
		glUseProgram(shader.shader_program);
	}

	void UnbindShader()
	{
		glUseProgram(0);
	}

	uint32 GetUniformLocation(Shader *shader, const String &uniform_name)
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

	void ShaderSetBool(Shader *shader, const String &uniform_name, const bool &b)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("INT32", loc);
		glUniform1i(loc, b);
	}

	void ShaderSetInt32(Shader *shader, const String &uniform_name, int x)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("INT32", loc);
		glUniform1i(loc, x);
	}

	void ShaderSetFloat(Shader *shader, const String &uniform_name, float x)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("FLOAT", loc);
		glUniform1f(loc, x);
	}

	void ShaderSetVec3(Shader *shader, const String &uniform_name, float x, float y, float z)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("VEC3", loc);
		glUniform3f(loc, x, y, z);
	}

	void ShaderSetVec3(Shader *shader, const String &uniform_name, float* data)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("VEC3", loc);
		glUniform3fv(loc, 1, data);
	}

	void ShaderSetMat4(Shader *shader, const String &uniform_name, float* data)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("MAT4", loc);
		glUniformMatrix4fv(loc, 1, false, data);
	}

	void ShaderBindUniformBuffer(const Shader &shader, uint32 binding_point, const String &uniform_name)
	{
		uint32 uniformBlockIndexRed = glGetUniformBlockIndex(shader.shader_program, uniform_name.c_str());
		glUniformBlockBinding(shader.shader_program, uniformBlockIndexRed, binding_point);
	}

	void ShaderBindTexture(Shader &shader, const Texture &texture, uint32 texture_slot, const String &uniform_name)
	{
		uint32 loc = -1;
		String name = uniform_name;
		loc = GetUniformLocation(&shader, name);
		if (loc == -1)
		{
			String ss = "ERROR::SHADER::TEXTURE: " + shader.name + " " + uniform_name;
			LOGC(ss.c_str());
		}
		else
		{			
			glUniform1i(loc, texture_slot);
			glActiveTexture(GL_TEXTURE0 + texture_slot);
			glBindTexture(GL_TEXTURE_2D, texture.object);
		}		
	}
	
	void ShaderBindCubeMap(Shader *shader, const CubeMap &cube_map, uint32 texture_slot, const String &uniform_name)
	{
		uint32 loc = -1;
		String name = uniform_name;
		if (uniform_name == "")
		{
			name = cube_map.config.uniform_name;
		}

		loc = GetUniformLocation(shader, name);
		if (loc == -1)
		{
			String ss = "ERROR::SHADER::TEXTURE: " + shader->name + " " + uniform_name;
			LOGC(ss.c_str());
		}
		else
		{
			glUniform1i(loc, texture_slot);
			glActiveTexture(GL_TEXTURE0 + texture_slot);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map.object);
		}
	}

	void BindFrameBuffer(const FrameBuffer &fbo)
	{
		Assert(fbo.object != 0) // @NOTE: Open-GL reserves 0 for the defualt framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo.object);
	}

	void UnbindFrameBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FreeFrameBuffer(FrameBuffer *fbo, bool attachments)
	{		
		if (fbo->object != 0)
		{
			glDeleteFramebuffers(1, &fbo->object);
			fbo->object = 0;
			if (attachments)
			{
				Texture *texture;
				texture = &fbo->colour0_texture_attachment;
				FreeTexture(texture);

				texture = &fbo->colour1_texture_attachment;
				FreeTexture(texture);

				texture = &fbo->colour2_texture_attachment;
				FreeTexture(texture);

				texture = &fbo->depth_texture_attachment;
				FreeTexture(texture);

				DepthStencilBuffer *render_buffer = &fbo->depthstencil_attchment;
				FreeDepthStencilBuffer(render_buffer);
			}			
		}
	}

	void CreateDepthStencilBuffer(DepthStencilBuffer *rb)
	{
		Assert(rb->object == 0);
		Assert(rb->width != 0);
		Assert(rb->height != 0);

		uint32 rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);

		glRenderbufferStorage(GL_RENDERBUFFER, rb->render_buffer_format,
			rb->width, rb->height);

		rb->object = rbo;

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void FreeDepthStencilBuffer(DepthStencilBuffer *rb)
	{
		if (rb->object != 0)
		{
			glDeleteRenderbuffers(1, &rb->object);
			rb->object = 0;
		}
	}

	void CreateFrameBuffer(FrameBuffer *fbo)
	{
		Assert(fbo->object == 0);
		glGenFramebuffers(1, &fbo->object);
	}

	void FrameBufferCopyDepth(const FrameBuffer &src, FrameBuffer *dst)
	{
		Assert(src.object != 0);
		Assert(src.depthstencil_attchment.object != 0);
		Assert(dst->object != 0);
		Assert(dst->depthstencil_attchment.object != 0);
		Assert(src.depthstencil_attchment.width == dst->depthstencil_attchment.width);
		Assert(src.depthstencil_attchment.height == dst->depthstencil_attchment.height);

		// @NOTE The read buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, src.object);

		// @NOTE: The write buffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->object); 

		glBlitFramebuffer(0, 0, src.depthstencil_attchment.width, src.depthstencil_attchment.height,			
						  0, 0, dst->depthstencil_attchment.width, dst->depthstencil_attchment.height,
						 GL_DEPTH_BUFFER_BIT, GL_NEAREST
		);
		
		
	}

	void FrameBufferBindColourAttachtments(FrameBuffer *buffer)
	{
		Assert(buffer->object != 0);
		BindFrameBuffer(*buffer);
		

		uint32 count = 0;
		uint32 attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
								GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

		Texture *texture = nullptr;
		
		texture = &buffer->colour0_texture_attachment;
		if (texture->object != 0)
		{
			uint32 type = texture->config.type;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, type,
				texture->object, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			count++;
		}		
		
		texture = &buffer->colour1_texture_attachment;
		if (texture->object != 0)
		{
			uint32 type = texture->config.type;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, type,
				texture->object, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT1);
			count++;
		}

		texture = &buffer->colour2_texture_attachment;
		if (texture->object != 0)
		{
			uint32 type = texture->config.type;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, type,
				texture->object, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT2);
			count++;
		}
		
		glDrawBuffers(count, attachments);
		UnbindFrameBuffer();
	}


	void FrameBufferBindDepthAttachment(FrameBuffer *buffer)
	{
		Assert(buffer->object != 0);
		Assert(buffer->depth_texture_attachment.object != 0);
		BindFrameBuffer(*buffer);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,	buffer->depth_texture_attachment.object, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		UnbindFrameBuffer();
	}

	void FrameBufferBindRenderAttachtment(FrameBuffer *buffer)
	{
		Assert(buffer->object != 0);
		Assert(buffer->depthstencil_attchment.object != 0);
		Assert(buffer->depthstencil_attchment.width != 0);
		Assert(buffer->depthstencil_attchment.height!= 0);
		
		BindFrameBuffer(*buffer);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, buffer->depthstencil_attchment.render_buffer_attachment_type,
			GL_RENDERBUFFER, buffer->depthstencil_attchment.object);
		
		UnbindFrameBuffer();
	}

	bool CheckFrameBuffer(const FrameBuffer &buffer)
	{
		bool res = true;
		BindFrameBuffer(buffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			LOGC("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
			res = false;
		}

		UnbindFrameBuffer();
		return res;
	}

	void CreateBatch(Batch *batch, const VertexBuffer &vbo_to_batch, const IndexBuffer &ibo_of_vbo)
	{
		Assert(batch->vao.object == 0);
		Assert(batch->ibo.object == 0);
		Assert(batch->transforms.size() != 0);
		Assert(ibo_of_vbo.object != 0);
		Assert(vbo_to_batch.object != 0);

		//TODO: This is slow because we read and write. Rather we just copy from one to the other
		uint32 mat_count = (uint32)batch->transforms.size();
		VertexBuffer mat_vbo;
		mat_vbo.lbo = LayoutBuffer({ ShaderDataType::Mat4 });
		mat_vbo.lbo.SetAttributeDivisor(1);
		mat_vbo.size_bytes = mat_count * sizeof(Mat4);
		mat_vbo.flags = VertexFlags::READ_WRITE;
		CreateVertexBuffer(&mat_vbo);
		WriteBufferData(&mat_vbo, batch->transforms, 0);

		VertexBuffer current_mesh_vbo = vbo_to_batch;
		std::vector<float> current_mesh_data;
		ReadBufferData(current_mesh_vbo, &current_mesh_data, current_mesh_vbo.size_bytes, 0);

		VertexBuffer mesh_vbo;
		mesh_vbo.lbo = current_mesh_vbo.lbo;
		mesh_vbo.size_bytes = current_mesh_vbo.size_bytes;
		mesh_vbo.flags = VertexFlags::READ_WRITE;
		CreateVertexBuffer(&mesh_vbo);
		WriteBufferData(&mesh_vbo, current_mesh_data, 0);

		IndexBuffer ibo;
		ibo.size_bytes = ibo_of_vbo.size_bytes;
		ibo.flags = VertexFlags::READ_WRITE;
		CreateIndexBuffer(&ibo);
		ibo.index_count = ibo_of_vbo.index_count;

		std::vector<uint32> index_data;
		ReadBufferData(ibo_of_vbo, &index_data, ibo_of_vbo.size_bytes, 0);

		WriteBufferData(&ibo, index_data, 0);

		VertexArray temp_vao;
		temp_vao.vertex_buffers.push_back(mesh_vbo);
		temp_vao.vertex_buffers.push_back(mat_vbo);

		CreateVertexArray(&temp_vao);


		batch->vao = temp_vao;
		batch->ibo = ibo;
	}

	void FreeBatch(Batch *batch)
	{
		FreeIndexBuffer(&batch->ibo);
		FreeVertexArray(&batch->vao, true);
		batch->transforms.clear();
	}


	void RenderBatch(const Shader &shader, const Batch &batch)
	{
		uint32 amount = (uint32)batch.transforms.size();
		//@NOTE: Test to make sure we haven't added anything to the transforms without recreating batch
		Assert(batch.vao.vertex_buffers[1].size_bytes / sizeof(Mat4) == amount);


		BindShader(shader);
		BindVertexArray(batch.vao);
		BindIndexBuffer(batch.ibo);

		glDrawElementsInstanced(GL_TRIANGLES, batch.ibo.index_count,
			GL_UNSIGNED_INT, 0, amount);

		UnbindIndexBuffer();
		UnbindVertexArray();
	}

	void RenderMesh(const Shader &shader, const GLMesh &mesh)
	{
		BindShader(shader);

		BindVertexArray(mesh.vao);
		BindIndexBuffer(mesh.ibo);

		glDrawElements(GL_TRIANGLES, mesh.ibo.index_count, GL_UNSIGNED_INT, 0);

		UnbindIndexBuffer();
		UnbindVertexArray();
	}

	void RenderMesh(const Shader &shader, const GLMesh *mesh)
	{
		BindShader(shader);

		BindVertexArray(mesh->vao);
		BindIndexBuffer(mesh->ibo);

		glDrawElements(GL_TRIANGLES, mesh->ibo.index_count, GL_UNSIGNED_INT, 0);

		UnbindIndexBuffer();
		UnbindVertexArray();
	}

	void GetOpenglStatistics(OpenGLStatistics *stats)
	{
		stats->vendor = String((const char*)glGetString(GL_VENDOR));
		stats->renderer = String((const char*)glGetString(GL_RENDERER));
		stats->version = String((const char*)glGetString(GL_VERSION));
		stats->shading_lang = String((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &stats->work_grp_cnt[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &stats->work_grp_cnt[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &stats->work_grp_cnt[2]);

		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &stats->work_grp_size[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &stats->work_grp_size[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &stats->work_grp_size[2]);

		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &stats->work_grp_inv);		
	}

	void PrintOpenglStatistics(const OpenGLStatistics &stats)
	{		
		printf("/*************OPENGL_STATS*************/ \n");
		printf("General Stats: \n");
		printf("Vendor: ");
		printf(stats.vendor.c_str()); printf("\n");
		printf("Renderer: ");
		printf(stats.renderer.c_str()); printf("\n");
		printf("Version: ");
		printf(stats.version.c_str()); printf("\n");
		printf("Shader_lang: ");
		printf(stats.shading_lang.c_str()); printf("\n");
		
		printf("Compute Stats: \n");
		printf("max global (total) work group size x:%i y:%i z:%i\n", 
			stats.work_grp_cnt[0], stats.work_grp_cnt[1], stats.work_grp_cnt[2]);
		printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
			stats.work_grp_size[0], stats.work_grp_size[1], stats.work_grp_size[2]);
		printf("max local work group invocations %i\n", stats.work_grp_inv);
		printf("/*************OPENGL_STATS*************/ \n");
	}

	void CreateGLMesh(GLMesh *mesh, LayoutBuffer lbo, void *vertex_data, uint32 vertex_size_bytes, void *index_data, uint32 index_size_bytes)
	{
		Assert(mesh->ibo.object == 0);
		Assert(mesh->vao.object == 0);
		Assert(mesh->vao.vertex_buffers.size() == 0);

		VertexBuffer vbo;
		vbo.lbo = lbo;
		vbo.size_bytes = vertex_size_bytes;
		vbo.flags = VertexFlags::READ_WRITE; 

		CreateVertexBuffer(&vbo);
		WriteBufferData(&vbo, vertex_data, vertex_size_bytes, 0);

		// @HACK: We just assume that its of size uint32 becasue we have no way to dertermine the actauly count.
		//		: Could use templates to solve this. butt eeeehh.
		IndexBuffer ibo;
		ibo.index_count = index_size_bytes / sizeof(uint32); 
		ibo.size_bytes = index_size_bytes;
		ibo.flags = VertexFlags::READ_WRITE;
		
		CreateIndexBuffer(&ibo);
		WriteBufferData(&ibo, index_data, index_size_bytes, 0);

		VertexArray vao;
		vao.vertex_buffers.push_back(vbo);
		CreateVertexArray(&vao);
		
		mesh->vao = vao;
		mesh->ibo = ibo;		
	}

	void InitializeOpenGl(uint32 window_width, uint32 window_height)
	{		
		glewExperimental = GL_TRUE;
		uint32 res = glewInit();
		Assert(res == GLEW_OK);

		StandardMeshes::Initilize();
		StandardTextures::Initilize();

		OpenGlState::window_width = window_width;
		OpenGlState::window_height = window_height;
		OpenGlState::current_viewport_width = window_width;
		OpenGlState::current_viewport_height = window_height;

	}

	cm::Mat4 CubeMapMatrices::projection = Perspective(90.0f, 1.0f, 0.1f, 10.0f);

	cm::Mat4 CubeMapMatrices::views[6] =
	{
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f),Vec3(0.0f, -1.0f,  0.0f)),
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)),
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)),
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f), Vec3(0.0f, -1.0f,  0.0f)),
		LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f), Vec3(0.0f, -1.0f,  0.0f))
	};



	CubeMapGenerator::CubeMapGenerator()
	{

	}

	CubeMapGenerator::~CubeMapGenerator()
	{
		if (created)
		{
			LOG("WARNING: Freeing EquirectangularToCubemap, please free yourself");
			Free();
		}
	}

	void CubeMapGenerator::Create()
	{
		Assert(!created);
		String vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;

			out vec3 local_pos;

			uniform mat4 projection;
			uniform mat4 view;

			void main()
			{
				local_pos = vpos;  
				gl_Position =  projection * view * vec4(local_pos, 1.0);
			}			
		)";

		String frag_src = R"(
			#version 330 core
			out vec4 FragColor;
			in vec3 local_pos;

			uniform sampler2D equirectangularMap;

			const vec2 invAtan = vec2(0.1591, 0.3183);
			vec2 SampleSphericalMap(vec3 v)
			{
				vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
				uv *= invAtan;
				uv += 0.5;
				return uv;
			}

			void main()
			{		
				vec2 uv = SampleSphericalMap(normalize(local_pos)); // make sure to normalize localPos
				vec3 color = texture(equirectangularMap, uv).rgb;
    
				FragColor = vec4(color, 1.0);
			}		
		)";

		shader = CreateShader(vert_src, frag_src);

		// @NOTE: The frame buffer will be invalid until we convert the cube map
		CreateFrameBuffer(&frame);

		created = true;
	}

	void CubeMapGenerator::Convert(const Texture &src, CubeMap *dst)
	{
		Assert(created);
		Assert(src.object != 0);
		Assert(dst->object != 0);

		BindShader(shader);
		BindFrameBuffer(frame);

		ShaderSetMat4(&shader, "projection", CubeMapMatrices::projection.arr);
		ShaderBindTexture(shader, src, 0, "equirectangularMap");

		uint32 old_width = OpenGlState::current_viewport_width;
		uint32 old_height = OpenGlState::current_viewport_height;

		SetViewPort(0, 0, dst->config.width, dst->config.height);

		for (uint32 i = 0; i < 6; ++i)
		{
			ShaderSetMat4(&shader, "view", CubeMapMatrices::views[i].arr);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dst->object, 0);			

			ClearColourBuffer();
			ClearDepthBuffer();

			GLMesh cube = StandardMeshes::cube;
			RenderMesh(shader, cube);
		}

		SetViewPort(old_width, old_height);
		
		UnbindFrameBuffer();
	}

	void CubeMapGenerator::Free()
	{
		Assert(created);
		FreeShader(&shader);
		FreeFrameBuffer(&frame, false);
		created = false;
	}


	EquirectangularGenerator::EquirectangularGenerator()
	{

	}

	EquirectangularGenerator::~EquirectangularGenerator()
	{

	}

	void EquirectangularGenerator::Create()
	{
		Assert(!created);
		String vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;
			layout (location = 1) in vec3 vnormal;
			layout (location = 2) in vec2 vtext;

			out vec2 texture_coords;

			void main()
			{
				texture_coords = vtext;  
				gl_Position = vec4(vpos, 1.0);
			}			
		)";

		String frag_src = R"(
			#version 330 core
			out vec4 FragColor;

			#define pi 3.14159265359
			in vec2 texture_coords;
			
			uniform samplerCube cube_map;
			vec3 GetVectorFromSphereicalUV(vec2 uv)
			{
					
				float theta = uv.x * pi;
				float phi = uv.y * pi/ 2;
				
				float x = cos(phi) * cos(theta);
				float y	= sin(phi);
				float z = cos(phi) * sin(theta);				
				vec3 dir = vec3(x, y, z);
				return dir;
			}

			void main()
			{		
				vec2 uv = texture_coords * 2 - vec2(1);
				vec3 dir = GetVectorFromSphereicalUV(uv);				
				FragColor = texture(cube_map, dir);
			}		
		)";

		shader = CreateShader(vert_src, frag_src);

		// @NOTE: The frame buffer will be invalid until we convert the cube map
		CreateFrameBuffer(&frame);

		created = true;
	}

	void EquirectangularGenerator::Convert(const CubeMap &src, Texture *dst)
	{
		Assert(created);
		Assert(src.object != 0);
		Assert(dst->object != 0);

		BindFrameBuffer(frame);
		BindShader(shader);

		uint32 old_width = OpenGlState::current_viewport_width;
		uint32 old_height = OpenGlState::current_viewport_height;

		SetViewPort(0, 0, dst->config.width, dst->config.height);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst->object, 0);
				
		ClearColourBuffer();
		ClearDepthBuffer();

		ShaderBindCubeMap(&shader, src, 0, "cube_map");

		GLMesh screen_quad = StandardMeshes::quad;
		RenderMesh(shader, screen_quad);

		SetViewPort(0, 0, old_width, old_height);

		UnbindFrameBuffer();
	}

	void EquirectangularGenerator::Free()
	{
		Assert(created);
		FreeShader(&shader);
		FreeFrameBuffer(&frame, false);
		created = false;
	}

	IrradianceGenerator::IrradianceGenerator()
	{

	}

	IrradianceGenerator::~IrradianceGenerator()
	{
		if (created)
		{
			LOG("WARNING: Freeing IrradianceCaclulator, please free yourself");
			Free();
		}
	}

	void IrradianceGenerator::Create()
	{
		Assert(!created);
		String vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;

			out vec3 local_pos;

			uniform mat4 projection;
			uniform mat4 view;

			void main()
			{
				local_pos = vpos;  
				gl_Position =  projection * view * vec4(local_pos, 1.0);
			}			
		)";

		String frag_src = R"(
		#version 330 core
		out vec4 FragColor;
		in vec3 local_pos;

		uniform samplerCube environmentMap;

		const float PI = 3.14159265359;

		void main()
		{		
			vec3 N = normalize(local_pos);

			vec3 irradiance = vec3(0.0);   
    
			// tangent space calculation from origin point
			vec3 up    = vec3(0.0, 1.0, 0.0);
			vec3 right = cross(up, N);
			up            = cross(N, right);
       
			float sampleDelta = 0.025;
			float nrSamples = 0.0;
			for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
			{
				for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
				{
					// spherical to cartesian (in tangent space)


					vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
					// tangent space to world
					vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

					float c5 = pow(cos(theta), 2);
					irradiance += textureLod(environmentMap, sampleVec, 0).rgb * c5 * sin(theta);
					nrSamples++;
				}
			}
			irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
			FragColor = vec4(irradiance, 1.0);
		}		
		)";

		shader = CreateShader(vert_src, frag_src);

		CreateFrameBuffer(&frame);
		created = true;
	}

	void IrradianceGenerator::Convert(const CubeMap &src, CubeMap *dst)
	{
		Assert(created);
		Assert(src.object != 0);
		Assert(dst->object != 0);

		uint32 old_width = OpenGlState::current_viewport_width;
		uint32 old_height = OpenGlState::current_viewport_height;

		BindShader(shader);
		BindFrameBuffer(frame);

		ShaderSetMat4(&shader, "projection", CubeMapMatrices::projection.arr);
		ShaderBindCubeMap(&shader, src, 0, "environmentMap");

		SetViewPort(0, 0, dst->config.width, dst->config.height);

		for (uint32 i = 0; i < 6; ++i)
		{
			ShaderSetMat4(&shader, "view", CubeMapMatrices::views[i].arr);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dst->object, 0);

			ClearColourBuffer();
			ClearDepthBuffer();

			GLMesh cube = StandardMeshes::cube;
			RenderMesh(shader, cube);
		}
				
		SetViewPort(0, 0, old_width, old_height);

		UnbindFrameBuffer();
	}

	void IrradianceGenerator::Free()
	{
		Assert(created);
		FreeShader(&shader);
		FreeFrameBuffer(&frame, false);
		created = false;
	}

	PrefilterGenerator::PrefilterGenerator()
	{

	}

	PrefilterGenerator::~PrefilterGenerator()
	{
		if (created)
		{
			LOG("WARNING: Freeing PrefilterGenerator, please free yourself");
			Free();
		}
	}

	void PrefilterGenerator::Create()
	{
		Assert(!created);
		String vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;

			out vec3 WorldPos;

			uniform mat4 projection;
			uniform mat4 view;

			void main()
			{
				WorldPos = vpos;  
				gl_Position =  projection * view * vec4(WorldPos, 1.0);
			}			
		)";

		String frag_src = R"(
			#version 330 core
			out vec4 FragColor;
			in vec3 WorldPos;

			uniform samplerCube environmentMap;
			uniform float roughness;

			const float PI = 3.14159265359;
			// ----------------------------------------------------------------------------
			float DistributionGGX(vec3 N, vec3 H, float roughness)
			{
				float a = roughness*roughness;
				float a2 = a*a;
				float NdotH = max(dot(N, H), 0.0);
				float NdotH2 = NdotH*NdotH;

				float nom   = a2;
				float denom = (NdotH2 * (a2 - 1.0) + 1.0);
				denom = PI * denom * denom;

				return nom / denom;
			}
			// ----------------------------------------------------------------------------
			// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
			// efficient VanDerCorpus calculation.
			float RadicalInverse_VdC(uint bits) 
			{
				 bits = (bits << 16u) | (bits >> 16u);
				 bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
				 bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
				 bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
				 bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
				 return float(bits) * 2.3283064365386963e-10; // / 0x100000000
			}
			// ----------------------------------------------------------------------------
			vec2 Hammersley(uint i, uint N)
			{
				return vec2(float(i)/float(N), RadicalInverse_VdC(i));
			}
			// ----------------------------------------------------------------------------
			vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
			{
				float a = roughness*roughness;
	
				float phi = 2.0 * PI * Xi.x;
				float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
				float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
				// from spherical coordinates to cartesian coordinates - halfway vector
				vec3 H;
				H.x = cos(phi) * sinTheta;
				H.y = sin(phi) * sinTheta;
				H.z = cosTheta;
	
				// from tangent-space H vector to world-space sample vector
				vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
				vec3 tangent   = normalize(cross(up, N));
				vec3 bitangent = cross(N, tangent);
	
				vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
				return normalize(sampleVec);
			}
			// ----------------------------------------------------------------------------
			void main()
			{		
				vec3 N = normalize(WorldPos);
    
				// make the simplyfying assumption that V equals R equals the normal 
				vec3 R = N;
				vec3 V = R;

				const uint SAMPLE_COUNT = 10000u;
				//const uint SAMPLE_COUNT = 1024u;
				vec3 prefilteredColor = vec3(0.0);
				float totalWeight = 0.0;
    
				for(uint i = 0u; i < SAMPLE_COUNT; ++i)
				{
					// generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
					vec2 Xi = Hammersley(i, SAMPLE_COUNT);
					vec3 H = ImportanceSampleGGX(Xi, N, roughness);
					vec3 L  = normalize(2.0 * dot(V, H) * H - V);

					float NdotL = max(dot(N, L), 0.0);
					if(NdotL > 0.0)
					{
						// sample from the environment's mip level based on roughness/pdf
						float D   = DistributionGGX(N, H, roughness);
						float NdotH = max(dot(N, H), 0.0);
						float HdotV = max(dot(H, V), 0.0);
						float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

						float resolution = textureSize(environmentMap, 0).x; // resolution of source cubemap (per face)
						float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
						float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

						float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 
            
						prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
						totalWeight      += NdotL;
					}
				}

				prefilteredColor = prefilteredColor / totalWeight;

				FragColor = vec4(prefilteredColor, 1.0);
			}

		)";

		shader = CreateShader(vert_src, frag_src);
		CreateFrameBuffer(&frame);

		created = true;
	}

	void PrefilterGenerator::Convert(const CubeMap &src, CubeMap *dst)
	{
		Assert(created);
		Assert(src.object != 0);
		Assert(dst->object != 0);
		Assert(dst->config.mipmaps)
		
		uint32 old_width = OpenGlState::current_viewport_width;
		uint32 old_height = OpenGlState::current_viewport_height;

		BindShader(shader);
		BindFrameBuffer(frame);

		ShaderSetMat4(&shader, "projection", CubeMapMatrices::projection.arr);
		ShaderBindCubeMap(&shader, src, 0, "environmentMap");

		const uint32 mip_levels = 5;
		for (uint32 mip = 0; mip < mip_levels; ++mip)
		{
			uint32 mip_width = dst->config.width *	 static_cast<uint32>(std::pow(0.5, mip));
			uint32 mip_height = dst->config.height * static_cast<uint32>(std::pow(0.5, mip));
			real32 roughness = (real32)mip / (real32)(mip_levels - 1);

			SetViewPort(0, 0, mip_width, mip_height);
			ShaderSetFloat(&shader, "roughness", roughness);
			for (uint32 i = 0; i < 6; i++)
			{
				ShaderSetMat4(&shader, "view", CubeMapMatrices::views[i].arr);

				frame.colour0_texture_attachment.object = dst->object;
				frame.colour0_texture_attachment.config.type = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dst->object, mip);

				ClearColourBuffer();
				ClearDepthBuffer();

				GLMesh cube = StandardMeshes::cube;
				RenderMesh(shader, cube);
			}
		}

		SetViewPort(0, 0, old_width, old_height);

		// @NOTE: Make sure we dont try to free somthing that was part of the cube map

		UnbindFrameBuffer();

	}


	LookUpTextureGenerator::LookUpTextureGenerator()
	{

	}

	LookUpTextureGenerator::~LookUpTextureGenerator()
	{
		if (created)
		{
			LOG("WARNING: Freeing LookUpTextureGenerator, please free yourself");
			Free();
		}
	}


	void PrefilterGenerator::Free()
	{
		Assert(created);
		FreeShader(&shader);
		FreeFrameBuffer(&frame, false);
		created = false;
	}

	void LookUpTextureGenerator::Create()
	{
		Assert(!created);
		String vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;
			layout (location = 1) in vec3 vnormal;
			layout (location = 2) in vec2 vtext;

			out vec2 TexCoords;

			void main()
			{
				TexCoords = vtext;  
				gl_Position = vec4(vpos, 1.0);
			}			
		)";


		String frag_src = R"(
			#version 330 core
			out vec2 FragColor;

			in vec2 TexCoords;

			const float PI = 3.14159265359;
			// ----------------------------------------------------------------------------
			// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
			// efficient VanDerCorpus calculation.
			float RadicalInverse_VdC(uint bits) 
			{
				 bits = (bits << 16u) | (bits >> 16u);
				 bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
				 bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
				 bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
				 bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
				 return float(bits) * 2.3283064365386963e-10; // / 0x100000000
			}
			// ----------------------------------------------------------------------------
			vec2 Hammersley(uint i, uint N)
			{
				return vec2(float(i)/float(N), RadicalInverse_VdC(i));
			}
			// ----------------------------------------------------------------------------
			vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
			{
				float a = roughness*roughness;
	
				float phi = 2.0 * PI * Xi.x;
				float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
				float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
				// from spherical coordinates to cartesian coordinates - halfway vector
				vec3 H;
				H.x = cos(phi) * sinTheta;
				H.y = sin(phi) * sinTheta;
				H.z = cosTheta;
	
				// from tangent-space H vector to world-space sample vector
				vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
				vec3 tangent   = normalize(cross(up, N));
				vec3 bitangent = cross(N, tangent);
	
				vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
				return normalize(sampleVec);
			}
			// ----------------------------------------------------------------------------
			float GeometrySchlickGGX(float NdotV, float roughness)
			{
				// note that we use a different k for IBL
				float a = roughness;
				float k = (a * a) / 2.0;

				float nom   = NdotV;
				float denom = NdotV * (1.0 - k) + k;

				return nom / denom;
			}
			// ----------------------------------------------------------------------------
			float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
			{
				float NdotV = max(dot(N, V), 0.0);
				float NdotL = max(dot(N, L), 0.0);
				float ggx2 = GeometrySchlickGGX(NdotV, roughness);
				float ggx1 = GeometrySchlickGGX(NdotL, roughness);

				return ggx1 * ggx2;
			}
			// ----------------------------------------------------------------------------
			vec2 IntegrateBRDF(float NdotV, float roughness)
			{
				vec3 V;
				V.x = sqrt(1.0 - NdotV*NdotV);
				V.y = 0.0;
				V.z = NdotV;

				float A = 0.0;
				float B = 0.0; 

				vec3 N = vec3(0.0, 0.0, 1.0);
    
				const uint SAMPLE_COUNT = 1024u;
				for(uint i = 0u; i < SAMPLE_COUNT; ++i)
				{
					// generates a sample vector that's biased towards the
					// preferred alignment direction (importance sampling).
					vec2 Xi = Hammersley(i, SAMPLE_COUNT);
					vec3 H = ImportanceSampleGGX(Xi, N, roughness);
					vec3 L = normalize(2.0 * dot(V, H) * H - V);

					float NdotL = max(L.z, 0.0);
					float NdotH = max(H.z, 0.0);
					float VdotH = max(dot(V, H), 0.0);

					if(NdotL > 0.0)
					{
						float G = GeometrySmith(N, V, L, roughness);
						float G_Vis = (G * VdotH) / (NdotH * NdotV);
						float Fc = pow(1.0 - VdotH, 5.0);

						A += (1.0 - Fc) * G_Vis;
						B += Fc * G_Vis;
					}
				}
				A /= float(SAMPLE_COUNT);
				B /= float(SAMPLE_COUNT);
				return vec2(A, B);
			}
			// ----------------------------------------------------------------------------
			void main() 
			{
				vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);
				FragColor = integratedBRDF;
			}
		)";


		shader = CreateShader(vert_src, frag_src);

		CreateFrameBuffer(&frame);

		created = true;
	}

	void LookUpTextureGenerator::Convert(Texture *dst)
	{
		Assert(created);
		Assert(dst->object != 0);
		
		uint32 old_width = OpenGlState::current_viewport_width;
		uint32 old_height = OpenGlState::current_viewport_height;

		BindShader(shader);
		BindFrameBuffer(frame);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst->object, 0);
		
		ClearColourBuffer();
		ClearDepthBuffer();

		SetViewPort(0, 0, dst->config.width, dst->config.height);

		RenderMesh(shader, StandardMeshes::quad);

		SetViewPort(0, 0, old_width, old_height);

	}

	void LookUpTextureGenerator::Free()
	{
		Assert(created);
		FreeShader(&shader);
		FreeFrameBuffer(&frame, false);
		created = false;
	}

	void SampleTextureBlur::Create(uint32 src_width, uint32 src_height, uint32 sampling_count)
	{
		Assert(0); // @REASON: THIS IS A WORK IN PROGRESS
		sampling_textures = new Texture[sampling_count];
		sampling_texture_count = sampling_count;
		for (uint32 i = 0; i < sampling_texture_count; i++)
		{
			sampling_textures[i].config.texture_format = GL_RGBA16F;
			sampling_textures[i].config.pixel_format = GL_RGBA;
			sampling_textures[i].config.wrap_t_mode = GL_CLAMP_TO_EDGE;
			sampling_textures[i].config.wrap_s_mode = GL_CLAMP_TO_EDGE;
			sampling_textures[i].config.min_filter = GL_LINEAR;
			sampling_textures[i].config.mag_filter = GL_LINEAR;
			sampling_textures[i].config.width = src_width *  static_cast<uint32>(std::pow(0.5, i));
			sampling_textures[i].config.height= src_height * static_cast<uint32>(std::pow(0.5, i));
			CreateTexture(&sampling_textures[i], nullptr);
		}

		CreateFrameBuffer(&frame);
	}

	void SampleTextureBlur::Blur(const Texture src, Texture *dst)
	{
		BindFrameBuffer(frame);
		BindShader(shader);
		DisableDepthBuffer();
		// @TODO: Do asserts
		// @NOTE: Store in the first down sampling texture
		// @NOTE: There is a bug here where we do out box filter on an un-down sampled texture in this first pass

#if 1
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sampling_textures[0].object, 0);
		
		SetViewPort(0, 0, sampling_textures[0].config.width, sampling_textures[0].config.height);
		ClearColourBuffer();
		ClearDepthBuffer();
		ShaderSetInt32(&shader, "first", 1);
		ShaderBindTexture(shader, src, 0, "colour_map");
		RenderMesh(shader, StandardMeshes::quad);
#else	
		sampling_textures[0] = src;
#endif
		ShaderSetFloat(&shader, "delta", 1);
		//ShaderSetInt32(&shader, "first", 0);
		for (int32 i = 1; i < static_cast<int32>(sampling_texture_count); i++)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sampling_textures[i].object, 0);

			SetViewPort(0, 0, sampling_textures[i].config.width, sampling_textures[i].config.height);
			ClearColourBuffer();
			ClearDepthBuffer();
			ShaderBindTexture(shader, sampling_textures[i - 1], 0, "colour_map");
			RenderMesh(shader, StandardMeshes::quad);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		ShaderSetFloat(&shader, "delta", 0.5f);
		for (int32 i = sampling_texture_count - 2; i >= 0; i--)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sampling_textures[i].object, 0);

			SetViewPort(0, 0, sampling_textures[i].config.width, sampling_textures[i].config.height);
			// @NOTE: We don't clear the buffers, because of the blending

			ShaderBindTexture(shader, sampling_textures[i + 1], 0, "colour_map");
			RenderMesh(shader, StandardMeshes::quad);
		}
		glDisable(GL_BLEND);

		SetViewPort(0, 0, OpenGlState::window_width, OpenGlState::window_height);
		EnableDepthBuffer();
		UnbindFrameBuffer();
	
	}

	void SampleTextureBlur::Free()
	{
		delete sampling_textures;
	}
	
	SimpleTextureBlur::SimpleTextureBlur()
	{

	}

	SimpleTextureBlur::~SimpleTextureBlur()
	{
		if (created)
		{
			LOG("WARNING: Freeing SimpleTextureBlur, please free yourself");
			Free();
		}
	}

	void SimpleTextureBlur::Create(uint32 src_width, uint32 src_height, uint32 kernel_size)
	{
		Assert(!created);
		Assert(kernel_size <= 1000); // @REASON: Making sure things don't get out of hand.
		String vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;
			layout (location = 1) in vec3 vnormal;
			layout (location = 2) in vec2 vtext;

			out vec2 texture_coords;

			void main()
			{
				texture_coords = vtext;
				gl_Position = vec4(vpos, 1.0);
			}			
		)";

		String frag_src = R"(
			#version 330 core
			out vec4 out_colour;

			in vec2 texture_coords;

			uniform int kernel_size;
			uniform sampler2D src_texture;

			void main()
			{		
				vec2 texel_size = 1.0 / vec2(textureSize(src_texture, 0));

				vec3 colour = vec3(0.0);
				for (int x = -kernel_size; x < kernel_size; x++) 
				{
					for (int y = -kernel_size; y < kernel_size; y++) 
					{												
						vec2 offset = vec2(float(x), float(y)) * texel_size;
						colour += texture(src_texture, texture_coords + offset).rgb;
					}
				}

				// @NOTE: Is this--> c / ((kernel_size + kernel_size) * (kernel_size + kernel_size));  
				colour = colour / (4 * kernel_size * kernel_size );  
				out_colour = vec4(colour, 1);
			}		
		)";

		shader.config.src_vert = vert_src;
		shader.config.src_frag = frag_src;
		CreateShader(&shader);
		
		CreateFrameBuffer(&frame);
		BindFrameBuffer(frame);
		frame.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
		frame.colour0_texture_attachment.config.pixel_format = GL_RGBA;
		frame.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		frame.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		frame.colour0_texture_attachment.config.min_filter = GL_LINEAR;
		frame.colour0_texture_attachment.config.mag_filter = GL_LINEAR;
		frame.colour0_texture_attachment.config.width = src_width;
		frame.colour0_texture_attachment.config.height = src_height;

		CreateTexture(&frame.colour0_texture_attachment, nullptr);
		
		FrameBufferBindColourAttachtments(&frame);

		Assert(CheckFrameBuffer(frame));

		UnbindFrameBuffer();

		created = true;
		this->kernel_size = kernel_size;
	}

	void SimpleTextureBlur::Blur(const Texture &src, Texture *dst)
	{
		Assert(created);
		Assert(src.object != 0);
		Assert(dst->object != 0);
		Assert(src.config.width == frame.colour0_texture_attachment.config.width);
		Assert(src.config.height == frame.colour0_texture_attachment.config.height);
		Assert(dst->config.width == frame.colour0_texture_attachment.config.width);
		Assert(dst->config.height == frame.colour0_texture_attachment.config.height);
		
		uint32 old_width = OpenGlState::current_viewport_width;
		uint32 old_height = OpenGlState::current_viewport_height;

		BindFrameBuffer(frame);
		BindShader(shader);
				
		SetViewPort(src.config.width, src.config.height);

		ClearColourBuffer();

		int32 k = static_cast<int32>(kernel_size);
		ShaderSetInt32(&shader, "kernel_size", k);
		ShaderBindTexture(shader, src, 0, "src_texture");

		RenderMesh(shader, StandardMeshes::quad);

		SetViewPort(old_width, old_height);

		UnbindFrameBuffer();

		CopyTexture(&frame.colour0_texture_attachment, dst);
	}

	void SimpleTextureBlur::Free()
	{
		Assert(created);
		FreeFrameBuffer(&frame, true);
		FreeShader(&shader);
		created = false;
	}
	
	GaussianTextureBlur::GaussianTextureBlur()
	{

	}

	GaussianTextureBlur::~GaussianTextureBlur()
	{
		if (created)
		{
			LOG("WARNING: Freeing GaussianTextureBlur, please free yourself");
			Free();
		}
	}

	void GaussianTextureBlur::Create(uint32 src_width, uint32 src_height, uint32 kernel_size, real32 downsample_mul, uint32 iterations)
	{
		Assert(!created);
		Assert(kernel_size <= 20 && kernel_size >= 1);			// @REASON: This the make in shader. Just making sure things do not get out of hand
		Assert(downsample_mul >= 0.1 && downsample_mul <= 1);	// @REASON: Just making sure things do not get out of hand
		Assert(iterations <= 4 && iterations >= 1);				// @REASON: Just making sure things do not get out of hand
		String horizontal_vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;
			layout (location = 1) in vec3 vnormal;
			layout (location = 2) in vec2 vtext;

			out vec2 texture_coords[20];			
			
			uniform float texel_size;

			void main()
			{
				for (int i = -5; i <= 5; i++)
				{
					texture_coords[i + 5] = vtext + vec2(i * texel_size, 0);  
				}					
				gl_Position = vec4(vpos, 1.0);
			}			
		)";

		String vertical_vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;
			layout (location = 1) in vec3 vnormal;
			layout (location = 2) in vec2 vtext;

			out vec2 texture_coords[20];			
			
			uniform float texel_size;

			void main()
			{
				for (int i = -5; i <= 5; i++)
				{
					texture_coords[i + 5] = vtext + vec2(0, i * texel_size);  
				}					
				gl_Position = vec4(vpos, 1.0);
			}			
		)";

		String frag_src = R"(
			#version 330 core
			out vec4 g_colour;

			#define pi 3.14159265359
			in vec2 texture_coords[20];
			
			uniform sampler2D src_texture;

			void main()
			{		
				vec4 out_colour = vec4(0.0);
				out_colour += texture(src_texture, texture_coords[0]) * 0.0093;
				out_colour += texture(src_texture, texture_coords[1]) * 0.028002;
				out_colour += texture(src_texture, texture_coords[2]) * 0.065984;
				out_colour += texture(src_texture, texture_coords[3]) * 0.121703;
				out_colour += texture(src_texture, texture_coords[4]) * 0.175713;
				out_colour += texture(src_texture, texture_coords[5]) * 0.198596;
				out_colour += texture(src_texture, texture_coords[6]) * 0.175713;
				out_colour += texture(src_texture, texture_coords[7]) * 0.121703;
				out_colour += texture(src_texture, texture_coords[8]) * 0.065984;
				out_colour += texture(src_texture, texture_coords[9]) * 0.028002;
				out_colour += texture(src_texture, texture_coords[10]) * 0.0093;

				g_colour = out_colour;
			}		
		)";

		String upsample_vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;
			layout (location = 1) in vec3 vnormal;
			layout (location = 2) in vec2 vtext;

			out vec2 texture_coords;

			void main()
			{
				texture_coords = vtext;  
				gl_Position = vec4(vpos, 1.0);
			}			
		)";

		String upsample_frag_src = R"(
			#version 330 core
			out vec4 out_colour;

			in vec2 texture_coords;
			uniform sampler2D src_texture;
			
			void main()
			{		
				// @NOTE: Make sure we don't get a nan value by clamping it to zero
				// @TODO: Use isnan() instead
				vec3 colour = max(vec3(0.0), texture(src_texture, texture_coords).rgb);
				out_colour = vec4(colour, 1);
			}		
		)";

		vertical_shader.config.src_vert = vertical_vert_src;
		vertical_shader.config.src_frag = frag_src;
		CreateShader(&vertical_shader);

		horizontal_shader.config.src_vert = horizontal_vert_src;
		horizontal_shader.config.src_frag = frag_src;
		CreateShader(&horizontal_shader);
		
		upsample_shader.config.src_vert = upsample_vert_src;
		upsample_shader.config.src_frag = upsample_frag_src;
		CreateShader(&upsample_shader);

		horizontal_frames = new FrameBuffer[iterations];
		vertical_frames = new FrameBuffer[iterations];

		for (uint32 i = 0; i < iterations; i++)
		{
			horizontal_frames[i].colour0_texture_attachment.config.texture_format = GL_RGBA16F;
			horizontal_frames[i].colour0_texture_attachment.config.pixel_format = GL_RGBA;
			horizontal_frames[i].colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
			horizontal_frames[i].colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
			horizontal_frames[i].colour0_texture_attachment.config.wrap_r_mode = GL_CLAMP_TO_EDGE;
			horizontal_frames[i].colour0_texture_attachment.config.min_filter = GL_LINEAR;
			horizontal_frames[i].colour0_texture_attachment.config.mag_filter = GL_LINEAR;
			horizontal_frames[i].colour0_texture_attachment.config.width = static_cast<uint32>(src_width * downsample_mul);
			horizontal_frames[i].colour0_texture_attachment.config.height = static_cast<uint32>(src_height * downsample_mul);

			vertical_frames[i].colour0_texture_attachment.config.texture_format = GL_RGBA16F;
			vertical_frames[i].colour0_texture_attachment.config.pixel_format = GL_RGBA;
			vertical_frames[i].colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
			vertical_frames[i].colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
			vertical_frames[i].colour0_texture_attachment.config.wrap_r_mode= GL_CLAMP_TO_EDGE;
			vertical_frames[i].colour0_texture_attachment.config.min_filter = GL_LINEAR;
			vertical_frames[i].colour0_texture_attachment.config.mag_filter = GL_LINEAR;
			vertical_frames[i].colour0_texture_attachment.config.width = static_cast<uint32>(src_width * downsample_mul);
			vertical_frames[i].colour0_texture_attachment.config.height = static_cast<uint32>(src_height * downsample_mul);

			CreateTexture(&horizontal_frames[i].colour0_texture_attachment, nullptr);
			CreateTexture(&vertical_frames[i].colour0_texture_attachment, nullptr);

			CreateFrameBuffer(&horizontal_frames[i]);
			CreateFrameBuffer(&vertical_frames[i]);

			FrameBufferBindColourAttachtments(&horizontal_frames[i]);
			FrameBufferBindColourAttachtments(&vertical_frames[i]);

			Assert(CheckFrameBuffer(horizontal_frames[i]));
			Assert(CheckFrameBuffer(vertical_frames[i]));

			src_width = horizontal_frames[i].colour0_texture_attachment.config.width;
			src_height = horizontal_frames[i].colour0_texture_attachment.config.height;
		}

		created = true;
		this->kernel_size = kernel_size;
		this->downsample_mul = downsample_mul;
		this->iterations = iterations;
	}

	void GaussianTextureBlur::Blur(const Texture &src, Texture *dst)
	{
		Assert(created);
		Assert(static_cast<uint32>(src.config.width * downsample_mul)  == horizontal_frames[0].colour0_texture_attachment.config.width);
		Assert(static_cast<uint32>(src.config.height * downsample_mul) == horizontal_frames[0].colour0_texture_attachment.config.height);
		Assert(src.object != 0);
		Assert(dst->object != 0);

		uint32 old_width = OpenGlState::current_viewport_width;
		uint32 old_height = OpenGlState::current_viewport_height;

		Texture current = src;
		
		for (uint32 i = 0; i < iterations; i++)
		{
			BindFrameBuffer(horizontal_frames[i]);
			BindShader(horizontal_shader);

			SetViewPort(horizontal_frames[i].colour0_texture_attachment.config.width, horizontal_frames[i].colour0_texture_attachment.config.height);

			ClearColourBuffer();

			ShaderSetFloat(&horizontal_shader, "texel_size", static_cast<real32>((1.0 / horizontal_frames[i].colour0_texture_attachment.config.width)));
			ShaderBindTexture(horizontal_shader, current, 0, "src_texture");

			RenderMesh(horizontal_shader, StandardMeshes::quad);

			UnbindFrameBuffer();


			BindFrameBuffer(vertical_frames[i]);
			BindShader(vertical_shader);

			ClearColourBuffer();

			ShaderSetFloat(&vertical_shader, "texel_size", static_cast<real32>((1.0 / vertical_frames[i].colour0_texture_attachment.config.height)));
			ShaderBindTexture(vertical_shader, horizontal_frames[i].colour0_texture_attachment, 0, "src_texture");

			RenderMesh(vertical_shader, StandardMeshes::quad);

			UnbindFrameBuffer();

			current = vertical_frames[i].colour0_texture_attachment;
		}

		SetViewPort(dst->config.width, dst->config.height);
		
		// @NOTE: We have have upsample the lowest blur to the dst
		//		: resolution that's why we can't just call CopyTexture
		BindShader(upsample_shader);
		BindFrameBuffer(horizontal_frames[0]);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst->object, 0);

		ClearColourBuffer();

		ShaderBindTexture(upsample_shader, current, 0, "src_texture");

		RenderMesh(upsample_shader, StandardMeshes::quad);
				
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, horizontal_frames[0].colour0_texture_attachment.object, 0);
		
		SetViewPort(old_width, old_height);		

		UnbindFrameBuffer();
	}

	void GaussianTextureBlur::Free()
	{
		Assert(created);
		for (uint32 i = 0; i < iterations; i++)
		{
			FreeFrameBuffer(&vertical_frames[i], true);
			FreeFrameBuffer(&horizontal_frames[i], true);
		}
		FreeShader(&vertical_shader);
		FreeShader(&horizontal_shader);
		
		delete[] vertical_frames;
		delete[] horizontal_frames;

		created = false;
	}

	void GaussianTextureBlur::SetKernelSize(uint32 ksize)
	{
		Assert(ksize == 11); // @REASON: It's on the todo check class for desc.
		Assert(ksize >= 1 && ksize <= max_kernel_size);
		kernel_size = ksize;	
	}

	int32 GaussianTextureBlur::GetKernelSize()
	{
		return kernel_size;
	}

	int32 GaussianTextureBlur::GetMaxKernelSize()
	{
		return max_kernel_size;
	}

	LuminanceFilter::LuminanceFilter()
	{

	}

	LuminanceFilter::~LuminanceFilter()
	{
		if (created)
		{
			LOG("WARNING: Freeing GaussianTextureBlur, please free yourself");
			Free();
		}
	}

	void LuminanceFilter::Create(uint32 src_width, uint32 src_height, real32 threshold)
	{
		Assert(!created);
		Assert(src_width != 0);
		Assert(src_height != 0);
		String vert_src = R"(
			#version 330 core
			layout (location = 0) in vec3 vpos;
			layout (location = 1) in vec3 vnormal;
			layout (location = 2) in vec2 vtext;

			out vec2 texture_coords;

			void main()
			{
				texture_coords = vtext;  
				gl_Position = vec4(vpos, 1.0);
			}			
		)";

		String frag_src = R"(
			#version 330 core
			out vec4 out_colour;

			#define pi 3.14159265359
			in vec2 texture_coords;
			
			uniform float t;
			uniform sampler2D src_texture;


			vec3 Prefilter (vec3 c) 
			{			    
				vec3 luma = vec3(0.2126, 0.7152, 0.0722);
				float brightness =  dot(luma, c);


				//float contribution = max(0, brightness - t);
			    //contribution /= max(brightness, 0.00001);
			    //return c * contribution;

				float knee = t * 0.5;
				float soft = brightness - t + knee;
				soft = clamp(soft, 0, 2 * knee);
				soft = soft * soft / (4 * knee + 0.00001);
				float contribution = max(soft, brightness - t);
				contribution /= max(brightness, 0.00001);
				return c * contribution;

				//vec3 luma = vec3(0.2126, 0.7152, 0.0722);
				//float brightness =  dot(luma, c);
				//return c * brightness  * t;
			}

			void main()
			{		
				vec3 colour = texture(src_texture, texture_coords).rgb;				
				colour = Prefilter(colour);
				out_colour = vec4(colour, 1);
			}		
		)";
		

		shader.config.src_vert = vert_src;
		shader.config.src_frag = frag_src;
		CreateShader(&shader);


		CreateFrameBuffer(&frame);
		frame.colour0_texture_attachment.config.texture_format = GL_RGBA16F;
		frame.colour0_texture_attachment.config.pixel_format = GL_RGBA;
		frame.colour0_texture_attachment.config.wrap_t_mode = GL_CLAMP_TO_EDGE;
		frame.colour0_texture_attachment.config.wrap_s_mode = GL_CLAMP_TO_EDGE;
		frame.colour0_texture_attachment.config.min_filter = GL_LINEAR;
		frame.colour0_texture_attachment.config.mag_filter = GL_LINEAR;
		frame.colour0_texture_attachment.config.width = src_width;
		frame.colour0_texture_attachment.config.height = src_height;
		
		CreateTexture(&frame.colour0_texture_attachment, nullptr);

		FrameBufferBindColourAttachtments(&frame);
		Assert(CheckFrameBuffer(frame));

		created = true;
		this->threshold = threshold;
	}

	void LuminanceFilter::Filter(const Texture &src, Texture *dst)
	{
		Assert(created);
		Assert(src.object != 0);
		Assert(dst->object != 0);
		Assert(src.config.width == dst->config.width);
		Assert(src.config.height == dst->config.height);

		uint32 old_width = OpenGlState::current_viewport_width;
		uint32 old_height = OpenGlState::current_viewport_height;

		BindFrameBuffer(frame);
		BindShader(shader);
			
		SetViewPort(dst->config.width, dst->config.height);
		
		ShaderSetFloat(&shader, "t", threshold);
		ShaderBindTexture(shader, src, 0, "src_texture");

		ClearColourBuffer(); 

		RenderMesh(shader, StandardMeshes::quad);

		CopyTexture(&frame.colour0_texture_attachment, dst);

		SetViewPort(old_width, old_height);

		UnbindFrameBuffer();
	}

	void LuminanceFilter::Free()
	{
		Assert(created);
			
		FreeShader(&shader);
		FreeFrameBuffer(&frame, true);

		created = false;
	}



	HemisphereKernel::HemisphereKernel()
	{

	}

	HemisphereKernel::~HemisphereKernel()
	{
		if (created)
		{
			LOG("WARNING: Freeing GaussianTextureBlur, please free yourself");
			Free();
		}
	}

	void HemisphereKernel::SetKernelSize(uint32 kernel_size)
	{
		Assert(created);
		Assert(kernel_size != 0);

		if (this->kernel_size > kernel_size)
		{
		}
			delete[] kernel_samples;
			kernel_samples = new Vec3[kernel_size];
		this->kernel_size = kernel_size;

		
		for (uint32 i = 0; i < kernel_size; i++)
		{
			Vec3 sample = Vec3(
				RandomBillateral(),
				RandomBillateral(),
				RandomUnillateral()
			);
			sample = Normalize(sample);
			sample = sample * RandomUnillateral();

			real32 scale = (real32)i / 64.0f;
			scale = Lerp(0.1f, 1.0f, scale * scale);
			sample = sample * scale;

			kernel_samples[i] = sample;
		}		
	}

	void HemisphereKernel::Create(uint32 kernel_size, uint32 noise_texture_size)
	{
		Assert(!created);
		Assert(kernel_size != 0);
		Assert(noise_texture_size != 0);

		kernel_samples = new Vec3[kernel_size];
		this->kernel_size = kernel_size;
		// @NOTE: Gen samples
		for (uint32 i = 0; i < kernel_size; i++)
		{
			Vec3 sample = Vec3(
				RandomBillateral(),
				RandomBillateral(),
				RandomUnillateral()
			);
			sample = Normalize(sample);
			sample = sample * RandomUnillateral();

			real32 scale = (real32)i / 64.0f;
			scale = Lerp(0.1f, 1.0f, scale * scale);
			sample = sample * scale;

			kernel_samples[i] = sample;
		}

		// @NOTE: Gen noise texture
		Vec3 *noise = new Vec3[noise_texture_size * noise_texture_size];
		for (uint32 i = 0; i < noise_texture_size * noise_texture_size; i++)
		{
			Vec3 n(
				RandomBillateral(),
				RandomBillateral(),
				0.0f);
			noise[i] = n;
		}
		noise_texture.config.height = noise_texture_size;
		noise_texture.config.width = noise_texture_size;
		noise_texture.config.data_type = GL_FLOAT;
		noise_texture.config.texture_format = GL_RGBA16F;
		noise_texture.config.min_filter = GL_NEAREST;
		noise_texture.config.mag_filter = GL_NEAREST;
		noise_texture.config.wrap_s_mode = GL_REPEAT;
		noise_texture.config.wrap_t_mode = GL_REPEAT;
		noise_texture.config.wrap_r_mode = GL_REPEAT;

		CreateTexture(&noise_texture, noise);
		delete[] noise;

		created = true;
	}

	void HemisphereKernel::Free()
	{
		Assert(created);
		FreeTexture(&noise_texture);
		delete[] kernel_samples;
		created = false;
	}



	

}