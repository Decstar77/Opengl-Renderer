#include "Rendering Stuffies/Texture.h"

namespace cm
{
	uint32 Texture::texture_id_counter = 0;

	Texture::Texture() :
		uniform_location(-1), active(false),
		load_number(static_cast<uint32>(0)), id(0)
	{
	}


	Texture::~Texture()
	{
	}

	Texture::Texture(const TextureConfig &config) : config(config),
		uniform_location(-1), active(true), load_number(static_cast<uint32>(0))
	{
		uint32 text;

		glGenTextures(1, &text);
		glBindTexture(config.type, text);

		if (config.type == GL_TEXTURE_CUBE_MAP)
		{
			for (int32 i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, config.texture_format,
					config.width, config.height, 0, config.pixel_format, config.data_type, NULL);
			}
		}
		else
		{
			glTexImage2D(config.type, 0, config.texture_format, config.width,
				config.height, 0, config.pixel_format, config.data_type, NULL);
		}

		glTexParameteri(config.type, GL_TEXTURE_MIN_FILTER, config.min_filter);
		glTexParameteri(config.type, GL_TEXTURE_MAG_FILTER, config.mag_filter);
		glTexParameteri(config.type, GL_TEXTURE_WRAP_S, config.wrap_s_mode);
		glTexParameteri(config.type, GL_TEXTURE_WRAP_T, config.wrap_t_mode);
		glTexParameteri(config.type, GL_TEXTURE_WRAP_R, config.wrap_r_mode);


		glBindTexture(config.type, 0);

		this->id = text;

	}


	Texture::Texture(const TextureConfig& config, std::vector<uint8> &data) : config(config),
		uniform_location(-1),  active(true), load_number(static_cast<uint32>(texture_id_counter++))
	{
		Assert(config.width != 0); 
		Assert(config.height != 0);
		
		
		//glGenTextures(1, &text);
		//glBindTexture(GL_TEXTURE_2D, text);
		////RELEARN
		////Make x-y repeat
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		////Doing this for the alpha test tut
		////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		////RELEARN
		////Use linear min and mag instead of GL_NEAREST
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config.width, config.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
		//glGenerateMipmap(GL_TEXTURE_2D);

		//glBindTexture(GL_TEXTURE_2D, 0);


		uint32 text;

		glGenTextures(1, &text);
		glBindTexture(config.type, text);


		glTexParameteri(config.type, GL_TEXTURE_MIN_FILTER, config.min_filter);
		glTexParameteri(config.type, GL_TEXTURE_MAG_FILTER, config.mag_filter);
		glTexParameteri(config.type, GL_TEXTURE_WRAP_S, config.wrap_s_mode);
		glTexParameteri(config.type, GL_TEXTURE_WRAP_T, config.wrap_t_mode);
		glTexParameteri(config.type, GL_TEXTURE_WRAP_R, config.wrap_r_mode);


		if (config.type == GL_TEXTURE_CUBE_MAP)
		{
			for (int32 i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, config.texture_format,
					config.width, config.height, 0, config.texture_format, GL_UNSIGNED_BYTE, data.data());
			}
		}
		else
		{
			glTexImage2D(config.type, 0, config.texture_format, config.width,
				config.height, 0, config.texture_format, config.data_type, data.data());

			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		}

		glBindTexture(config.type, 0);

		this->id = text;

	}



}