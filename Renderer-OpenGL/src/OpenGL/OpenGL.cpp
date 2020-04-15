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
			   		
		uint32 size_bytes = buffer->lbo.GetTotalSize();

		glBufferStorage(GL_UNIFORM_BUFFER, size_bytes, NULL, static_cast<uint32>(VertexFlags::READ_WRITE));

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		buffer->object = buff;
		buffer->size_bytes = size_bytes;
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
		for (int32 i = 0; i < vbo_count; i++)
		{
			VertexBuffer current_vbo = vao->vertex_buffers[i];
			BufferLayout lbo = current_vbo.lbo;
			BindVertexBuffer(current_vbo);
			current_vbo.lbo.Reset();
			uint32 stride = lbo.GetStride();
			for (uint32 i = 0; i < lbo.GetComponentCount(); i++)
			{
				uint32 offset = lbo.GetCurrentOffSet();
				uint32 compCount = lbo.GetCurrentComponentAttribCount();
				if (lbo.GetCurrentShaderType() == ShaderDataType::Mat4)
				{
					uint32 smol_offset = offset;
					for (int32 ii = 0; ii < 4; ii++)
					{
						glEnableVertexAttribArray(attrib_counter);
						glVertexAttribPointer(attrib_counter, 4, GL_FLOAT, GL_FALSE, stride, (void*)(smol_offset));
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
						glVertexAttribPointer(attrib_counter, 3, GL_FLOAT, GL_FALSE, stride, (void*)(offset));
						glVertexAttribDivisor(attrib_counter, lbo.GetAttributeDivisor());
						attrib_counter++;
					}
				}
				else
				{
					glEnableVertexAttribArray(attrib_counter);
					glVertexAttribPointer(attrib_counter, compCount, GL_FLOAT, GL_FALSE, stride, (void*)(offset));
					glVertexAttribDivisor(attrib_counter, lbo.GetAttributeDivisor());
					attrib_counter++;
				}
				lbo.Next();
			}
		}
		glBindVertexArray(0);
		vao->object = buffer_id;
	}

	BufferLayout GetTotalVertexBufferLayout(const VertexArray &vao)
	{
		Assert(0);//Depricated
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
		Assert(0); //Depricated
		uint32 buffer_id = vao->object;
		if (buffer_id == 0)
		{
			glGenVertexArrays(1, &buffer_id);
		}

		glBindVertexArray(buffer_id);
		BindVertexBuffer(vbo);
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
		UnbindVertexBuffer();

		vao->vertex_buffers.push_back(vbo);
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

		if (cube_map->config.generate_mip_maps)
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

	void ShaderSetInt32(Shader *shader, const std::string &uniform_name, int x)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("INT32", loc);
		glUniform1i(loc, x);
	}

	void ShaderSetFloat(Shader *shader, const std::string &uniform_name, float x)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("FLOAT", loc);
		glUniform1f(loc, x);
	}

	void ShaderSetVec3(Shader *shader, const std::string &uniform_name, float x, float y, float z)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("VEC3", loc);
		glUniform3f(loc, x, y, z);
	}

	void ShaderSetVec3(Shader *shader, const std::string &uniform_name, float* data)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("VEC3", loc);
		glUniform3fv(loc, 1, data);
	}

	void ShaderSetMat4(Shader *shader, const std::string &uniform_name, float* data)
	{
		uint32 loc = GetUniformLocation(shader, uniform_name);
		CHECK("MAT4", loc);
		glUniformMatrix4fv(loc, 1, false, data);
	}

	void ShaderBindUniformBuffer(const Shader &shader, uint32 binding_point, const std::string &uniform_name)
	{
		uint32 uniformBlockIndexRed = glGetUniformBlockIndex(shader.shader_program, uniform_name.c_str());
		glUniformBlockBinding(shader.shader_program, uniformBlockIndexRed, binding_point);
	}

	void ShaderBindTexture(Shader &shader, const Texture &texture, uint8 texture_slot, const std::string &uniform_name)
	{
		Assert(texture_slot < 4);
		uint32 loc = -1;
		std::string name = uniform_name;
		if (uniform_name == "")
		{
			name = texture.config.uniform_name;
		}
		loc = GetUniformLocation(&shader, name);
		if (loc == -1)
		{
			std::string ss = "ERROR::SHADER::TEXTURE: " + shader.name + " " + uniform_name;
			LOGC(ss.c_str());
		}
		else
		{			
			glUniform1i(loc, texture_slot);
			glActiveTexture(GL_TEXTURE0 + texture_slot);
			glBindTexture(GL_TEXTURE_2D, texture.object);
		}		
	}
	
	void ShaderBindCubeMap(Shader *shader, const CubeMap &cube_map, uint8 texture_slot, const std::string &uniform_name)
	{
		Assert(texture_slot < 4);
		uint32 loc = -1;
		std::string name = uniform_name;
		if (uniform_name == "")
		{
			name = cube_map.config.uniform_name;
		}

		loc = GetUniformLocation(shader, name);
		if (loc == -1)
		{
			std::string ss = "ERROR::SHADER::TEXTURE: " + shader->name + " " + uniform_name;
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

				RenderBuffer *render_buffer = &fbo->render_attchment;
				FreeRenderBuffer(render_buffer);
			}			
		}
	}

	void FreeRenderBuffer(RenderBuffer *rb)
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


	void FrameBufferAddDepthAttachments(FrameBuffer *buffer)
	{
		Assert(buffer->object != 0);
		Assert(buffer->depth_texture_attachment.object != 0);
		BindFrameBuffer(*buffer);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,	buffer->depth_texture_attachment.object, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		UnbindFrameBuffer();
	}

	void FrameAddBufferRenderAttachtment(FrameBuffer *buffer)
	{
		Assert(buffer->object != 0);
		Assert(buffer->render_attchment.object == 0);
		Assert(buffer->render_attchment.width != 0);
		Assert(buffer->render_attchment.height!= 0);
		
		uint32 rbo;
		BindFrameBuffer(*buffer);
		
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
				
		glRenderbufferStorage(GL_RENDERBUFFER, buffer->render_attchment.render_buffer_format, 
			buffer->render_attchment.width, buffer->render_attchment.height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, buffer->render_attchment.render_buffer_attachment_type,
			GL_RENDERBUFFER, rbo);
		
		buffer->render_attchment.object = rbo;
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
		mat_vbo.lbo = BufferLayout({ ShaderDataType::Mat4 });
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

	void GetOpenglStatistics(OpenGLStatistics *stats)
	{
		stats->vendor = std::string((const char*)glGetString(GL_VENDOR));
		stats->renderer = std::string((const char*)glGetString(GL_RENDERER));
		stats->version = std::string((const char*)glGetString(GL_VERSION));
		stats->shading_lang = std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

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

	void InitializeOpenGl(uint32 window_width, uint32 window_height)
	{
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			Assert(0);
		}
		OpenGlState::window_width = window_width;
		OpenGlState::window_height = window_height;
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
		std::string vert_src = R"(
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

		std::string frag_src = R"(
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

		ChangeViewPort(0, 0, dst->config.width, dst->config.height);

		for (uint32 i = 0; i < 6; ++i)
		{
			ShaderSetMat4(&shader, "view", CubeMapMatrices::views[i].arr);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dst->object, 0);			

			ClearColourBuffer();
			ClearDepthBuffer();

			GLMesh cube = StandardMeshes::cube;
			RenderMesh(shader, cube);
		}

		ChangeViewPort(0, 0, 1280, 720);
		
		UnbindFrameBuffer();
	}

	void CubeMapGenerator::Free()
	{
		Assert(created);
		FreeShader(&shader);
		FreeFrameBuffer(&frame, true);
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
		std::string vert_src = R"(
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

		std::string frag_src = R"(
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

		ChangeViewPort(0, 0, dst->config.width, dst->config.height);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst->object, 0);
				
		ClearColourBuffer();
		ClearDepthBuffer();

		ShaderBindCubeMap(&shader, src, 0, "cube_map");

		GLMesh screen_quad = StandardMeshes::quad;
		RenderMesh(shader, screen_quad);

		ChangeViewPort(0, 0, 1280, 720);

		UnbindFrameBuffer();
	}

	void EquirectangularGenerator::Free()
	{
		Assert(created);
		FreeShader(&shader);
		FreeFrameBuffer(&frame, true);
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
		std::string vert_src = R"(
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

		std::string frag_src = R"(
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

					irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
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

		BindShader(shader);
		BindFrameBuffer(frame);

		ShaderSetMat4(&shader, "projection", CubeMapMatrices::projection.arr);
		ShaderBindCubeMap(&shader, src, 0, "environmentMap");

		ChangeViewPort(0, 0, dst->config.width, dst->config.height);

		for (uint32 i = 0; i < 6; ++i)
		{
			ShaderSetMat4(&shader, "view", CubeMapMatrices::views[i].arr);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dst->object, 0);

			ClearColourBuffer();
			ClearDepthBuffer();

			GLMesh cube = StandardMeshes::cube;
			RenderMesh(shader, cube);
		}
				
		ChangeViewPort(0, 0, 1280, 720);

		UnbindFrameBuffer();
	}

	void IrradianceGenerator::Free()
	{
		Assert(created);
		FreeShader(&shader);
		FreeFrameBuffer(&frame, true);
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
		std::string vert_src = R"(
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

		std::string frag_src = R"(
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
		Assert(dst->config.generate_mip_maps)

			BindShader(shader);
		BindFrameBuffer(frame);

		ShaderSetMat4(&shader, "projection", CubeMapMatrices::projection.arr);
		ShaderBindCubeMap(&shader, src, 0, "environmentMap");

		const uint32 mip_levels = 5;
		for (uint32 mip = 0; mip < mip_levels; ++mip)
		{
			uint32 mip_width = dst->config.width * std::pow(0.5, mip);
			uint32 mip_height = dst->config.height * std::pow(0.5, mip);
			real32 roughness = (real32)mip / (real32)(mip_levels - 1);

			ChangeViewPort(0, 0, mip_width, mip_height);
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

		ChangeViewPort(0, 0, 1280, 720);

		// @NOTE: Make sure we dont try to free somthing that was part of the cube map
		frame.colour0_texture_attachment.object = 0;

		UnbindFrameBuffer();

	}


	LookUpTextureGenerator::LookUpTextureGenerator()
	{

	}

	LookUpTextureGenerator::~LookUpTextureGenerator()
	{

	}


	void PrefilterGenerator::Free()
	{
		Assert(created);
		FreeShader(&shader);
		FreeFrameBuffer(&frame, true);
		created = false;
	}

	void LookUpTextureGenerator::Create()
	{
		Assert(!created);
		std::string vert_src = R"(
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


		std::string frag_src = R"(
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

		BindShader(shader);
		BindFrameBuffer(frame);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst->object, 0);
		
		ClearColourBuffer();
		ClearDepthBuffer();

		ChangeViewPort(0, 0, dst->config.width, dst->config.height);

		RenderMesh(shader, StandardMeshes::quad);

		ChangeViewPort(0, 0, 1280, 720);

		frame.colour0_texture_attachment.object = 0;
	}

	void LookUpTextureGenerator::Free()
	{
		Assert(created);
		FreeShader(&shader);
		FreeFrameBuffer(&frame, true);
		created = false;
	}
}