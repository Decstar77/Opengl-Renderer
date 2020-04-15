#include "World.h"

namespace cm
{

	void RenderWorld(Shader *shader, Shader *batch_shader, const World &world)
	{

		//if (batch_shader != nullptr)
		//{
		//	for (int32 i = 0; i < world.batches.size(); i++)
		//	{
		//		RenderBatch(*batch_shader, world.batches[i]);
		//	}
		//}

		if (shader != nullptr)
		{
			BindShader(*shader);
			for (int32 i = 0; i < world.objects.size(); i++)
			{
				WorldObject* a = world.objects[i];
				GLMesh mesh = a->GetMeshForRender();
				Mat4 transform = a->GetTransformMatrix();

				ShaderSetMat4(shader, "model", transform.arr);
				RenderMesh(*shader, mesh);
			}
		}
	}

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

		ShaderSetMat4(&shader, "projection", projection.arr);
		ShaderBindTexture(shader, src, 0, "equirectangularMap");

		RenderCommands::ChangeViewPort(0, 0, dst->config.width, dst->config.height);
		
		for (uint32 i = 0; i < 6; ++i)
		{
			ShaderSetMat4(&shader, "view", views[i].arr);
						
			// @NOTE: A sneaky trick making a texture2d a cube map
			frame.colour0_texture_attachment.object = dst->object;
			frame.colour0_texture_attachment.config.type = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

			// @NOTE: Changing the state of the frame buffer means we have to rebind it 
			FrameBufferBindColourAttachtments(&frame);
			BindFrameBuffer(frame);

			RenderCommands::ClearColourBuffer();
			RenderCommands::ClearDepthBuffer();

			GLMesh cube = StandardMeshes::cube;
			RenderMesh(shader, cube);
		}

		RenderCommands::ChangeViewPort(0, 0, 1280, 720);

		// @NOTE: Make sure we dont try to free somthing that was part of the cube map
		frame.colour0_texture_attachment.object = 0;

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

		RenderCommands::ChangeViewPort(0, 0, dst->config.width, dst->config.height);

		// @NOTE: A sneaky trick making a texture2d a cube map
		frame.colour0_texture_attachment.object = dst->object;
		frame.colour0_texture_attachment.config.type = GL_TEXTURE_2D;

		// @NOTE: Changing the state of the frame buffer means we have to rebind it 
		FrameBufferBindColourAttachtments(&frame);

		Assert(CheckFrameBuffer(frame));

		BindFrameBuffer(frame);
		BindShader(shader);

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();

		ShaderBindCubeMap(&shader, src, 0, "cube_map");

		GLMesh screen_quad = StandardMeshes::quad;
		RenderMesh(shader, screen_quad);

		RenderCommands::ChangeViewPort(0, 0, 1280, 720);

		// @NOTE: Make sure we dont try to free somthing that was part of the cube map
		frame.colour0_texture_attachment.object = 0;

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

		ShaderSetMat4(&shader, "projection", projection.arr);
		ShaderBindCubeMap(&shader, src, 0, "environmentMap");

		RenderCommands::ChangeViewPort(0, 0, dst->config.width, dst->config.height);

		for (uint32 i = 0; i < 6; ++i)
		{
			ShaderSetMat4(&shader, "view", views[i].arr);

			// @NOTE: A sneaky trick making a texture2d a cube map
			frame.colour0_texture_attachment.object = dst->object;
			frame.colour0_texture_attachment.config.type = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

			// @NOTE: Changing the state of the frame buffer means we have to rebind it 
			FrameBufferBindColourAttachtments(&frame);
			BindFrameBuffer(frame);

			RenderCommands::ClearColourBuffer();
			RenderCommands::ClearDepthBuffer();

			GLMesh cube = StandardMeshes::cube;
			RenderMesh(shader, cube);
		}

		RenderCommands::ChangeViewPort(0, 0, 1280, 720);

		// @NOTE: Make sure we dont try to free somthing that was part of the cube map
		frame.colour0_texture_attachment.object = 0;

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

		ShaderSetMat4(&shader, "projection", projection.arr);
		ShaderBindCubeMap(&shader, src, 0, "environmentMap");
				
		const uint32 mip_levels = 5; 
		for (uint32 mip = 0; mip < mip_levels; ++mip)
		{
			uint32 mip_width = dst->config.width * std::pow(0.5, mip);
			uint32 mip_height = dst->config.height * std::pow(0.5, mip);
			real32 roughness = (real32)mip / (real32)(mip_levels- 1);
			
			RenderCommands::ChangeViewPort(0, 0, mip_width, mip_height);
			ShaderSetFloat(&shader, "roughness", roughness);
			for (uint32 i = 0; i < 6; i++)
			{
				ShaderSetMat4(&shader, "view", views[i].arr);

				frame.colour0_texture_attachment.object = dst->object;
				frame.colour0_texture_attachment.config.type = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

				// @NOTE: Changing the state of the frame buffer means we have to rebind it 
				//FrameBufferBindColourAttachtments(&frame);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dst->object, mip);


				RenderCommands::ClearColourBuffer();
				RenderCommands::ClearDepthBuffer();

				GLMesh cube = StandardMeshes::cube;
				RenderMesh(shader, cube);
			}
		}

		RenderCommands::ChangeViewPort(0, 0, 1280, 720);

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


		frame.colour0_texture_attachment.object = dst->object;
		FrameBufferBindColourAttachtments(&frame);

		BindShader(shader);
		BindFrameBuffer(frame);		

		RenderCommands::ClearColourBuffer();
		RenderCommands::ClearDepthBuffer();
		RenderCommands::ChangeViewPort(0, 0, dst->config.width, dst->config.height);

		RenderMesh(shader, StandardMeshes::quad);
		
		RenderCommands::ChangeViewPort(0, 0, 1280, 720);

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
