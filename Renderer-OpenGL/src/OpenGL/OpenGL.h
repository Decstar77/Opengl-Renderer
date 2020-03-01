#pragma once
#include "Core.h"
#include "glew.h"
#include "BufferLayout.h"
#include <unordered_map>
namespace cm
{	
	//===============================================================//
	/*		
		@TODO: Change CreateShader to take in shader pointer
	*/
	//===============================================================//
	enum class VertexFlags : uint32
	{
		READ = GL_MAP_READ_BIT,
		WRITE = GL_MAP_WRITE_BIT,
		READ_WRITE = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT,
		READ_WRITE_COPY = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT

	};

	enum class BufferType : uint32
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

	struct OpenGLStatistics
	{
		std::string vendor;
		std::string renderer;
		std::string version;
		std::string shading_lang;
			   
		int32 work_grp_cnt[3];
		int32 work_grp_size[3];
		int32 work_grp_inv;
	};

	struct UniformBuffer
	{
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
		uint32 index_count = 0;
		BufferLayout lbo;
		BufferType type = BufferType::Array_buffer;
	};

	struct IndexBuffer
	{
		uint32 object = 0;
		uint32 index_count = 0;
		uint32 size_bytes = 0 ;
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
		uint32 type = GL_TEXTURE_CUBE_MAP;
		uint32 texture_format = GL_RGBA; // Or GL_RGBA32F for example // first
		uint32 pixel_format = GL_RGBA;	//second
		uint32 data_type = GL_FLOAT;    //GL_UNSIGNED_BYTE
		uint32 min_filter = GL_LINEAR;
		uint32 mag_filter = GL_LINEAR;
		uint32 wrap_s_mode = GL_CLAMP_TO_EDGE;
		uint32 wrap_t_mode = GL_CLAMP_TO_EDGE;
		uint32 wrap_r_mode = GL_CLAMP_TO_EDGE;
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

	struct RenderBuffer
	{
		uint32 object = 0;
		uint32 width = 0;
		uint32 height = 0;
		uint32 render_buffer_format = GL_DEPTH24_STENCIL8;
		uint32 render_buffer_attachment_type = GL_DEPTH_STENCIL_ATTACHMENT;
	};

	struct FrameBuffer
	{
		BufferType type = BufferType::Frame_buffer;
		uint32 object = 0;

		RenderBuffer render_attchment;
		Texture colour0_texture_attachment;
		Texture colour1_texture_attachment;
		Texture colour2_texture_attachment;
		Texture depth_texture_attachment;
	};
		
	struct Shader
	{
		uint32 shader_program;
		ShaderType type;
		std::string name;
		std::unordered_map<std::string, uint32> uniform_cache;
	};

	template<typename T>
	void CreateBuffer(T *buffer, uint32 buffer_size_bytes, VertexFlags flags)
	{
		Assert(buffer->object == 0);

		T buff;
		uint32 object;
		uint32 type = static_cast<uint32>(buff.type);
		glGenBuffers(1, &object);
		glBindBuffer(type, object);	

		glBufferStorage(type, buffer_size_bytes, NULL, static_cast<uint32>(flags));

		glBindBuffer(type, 0);

		buffer->object = object;		
		buffer->size_bytes = buffer_size_bytes;
	}

	template<typename T>	
	void BindBuffer(const T &buffer)
	{
		Assert(buffer.object != 0);
		glBindBuffer(static_cast<uint32>(buffer.type), buffer.object);
	}
	
	template<typename T>
	void UnbindBuffer(const T &buffer)
	{
		uint32 type = static_cast<uint32>(buffer.type);
		glBindBuffer(type, 0);
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

	//************************************
	// Uniform Buffer Functions
	//************************************

	void CreateUniformBuffer(UniformBuffer *buffer);

	void UniformBufferSetBindPoint(const UniformBuffer &ubo, uint32 binding_point);

	//************************************
	// Vertex Array Functions
	//************************************
	void BindVertexArray(const VertexArray &vao);
	
	void UnbindVertexArray();
	
	void FreeVertexArray(VertexArray *vao, bool clear_vbos);
	
	void CreateVertexArray(VertexArray *vao);
	
	BufferLayout GetTotalVertexBufferLayout(const VertexArray &vao);
	
	void VertexArrayAddBuffer(VertexArray *vao, VertexBuffer &vbo, BufferLayout &added_lbo);
	
	//************************************
	// Texture Functions
	//************************************
	
	void BindTexture(Texture *texture);

	void BindTexture(const Texture &texture);

	void UnbindTexure();

	void CreateTexture(Texture *texture, const void* data);

	void CopyTexture(Texture *src, Texture *dst);

	void TextureSetBorder(Texture *texture, float *border_colour);

	void FreeTexture(Texture *texture);
	
	void CreateCubeMap(CubeMap *cube_map, const void **data);

	void CreateCubeMapFrom6(CubeMap *cube_map, DynaArray<Texture> textures);

	//************************************
	// Shader Functions
	//************************************
	
	Shader CreateShader(std::string vertex_source, std::string fragment_source);
	
	Shader CreateComputeShader(std::string source);
	
	void FreeShader(Shader *shader);
	
	void BindShader(const Shader &shader);
	
	uint32 GetUniformLocation(Shader *shader, const std::string &uniform_name);
	
	void ShaderSetInt32(Shader &shader, const std::string &uniform_name, int x);
	
	void ShaderSetFloat(Shader &shader, const std::string &uniform_name, float x);
	
	void ShaderSetVec3(Shader &shader, const std::string &uniform_name, float x, float y, float z);
	
	void ShaderSetVec3(Shader &shader, const std::string &uniform_name, float* data);
	
	void ShaderSetMat4(Shader &shader, const std::string &uniform_name, float* data);
	
	void ShaderBindUniformBuffer(Shader &shader, uint32 binding_point, const std::string &uniform_name);
	
	void ShaderBindTexture(Shader &shader, Texture texture, uint8 texture_slot, const std::string &uniform_name);

	//************************************
	// FrameBuffer Functions
	//************************************
	
	void BindFrameBuffer(const FrameBuffer &fbo);
	
	void UnbindFrameBuffer();
	
	void FreeFrameBuffer(FrameBuffer *fbo);
	
	void CreateFrameBuffer(FrameBuffer *fbo);
	
	void FrameBufferAddColourAttachtments(FrameBuffer *buffer);
	
	void FrameBufferAddDepthAttachments(FrameBuffer *buffer);

	void FrameAddBufferRenderAttachtment(FrameBuffer *buffer);
	
	bool CheckFrameBuffer(const FrameBuffer &buffer); // Returns true if FrameBuffer is good 

	//************************************
	// Other Functions
	//************************************

	void GetOpenglStatistics(OpenGLStatistics *stats);

	void PrintOpenglStatistics(const OpenGLStatistics &stats);

}


