#include "AssetLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/std_image.h"
namespace cm
{
	std::string ReadFile(const std::string &file_directory)
	{
		std::ifstream file;
		file.open(file_directory);
		Assert(file.is_open());

		std::stringstream ss;
		ss << file.rdbuf();
		file.close();

		return ss.str();
	}

	cm::FileDataTexture<uint8> LoadTexture(const std::string &file_directory)
	{
		int32 width;
		int32 height;
		int32 nrChannels;
		//stbi_set_flip_vertically_on_load(true);
		uint8 * t_data = stbi_load(file_directory.c_str(), &width, &height, &nrChannels, 0);
		Assert(t_data != nullptr);
		uint32 size = width * height * nrChannels * sizeof(uint8);

		LOG("Size bytes %f", (float)size * BYTES_TO_MEGABYTES);
		FileDataTexture<uint8> data;

		data.width = width;
		data.height = height;
		data.channel = nrChannels;

		data.data.CopyFromPtr(t_data, size);
		stbi_image_free(t_data);
		return data;
	}

	cm::FileDataTexture<float> LoadHDRI(const std::string &file_directory)
	{
		int32 width;
		int32 height;
		int32 nrChannels;
		stbi_set_flip_vertically_on_load(true);
		float * t_data = stbi_loadf(file_directory.c_str(), &width, &height, &nrChannels, 0);
		Assert(t_data != nullptr);
		uint32 size = width * height * nrChannels * sizeof(float);

		LOG("Size bytes %f", (float)size * BYTES_TO_MEGABYTES);
		FileDataTexture<float> data;

		data.width = width;
		data.height = height;
		data.channel = nrChannels;

		data.data.insert(data.data.end(), &t_data[0], &t_data[size / sizeof(float)]);
		stbi_image_free(t_data);
		return data;		
	}

	void ParseObj(const std::string &file_directory, DynaArray<Vec3> *points, DynaArray<Vec3> *normals, DynaArray<Vec3> *texture_coords, DynaArray<uint32> *layout)
	{
		std::ifstream file;
		file.open(file_directory);

		Assert(file.is_open());

		std::stringstream file_stream;

		file_stream << file.rdbuf();

		file.close();
		std::string line;
		while (getline(file_stream, line))
		{
			if (line.substr(0, 2) == "v ")
			{
				std::istringstream s(line.substr(2));
				Vec3 v; s >> v.x; s >> v.y; s >> v.z;
				points->push_back(v);
			}
			else if (line.substr(0, 2) == "vn ")
			{
				std::istringstream s(line.substr(2));
				Vec3 v; s >> v.x; s >> v.y; s >> v.z;
				normals->push_back(v);
			}
			else if (line.substr(0, 2) == "vt ")
			{
				std::istringstream s(line.substr(2));
				Vec3 v; s >> v.x; s >> v.y; s >> v.z;
				texture_coords->push_back(v);
			}
			else if (line.substr(0, 2) == "f ")
			{
				std::string face = line.substr(2);
				std::replace(face.begin(), face.end(), ' ', '/');
				std::string other;
				std::istringstream ss(face);
				for (int32 i = 0; i < 8; i++)
				{
					uint32 data = 0;
					ss >> data;
					data--;
					layout->push_back(data);
					std::getline(ss, other, '/');
				}
			}
			else if (line[0] == '#')
			{
				/* ignoring this line */
			}
			else
			{
				/* ignoring this line */
			}
		}

	}

