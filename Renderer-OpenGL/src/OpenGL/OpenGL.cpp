#include "OpenGL.h"



namespace cm
{

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
		glDeleteTextures(1, &texture->object);
		texture->config.width = 0;
		texture->config.height = 0;		
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


		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		cube_map->object = obj;
	}

	void CreateCubeMapFrom6(CubeMap *cubemap, DynaArray<Texture> textures)
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


	void BindFrameBuffer(const FrameBuffer &fbo)
	{
		Assert(fbo.object != 0) // @NOTE: Open-GL reserves 0 for the defualt framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo.object);
	}

	void UnbindFrameBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FreeFrameBuffer(FrameBuffer *fbo)
	{
		Assert(0); //@TODO: We need to free the colour buffers etc
		if (fbo->object != 0)
		{
			glDeleteFramebuffers(1, &fbo->object);
			fbo->object = 0;
		}
	}

	void CreateFrameBuffer(FrameBuffer *fbo)
	{
		Assert(fbo->object == 0);
		glGenFramebuffers(1, &fbo->object);
	}

	void FrameBufferAddColourAttachtments(FrameBuffer *buffer)
	{
		Assert(buffer->object != 0);
		BindFrameBuffer(*buffer);
		Texture *texture = &buffer->colour0_texture_attachment;

		uint32 count = 0;
		uint32 attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
								GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

		for (int32 i = 0; i < 3; i++)
		{
			if (texture[i].object != 0)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
					texture[i].object, 0);
				glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
				count++;
			}
			else
			{
				break;
			}
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
		DynaArray<float> current_mesh_data;
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

		DynaArray<uint32> index_data;
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




}