#include "OpenGL.h"



namespace cm
{
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
			BindBuffer(current_vbo);
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
			for (uint32 i = 0; i < vbo_count; i++)
			{
				FreeBuffer(&vao->vertex_buffers[i]); // Free the GPU memory
			}
			vao->vertex_buffers.clear();

			glDeleteVertexArrays(1, &vao->object);
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


	void CreateTexture(Texture *texture, const void* data)
	{
		uint32 text;

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


	void BindFrameBuffer(const FrameBuffer &fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo.object);
	}

	void UnbindFrameBuffer(const FrameBuffer &fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FreeFrameBuffer(FrameBuffer *fbo)
	{
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

		glRenderbufferStorage(GL_RENDERBUFFER, buffer->config.render_buffer_format, width, height);

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


	void PrintComputeShaderStats()
	{
		int work_grp_cnt[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
		printf("max global (total) work group size x:%i y:%i z:%i\n",
			work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

		int work_grp_size[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
		printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
			work_grp_size[0], work_grp_size[1], work_grp_size[2]);

		int work_grp_inv;
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
		printf("max local work group invocations %i\n", work_grp_inv);
	}




}