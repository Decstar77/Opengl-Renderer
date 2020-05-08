#pragma once
#include "Core.h"
#include "glew.h"
#include "Math/CosmicMath.h"
#include "Utility.h"
namespace cm
{
	//===============================================================//
	/*
		@NOTE: The depthstencil sturct is acutally an opengl render buffer
			 : just named it so because for clarity

		@TODO: Make every function assert that open is initilized!
	*/
	//===============================================================//

	//************************************
	// Enums 
	//************************************

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

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
	   
	//************************************
	// Opengl structures
	//************************************

	struct LayoutBuffer
	{
		uint32 stride = 0;
		uint32 current_offset = 0;
		uint32 current_next = 0;
		uint32 attribute_divisor = 0;

		Array<ShaderDataType> layout;

		LayoutBuffer();
		LayoutBuffer(Array<ShaderDataType> layout);
		~LayoutBuffer();
	};

	struct OpenGLStatistics
	{
		String vendor;
		String renderer;
		String version;
		String shading_lang;

		int32 work_grp_cnt[3];
		int32 work_grp_size[3];
		int32 work_grp_inv;
	};

	struct UniformBuffer
	{
		uint32 object = 0;
		uint32 binding_point = 0;		
		uint32 size_bytes = 0;
		BufferType type = BufferType::Uniform_buffer;
	};

	struct VertexBuffer
	{
		uint32 object = 0;
		uint32 size_bytes = 0;
		uint32 index_count = 0;
		LayoutBuffer lbo;
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
		bool32 mipmaps = false;
		String name = "";
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
		bool32 mipmaps = false;
		String uniform_name = "";
	};

	struct ShaderConfig
	{
		ShaderType type;
		String name = "";
		String src_vert = "";	// NOTE: Gets freed and set to "Linked" when sucessfully created else empty string
		String src_frag = "";	// NOTE: Gets freed and set to "Linked" when sucessfully created else empty string
		String src_geom = "";	// NOTE: Gets freed and set to "Linked" when sucessfully created else empty string		
	};

	struct GLMeshConfig
	{
		uint32 size_bytes = 0;		
		bool32 normals = true;
		bool32 texture_coords = true;
		bool32 tangent = true;
		bool32 bitanget = true;
		bool32 vertex_colour = false;
		bool32 vertex_bones = false;		
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

	struct DepthStencilBuffer
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

		DepthStencilBuffer depthstencil_attchment;

		Texture colour0_texture_attachment;
		Texture colour1_texture_attachment;
		Texture colour2_texture_attachment;

		Texture depth_texture_attachment;
	};

	struct Shader
	{
		uint32 shader_program;
		std::unordered_map<String, uint32> uniform_cache;

		ShaderConfig config;
		// @TODO: Remove these
		ShaderType type;
		String name;
	};

	struct GLMesh
	{
		GLMeshConfig config;
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
	void ReadBufferData(const Q &buffer, void *dst, uint32 size_byte, uint32 offset_bytes)
	{
		Assert(buffer.object != 0);
		size_byte = size_byte == 0 ? (buffer.size_bytes - offset_bytes) : size_byte;
		Assert(offset_bytes + size_byte <= buffer.size_bytes);

		uint32 type = static_cast<uint32>(buffer.type);

		glBindBuffer(type, buffer.object);

		void *ptr = glMapBuffer(type, GL_READ_ONLY);
		ptr = static_cast<char*>(ptr) + offset_bytes;

		memcpy(dst, ptr, size_byte);

		glUnmapBuffer(type);
		glBindBuffer(type, 0);
	}

	template <typename Q>
	void WriteBufferData(Q *buffer, void *data, uint32 size_bytes, uint32 offset_bytes)
	{
		Assert(buffer->object != 0);
		Assert(offset_bytes + size_bytes <= buffer->size_bytes);

		uint32 type = static_cast<uint32>(buffer->type);

		glBindBuffer(type, buffer->object);

		void *ptr = glMapBuffer(type, GL_WRITE_ONLY);
		ptr = static_cast<char*>(ptr) + offset_bytes;

		memcpy(ptr, data, size_bytes);

		glUnmapBuffer(type);
		glBindBuffer(type, 0);
	}

	//************************************
	// Layout Buffer Functions
	//************************************

	void LayoutBufferReset(LayoutBuffer *layout_buffer);
	
	void LayoutBufferNext(LayoutBuffer *layout_buffer);

	void LayoutBufferSetAttributeDivisor(LayoutBuffer *layout_buffer, const uint32 &div);
	
	uint32 LayoutBufferGetTotalAttributeCount(const LayoutBuffer &layout_buffer);
	
	uint32 LayoutBufferGetCurrentOffSet(const LayoutBuffer &layout_buffer);
	
	uint32 LayoutBufferGetCurrentSize(const LayoutBuffer &layout_buffer);
	
	uint32 LayoutBufferGetCurrentComponentAttribCount(const LayoutBuffer &layout_buffer);
	
	uint32 LayoutBufferGetComponentCount(const LayoutBuffer &layout_buffer);
	