	cm::FileDataMesh LoadMesh(const std::string &file_directory, bool tangents)
	{

		//: Open file and read everything once in a stringstream
		std::ifstream file;
		file.open(file_directory);

		Assert(file.is_open());

		std::stringstream file_stream;

		file_stream << file.rdbuf();

		file.close();

		//:Parse the string data into float and uint32 arrays
		std::string line;
		std::string name;
		DynaArray<float> points;
		DynaArray<float> t_coords;
		DynaArray<float> normals;
		DynaArray<uint32> vertex_data;

		while (!file_stream.eof())
		{
			std::getline(file_stream, line);

			if (line == "")
			{
				continue;
			}

			if (line[0] != '#')
			{
				if (line[0] != 'o')
				{
					if (line[0] == 'v' && line[1] == ' ')
					{
						ParseLineObj(points, line, ' ');
					}
					else if (line[1] == 't')
					{
						ParseLineObj(t_coords, line, ' ');
					}
					else if (line[1] == 'n')
					{
						ParseLineObj(normals, line, ' ');
					}
					else if (line[0] == 's')
					{

					}
					else if (line[0] == 'f')
					{
						//Remove the f and space;
						line.erase(0, 2);
						for (int i = 0; i < line.length(); i++)
						{
							if (line[i] == ' ')
							{
								line[i] = '/';
							}
						}

						line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
						ParseLineObj(vertex_data, line, '/');
					}
				}
				else
				{
					name = line.substr(2, line.length() - 2);
				}
			}
			else
			{
				continue;
			}
		}

		LOG("PARESED FILE");
		uint32 face_vertex_index[] = { 0,0,0 };
		Vec3 face_point_positions[] = { 0,0,0 };
		Vec2 face_t_coords[] = { 0,0,0 };

		uint32 face_counter = 0;

		const uint32 stride = 3 + 3 + 2 + 3 + 3;
		const uint32 tangent_stride_offset = 3 + 3 + 2;
		const uint32 bitangent_stride_offset = 3 + 3 + 2 + 3;

		Vec3 min = Vec3(1000000);
		Vec3 max = Vec3(-1000000);
		//: Construct all possible vertices
		uint32 all_vert_counter = 0;
		DynaArray<float> all_vertices;

		std::unordered_map<uint32, uint32> combinations;
		uint32 index_inc = 0;
		uint32 index_counter = 0;

		DynaArray<uint32> indices;
		for (uint32 i = 0; i < vertex_data.size(); i += 3) // Once per vertex in face
		{
			std::stringstream ss;
			ss << vertex_data.at(i) << vertex_data.at(i + 1) << vertex_data.at(i + 2);
			uint32 combination = std::hash_value(ss.str());

			uint32 vert_index = (vertex_data.at(i) - 1) * 3;
			uint32 vert_text = (vertex_data.at(i + 1) - 1) * 2;
			uint32 vert_norm = (vertex_data.at(i + 2) - 1) * 3;

			Vec3 position = Vec3(points.at(vert_index), points.at(vert_index + 1), points.at(vert_index + 2));
			Vec3 normal = Vec3(normals.at(vert_norm), normals.at(vert_norm + 1), normals.at(vert_norm + 2));
			Vec2 t_coord = Vec2(t_coords.at(vert_text), 1 - t_coords.at(vert_text + 1));



			min.x = fmin(position.x, min.x);
			min.y = fmin(position.y, min.y);
			min.z = fmin(position.z, min.z);


			max.x = fmax(position.x, max.x);
			max.y = fmax(position.y, max.y);
			max.z = fmax(position.z, max.z);



			if (tangents)
			{
				face_point_positions[face_counter] = position;
				face_t_coords[face_counter] = t_coord;
			}
			if (combinations[combination] != 0)
			{
				indices.push_back(combinations[combination] - 1);
				if (tangents)
				{
					face_vertex_index[face_counter] = (combinations[combination] - 1) * stride + tangent_stride_offset;
				}
			}
			else
			{
				if (tangents)
				{
					face_vertex_index[face_counter] = (uint32)all_vertices.size() + tangent_stride_offset;
				}


				//Position
				all_vertices.push_back(position.x);
				all_vertices.push_back(position.y);
				all_vertices.push_back(position.z);
				//Normal
				all_vertices.push_back(normal.x);
				all_vertices.push_back(normal.y);
				all_vertices.push_back(normal.z);
				//Texture coord
				all_vertices.push_back(t_coord.x);
				all_vertices.push_back(t_coord.y);
				if (tangents)
				{
					//Tangent
					all_vertices.push_back(0);
					all_vertices.push_back(0);
					all_vertices.push_back(0);
					//Bitangent
					all_vertices.push_back(0);
					all_vertices.push_back(0);
					all_vertices.push_back(0);
				}
				indices.push_back(index_counter);
				combinations[combination] = index_counter + 1;
				index_counter++;
			}


			if (face_counter++ == 2 && tangents) //@Speed this is a bad branch
			{
				Vec3 edge1 = face_point_positions[1] - face_point_positions[0];
				Vec3 edge2 = face_point_positions[2] - face_point_positions[0];

				Vec2 deltaUV1 = face_t_coords[1] - face_t_coords[0];
				Vec2 deltaUV2 = face_t_coords[2] - face_t_coords[0];

				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

				Vec3 tangent(1);
				Vec3 bitangent(1);

				tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
				tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
				tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
				tangent = Normalize(tangent);

				bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
				bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
				bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
				bitangent = Normalize(bitangent);

				for (int ii = 0; ii < 3; ii++)
				{
					all_vertices.at(face_vertex_index[ii]) = tangent.x;
					all_vertices.at(face_vertex_index[ii] + 1) = tangent.y;
					all_vertices.at(face_vertex_index[ii] + 2) = tangent.z;

					all_vertices.at(face_vertex_index[ii] + 3) = bitangent.x;
					all_vertices.at(face_vertex_index[ii] + 4) = bitangent.y;
					all_vertices.at(face_vertex_index[ii] + 5) = bitangent.z;
				}
				face_vertex_index[0] = 0;
				face_vertex_index[1] = 0;
				face_vertex_index[2] = 0;
				face_counter = 0;
			}
		}

		//: Log the loaded model
		LOG("Total Mbs: %f", all_vertices.size() * sizeof(float) * BYTES_TO_MEGABYTES);
		LOG("Final Vertex Count: %f", (float)all_vertices.size() / (float)stride);
		LOG("Final Tri    Count: %f", (float)indices.size() / 3.f);

		FileDataMesh mesh;
		mesh.indices = indices;
		mesh.data = all_vertices;
		mesh.size_bytes = all_vertices.size() * sizeof(float);
		mesh.min_vertex = min;
		mesh.max_vertex = max;
		return mesh;
	}

}

