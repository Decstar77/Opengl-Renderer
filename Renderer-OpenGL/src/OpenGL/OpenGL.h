#pragma once
#include "Core.h"
#include "glew.h"
#include "BufferLayout.h"
#include "Math/CosmicMath.h"

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

	struct VertexBuffer
	{
		uint32 object = 0;
		uint32 size_bytes = 0;
		uint32 index_count = 0;
		BufferLayout lbo;
		VertexFlags flags = VertexFlags::READ_WRITE;
		BufferType type = BufferType::Array_buffer;
	};

	struct IndexBuffer
	{
		uint32 object = 0;
		uint32 index_count = 0;
		uint32 size_bytes = 0;
		VertexFlags flags = VertexFlags::READ_WRITE;
		BufferType type = BufferType::Index_buffer;
	};

	struct VertexArray
	{
		uint32 object = 0;
		std::vector<VertexBuffer> vertex_buffers;
	};

	struct TextureConfig
	{
		uint32 type = GL_TEXTURE_2D;
		uint32 texture_format = GL_RGBA;	// NOTE: Or GL_RGBA32F 
		uint32 pixel_format = GL_RGBA;		// NOTE: Or GL_RG
		uint32 data_type = GL_FLOAT;		// NOTE: Or GL_UNSIGNED_BYTE
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
		uint32 texture_format = GL_RGBA;	// NOTE: Or GL_RGBA32F  
		uint32 pixel_format = GL_RGBA;		// NOTE: Or GL_RG
		uint32 data_type = GL_FLOAT;		// NOTE: Or GL_UNSIGNED_BYTE
		uint32 min_filter = GL_LINEAR;
		uint32 mag_filter = GL_LINEAR;
		uint32 wrap_s_mode = GL_CLAMP_TO_EDGE;
		uint32 wrap_t_mode = GL_CLAMP_TO_EDGE;
		uint32 wrap_r_mode = GL_CLAMP_TO_EDGE;
		uint32 width = 0;
		uint32 height = 0;
		bool generate_mip_maps = false;
		std::string uniform_name = "";
	};

	struct ShaderConfig
	{
		ShaderType type;
		std::string name;
		std::string src_vert;	// NOTE: Gets freed and set to Linked when sucessfully created else emptry string
		std::string scr_frag;	// NOTE: Gets freed and set to Linked when sucessfully created else emptry string
		std::string src_geom;	// NOTE: Gets freed and set to Linked when sucessfully created else emptry string		
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
		std::unordered_map<std::string, uint32> uniform_cache;
		ShaderType type;
		std::string name;
	};

	struct GLMesh
	{
		VertexArray vao;
		IndexBuffer ibo;
	};

	struct Batch
	{
		VertexArray vao;
		IndexBuffer ibo;
		std::vector<Mat4> transforms;
	};

	//************************************
	// Usefull classes for rendering
	//************************************

	// NOTE: We could make getters for these to be safe, but I think it's a bit convaluted to do so
	class StandardMeshes
	{
	public:
		static GLMesh quad;
		static GLMesh plane;
		static GLMesh cube;
		static GLMesh sphere;
		static GLMesh cone;

		static void Initilize();
	};

	// NOTE: We could make getters for these to be safe, but I think it's a bit convaluted to do so
	// TODO: Complete as needed
	class OpenGlState
	{
	public:
		static uint32 current_viewport_width;
		static uint32 current_viewport_height;
		static uint32 window_width;
		static uint32 window_height;
	};
	
	class CubeMapMatrices
	{
	public:
		static Mat4 projection;
		static Mat4 views[6];
	};

	class CubeMapGenerator
	{
	private:
		Shader shader;
		FrameBuffer frame;
		bool created = false;

	public:
		void Create();
		void Convert(const Texture &src, CubeMap *dst);
		void Free();

	public:
		CubeMapGenerator();
		~CubeMapGenerator();
	};

	class EquirectangularGenerator
	{
	private:
		Shader shader;
		FrameBuffer frame;
		bool created = false;

	public:
		void Create();
		void Convert(const CubeMap &src, Texture *dst);
		void Free();

	public:
		EquirectangularGenerator();
		~EquirectangularGenerator();
	};

	class IrradianceGenerator
	{
	private:
		Shader shader;
		FrameBuffer frame;
		bool created = false;

	public:
		void Create();
		void Convert(const CubeMap &src, CubeMap *dst);
		void Free();

	public:
		IrradianceGenerator();
		~IrradianceGenerator();
	};

	class PrefilterGenerator
	{
	private:
		Shader shader;
		FrameBuffer frame;
		bool created = false;

	public:
		void Create();
		void Convert(const CubeMap &src, CubeMap *dst);
		void Free();

	public:
		PrefilterGenerator();
		~PrefilterGenerator();
	};

	class LookUpTextureGenerator
	{
	private:
		FrameBuffer frame;
		bool created = false;

	public:
		void Create();
		void Convert(Texture *dst);
		void Free();
		Shader shader;

	public:
		LookUpTextureGenerator();
		~LookUpTextureGenerator();

	};

	//************************************
	// Write Buffer Functions C++ Templated
	//************************************

	template <typename Q, typename T>
	void ReadBufferData(const Q &buffer, std::vector<T> *data, uint32 size_byte, uint32 offset_bytes)
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
	void WriteBufferData(Q *buffer, const std::vector<T>& data, uint32 offset_bytes)
	{
		Assert(buffer->object != 0);
		Assert(offset_bytes + data.size() * sizeof(T) <= buffer->size_bytes);

		uint32 type = static_cast<uint32>(buffer->type);

		glBindBuffer(type, buffer->object);

		void *ptr = glMapBuffer(type, GL_WRITE_ONLY);
		ptr = static_cast<char*>(ptr) + offset_bytes;

		const void * new_data = static_cast<const void *>(data.data());
		uint32 size = sizeof(T) * static_cast<uint32>(data.size());

		memcpy(ptr, new_data, size);


		glUnmapBuffer(type);
		glBindBuffer(type, 0);
	}

	template <typename Q>
	void ReadBufferData(const Q &buffer, void *data, uint32 size_byte, uint32 offset_bytes)
	{
		Assert(buffer.object != 0);
		size_byte = size_byte == 0 ? (buffer.size_bytes - offset_bytes) : size_byte;
		Assert(offset_bytes + size_byte <= buffer.size_bytes);
		Assert(0); // @REASON: Untested
		uint32 type = static_cast<uint32>(buffer.type);

		glBindBuffer(type, buffer.object);

		void *ptr = glMapBuffer(type, GL_READ_ONLY);
		ptr = static_cast<char*>(ptr) + offset_bytes;

		memcpy(data, ptr, size_byte);

		glUnmapBuffer(type);
		glBindBuffer(type, 0);
	}

	template <typename Q>
	void WriteBufferData(Q *buffer, void *data, uint32 size_bytes, uint32 offset_bytes)
	{
		Assert(buffer->object != 0);
		Assert(offset_bytes + size_bytes <= buffer->size_bytes);
		Assert(0); // @REASON: Untested
		uint32 type = static_cast<uint32>(buffer->type);

		glBindBuffer(type, buffer->object);

		void *ptr = glMapBuffer(type, GL_WRITE_ONLY);
		ptr = static_cast<char*>(ptr) + offset_bytes;

		memcpy(ptr, data, size_bytes);

		glUnmapBuffer(type);
		glBindBuffer(type, 0);
	}

	//************************************
	// Vertex Buffer Functions
	//************************************

	void CreateVertexBuffer(VertexBuffer *vbo);

	void BindVertexBuffer(const VertexBuffer &vbo);

	void UnbindVertexBuffer();

	void FreeVerteBuffer(VertexBuffer *vbo);

	//************************************
	// Index Buffer Functions
	//************************************

	void CreateIndexBuffer(IndexBuffer *ibo);

	void BindIndexBuffer(const IndexBuffer &ibo);

	void UnbindIndexBuffer();

	void FreeIndexBuffer(IndexBuffer *ibo);

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

	void CreateCubeMapFrom6(CubeMap *cube_map, std::vector<Texture> textures);

	//************************************
	// Shader Functions
	//************************************

	// @TODO: Prehaps we should store the source in the shader struct
	// @TODO: ShaderUniforms should be const float *data

	Shader CreateShader(std::string vertex_source, std::string fragment_source);

	Shader CreateComputeShader(std::string source);

	// @TODO: Complete
	//Shader CreateBatchShaderFromShader(std::string vertex_source, std::string fragment_source);


	void FreeShader(Shader *shader);

	void BindShader(const Shader &shader);

	uint32 GetUniformLocation(Shader *shader, const std::string &uniform_name);

	void ShaderSetInt32(Shader *shader, const std::string &uniform_name, int x);

	void ShaderSetFloat(Shader *shader, const std::string &uniform_name, float x);

	void ShaderSetVec3(Shader *shader, const std::string &uniform_name, float x, float y, float z);

	void ShaderSetVec3(Shader *shader, const std::string &uniform_name, float* data);

	void ShaderSetMat4(Shader *shader, const std::string &uniform_name, float* data);

	void ShaderBindUniformBuffer(const Shader &shader, uint32 binding_point, const std::string &uniform_name);

	// @TODO: Complete
	//void ShaderBindUniformBuffersFromSource(Shader &shader);

	void ShaderBindTexture(Shader &shader, const Texture &texture, uint8 texture_slot, const std::string &uniform_name);

	void ShaderBindCubeMap(Shader *shader, const CubeMap &cube_map, uint8 texture_slot, const std::string &uniform_name);

	//************************************
	// FrameBuffer Functions
	//************************************

	void BindFrameBuffer(const FrameBuffer &fbo);

	void UnbindFrameBuffer();

	void FreeFrameBuffer(FrameBuffer *fbo, bool attachments);

	void FreeRenderBuffer(RenderBuffer *rb);

	void CreateFrameBuffer(FrameBuffer *fbo);

	void FrameBufferBindColourAttachtments(FrameBuffer *buffer);

	void FrameBufferAddDepthAttachments(FrameBuffer *buffer);

	void FrameAddBufferRenderAttachtment(FrameBuffer *buffer);

	bool CheckFrameBuffer(const FrameBuffer &buffer); // Returns true if FrameBuffer is good 

	//************************************
	// Render Functions
	//************************************

	void CreateBatch(Batch *batch, const VertexBuffer &vbo_to_batch, const IndexBuffer &ibo_of_vbo);

	void FreeBatch(Batch *batch);

	//************************************
	// Render Functions
	//************************************

	void RenderBatch(const Shader &shader, const Batch &batch);

	void RenderMesh(const Shader &shader, const GLMesh &mesh);

	//************************************
	// Other Functions
	//************************************

	void GetOpenglStatistics(OpenGLStatistics *stats);

	void PrintOpenglStatistics(const OpenGLStatistics &stats);

	void InitializeOpenGl(uint32 window_width, uint32 window_height);

	//************************************
	// Inlined Opengl State functions
	//************************************

	static inline void EnableMulitSampling()
	{
		glEnable(GL_MULTISAMPLE);
	}

	static inline void Clear(Vec4 colour)
	{
		glClearColor(colour.x, colour.y, colour.z, colour.w);
	}

	static inline void EnableDepthBuffer()
	{
		glEnable(GL_DEPTH_TEST);
	}

	static inline void EnableCubeMapSeamless()
	{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	static inline void DepthBufferFunction(uint32 func)
	{
		glDepthFunc(func);
	}

	static inline void Enable(uint32 cmd)
	{
		glEnable(cmd);
	}

	static inline void EnableStencilBuffer()
	{
		glEnable(GL_STENCIL_TEST);
	}

	static inline void DisableDepthBuffer()
	{
		glDisable(GL_DEPTH_TEST);
	}

	static inline void EnableDepthBufferWriting()
	{
		glDepthMask(GL_FALSE);
	}

	static inline void DisableDepthBufferWriting()
	{
		glDepthMask(GL_FALSE);
	}

	static inline void DisbleStencilBuffer()
	{
		glDisable(GL_STENCIL_TEST);
	}

	static inline void ClearBuffers()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	static inline void ClearColourBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	static inline void ClearDepthBuffer()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	static inline void EnableFaceCulling()
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	}

	static inline void CullBackFace()
	{
		glCullFace(GL_BACK);
	}

	static inline void CullFrontFace()
	{
		glCullFace(GL_FRONT);
	}

	static inline void EnableWireframe()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	static inline void DisableBlending()
	{
		glDisable(GL_BLEND);
	}

	static inline void DisableFaceCulling()
	{
		glDisable(GL_CULL_FACE);
	}

	static inline void DisableWireframe()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	static inline void EnableBlending()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	static inline void ChangeViewPort(uint32 width, uint32 height)
	{
		glViewport(0, 0, width, height);
	}

	static inline void ChangeViewPort(uint32 start_x, uint32 start_y, uint32 width, uint32 height)
	{
		glViewport(start_x, start_y, width, height);
	}	
}