	uint32 LayoutBufferGetStride(const LayoutBuffer &layout_buffer);
		
	uint32 LayoutBufferGetTotalSize(const LayoutBuffer &layout_buffer);
	
	uint32 LayoutBufferGetSizeOf(const LayoutBuffer &layout_buffer, const uint32 &index);
	
	uint32 LayoutBufferGetAttributeDivisor(const LayoutBuffer &layout_buffer);
	
	ShaderDataType LayoutBufferGetCurrentShaderType(const LayoutBuffer &layout_buffer);

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
	
	// @TODO: Complete
	//Shader CreateBatchShaderFromShader(String vertex_source, String fragment_source);
	
	void CreateShader(Shader *shader);
	
	Shader CreateComputeShader(String source);
	   
	void FreeShader(Shader *shader);

	void BindShader(const Shader &shader);

	void UnbindShader();

	uint32 GetUniformLocation(Shader *shader, const String &uniform_name);

	void ShaderSetBool(Shader *shader, const String &uniform_name, const bool &b);

	void ShaderSetInt32(Shader *shader, const String &uniform_name, int32 x);

	void ShaderSetFloat(Shader *shader, const String &uniform_name, float x);

	void ShaderSetVec3(Shader *shader, const String &uniform_name, float x, float y, float z);

	void ShaderSetVec3(Shader *shader, const String &uniform_name, float* data);

	void ShaderSetMat4(Shader *shader, const String &uniform_name, float* data);

	void ShaderBindUniformBuffer(const Shader &shader, uint32 binding_point, const String &uniform_name);

	// @TODO: Complete
	//void ShaderBindUniformBuffersFromSource(Shader &shader);

	void ShaderBindTexture(Shader &shader, const Texture &texture, uint32 texture_slot, const String &uniform_name);

	void ShaderBindCubeMap(Shader *shader, const CubeMap &cube_map, uint32 texture_slot, const String &uniform_name);

	//************************************
	// FrameBuffer Functions
	//************************************

	void BindFrameBuffer(const FrameBuffer &fbo);

	void BindFrameBuffer(const FrameBuffer *fbo);

	void UnbindFrameBuffer();

	void FreeFrameBuffer(FrameBuffer *fbo, bool attachments);

	void CreateDepthStencilBuffer(DepthStencilBuffer *rb);

	void FreeDepthStencilBuffer(DepthStencilBuffer *rb);

	void CreateFrameBuffer(FrameBuffer *fbo);

	void FrameBufferCopyDepth(const FrameBuffer &src, FrameBuffer *dst);

	void FrameBufferBindColourAttachtments(FrameBuffer *buffer);

	void FrameBufferBindDepthAttachment(FrameBuffer *buffer);

	void FrameBufferBindRenderAttachtment(FrameBuffer *buffer);

	bool CheckFrameBuffer(const FrameBuffer &buffer); 

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

	void RenderMesh(const Shader &shader, const GLMesh *mesh);

	//************************************
	// Other Functions
	//************************************

	uint32 GetShaderDataTypeComponentCount(ShaderDataType type);

	uint32 GetShaderDataTypeSize(ShaderDataType type);

	void GetOpenglStatistics(OpenGLStatistics *stats);

	void PrintOpenglStatistics(const OpenGLStatistics &stats);

	void InitializeOpenGl(uint32 window_width, uint32 window_height);

	//************************************
	// Common use functions
	//************************************
	
	void CreateGLMesh(GLMesh *mesh, LayoutBuffer lbo, void *vertex_data, uint32 vertex_size_bytes, void *index_data, uint32 index_size_bytes);
	
	void CreateFrameBufferGBuffer(FrameBuffer *g_buffer, const uint32 &width, const uint32 &height);

	void CreateFrameBufferColourOnly(FrameBuffer *c_buffer, const uint32 &width, const uint32 &height);

	void CreateFrameBufferDepthOnly(FrameBuffer *d_buffer, const uint32 &width, const uint32 &height);

	void CreateFrameBufferColourDepth(FrameBuffer *cd_buffer, const uint32 &width, const uint32 &height);

	//************************************
	// Usefull classes for rendering
	//************************************

	class StandardMeshes
	{
	private:
		static bool created;

	public:
		static GLMesh quad;
		static GLMesh plane;
		static GLMesh cube;
		static GLMesh sphere;
		static GLMesh cone;
		inline static bool IsInitilized() { return created; }
		inline static GLMesh Cube() {Assert(created); return cube; }
		inline static GLMesh Plane() {Assert(created); return plane; }
		inline static GLMesh ScreenSpaceQuad() {Assert(created); return quad; }

		static void Initilize();
	};

	class StandardTextures
	{
	private:
		static Texture zero_texture;
		static Texture one_texture;
		static Texture invalid_texture;
		static bool created;

	public:
		static inline const Texture GetZeroTexture() { Assert(created); return zero_texture; }
		static inline const Texture GetOneTexture() { Assert(created); return one_texture; }
		static inline const Texture GetInvalidTexture() { Assert(created); return invalid_texture; }
		static void Initilize();
	};

	class OpenGlState
	{
		// NOTE: We could make getters for these to be safe, but I think it's a bit convaluted to do so
		// TODO: Complete as needed
	public:
		static bool32 is_initilized;
		static uint32 current_viewport_width;
		static uint32 current_viewport_height;
		static uint32 window_width;
		static uint32 window_height;
	};
	class SampleTextureBlur
	{
	private:
		FrameBuffer frame;
		bool created = false;

	public:
		Shader shader;

		uint32 sampling_texture_count;
		Texture *sampling_textures;


		void Create(uint32 src_width, uint32 src_height, uint32 sampling_count);
		void Blur(const Texture src, Texture *dst);
		void Free();
	};

	class SimpleTextureBlur
	{
		// @NOTE: A fast blur for small kernel sizes
		//		: Not as good as guassian blur but cheaper
		//		: Get expensive for larger kernel sizes O(n^2)

	private:
		FrameBuffer frame;
		Shader shader;
		bool created = false;

	public:
		uint32 kernel_size;

		void Create(uint32 src_width, uint32 src_height, uint32 kernel_size);
		void Blur(const Texture &src, Texture *dst);
		void Free();


	public:
		SimpleTextureBlur();
		~SimpleTextureBlur();

	};

	class GaussianTextureBlur
	{
		// @NOTE: A good blur but more expensive than simple blur
		//		: If a very good large blur is needed then this is best

		// @TODO: Vaiable kernel_size by calcing the values ourselfs
	private:
		bool created = false;
		uint32 max_kernel_size = 20;
		uint32 kernel_size = 0;
		uint32 iterations = 0;
		real32 downsample_mul = 0;

		FrameBuffer *vertical_frames;
		FrameBuffer *horizontal_frames;

		Shader vertical_shader;
		Shader horizontal_shader;
		Shader upsample_shader;

	public:
		void Create(uint32 src_width, uint32 src_height, uint32 kernel_size = 11, real32 downsample_mul = 1, uint32 iterrations = 1);
		void Blur(const Texture &src, Texture *dst);
		void Free();

		void SetKernelSize(uint32 ksize);
		int32 GetKernelSize();
		int32 GetMaxKernelSize();

	public:
		GaussianTextureBlur();
		~GaussianTextureBlur();

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
		Shader shader;
		FrameBuffer frame;
		bool created = false;

	public:
		void Create();
		void Convert(Texture *dst);
		void Free();

	public:
		LookUpTextureGenerator();
		~LookUpTextureGenerator();

	};

	class LuminanceFilter
	{
	private:
		Shader shader;
		FrameBuffer frame;
		bool created = false;
		real32 threshold = 0;

	public:
		void Create(uint32 src_width, uint32 src_height, real32 threshold);
		void Filter(const Texture &src, Texture *dst);
		void Free();
		inline void SetThreshold(real32 t) { threshold = t; }

	public:
		LuminanceFilter();
		~LuminanceFilter();
	};

	class HemisphereKernel
	{
	private:
		bool created = false;
		uint32 kernel_size;
		Vec3f *kernel_samples;

	public:
		void Create(uint32 kernel_size, uint32 noise_texture_size);
		void Free();

		Texture noise_texture;


	public:
		HemisphereKernel();
		~HemisphereKernel();

		inline uint32 GetKernelSize() const { return kernel_size; }
		inline Vec3f GetKernelSample(uint32 index) const { Assert(index >= 0 && index < kernel_size); return kernel_samples[index]; }
		void SetKernelSize(uint32 kernel_size); 
	};
	   	 
	//************************************
	// Inlined Opengl State functions
	//************************************

	static inline void EnableMulitSampling()
	{
		glEnable(GL_MULTISAMPLE);
	}

	static inline void SetClearColour(Vec4f colour)
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

	static inline void DisableStencilBuffer()
	{
		glDisable(GL_STENCIL_TEST);
	}

	static inline void StencilOperationKKR()
	{
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

	static inline void StencilZeroMask()
	{
		glStencilMask(0x00);
	}

	static inline void StencilWriteZero()
	{
		Assert(0); // @REASON: Incomplete
	}

	static inline void StencilWriteOnes()
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}

	static inline void StencilDiscardOnOne()
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
	}

	static inline void StencilDiscardOnZero()
	{
		Assert(0); // @REASON: Incomplete
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

	static inline void ClearAllBuffers()
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
	}

	static inline void BlendFunctionOneMinusSrcAlpha()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	static inline void SetViewPort(uint32 width, uint32 height)
	{
		OpenGlState::current_viewport_width = width;
		OpenGlState::current_viewport_height = height;
		glViewport(0, 0, width, height);
	}

	static inline void SetViewPort(uint32 start_x, uint32 start_y, uint32 width, uint32 height)
	{
		OpenGlState::current_viewport_width = width;
		OpenGlState::current_viewport_height = height;
		glViewport(start_x, start_y, width, height);
	}	
	   	  












}


