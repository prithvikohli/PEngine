#include <iostream>
#include <glad/glad.h>
#include "model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int texture_from_file(const char* name, std::string dir);

Model::Model(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "!!! ERROR: " << importer.GetErrorString() << "\n";
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene)
{
	for (int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(process_mesh(mesh, scene));
	}

	for (int j = 0; j < node->mNumChildren; ++j)
		process_node(node->mChildren[j], scene);
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex>		   vertices;
	std::vector<Texture>	   textures;
	std::vector<unsigned int>  indices;

	for (int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		aiVector3D position = mesh->mVertices[i];
		vertex.pos.x = position.x;
		vertex.pos.y = position.y;
		vertex.pos.z = position.z;

		aiVector3D normal = mesh->mNormals[i];
		vertex.normal.x = normal.x;
		vertex.normal.y = normal.y;
		vertex.normal.z = normal.z;

		if (mesh->mTextureCoords[0])
		{
			aiVector3D tex_coords = mesh->mTextureCoords[0][i];
			vertex.tex_coords.x = tex_coords.x;
			vertex.tex_coords.y = tex_coords.y;
		}
		else
			vertex.tex_coords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (int j = 0; j < mesh->mNumFaces; ++j)
	{
		aiFace face = mesh->mFaces[j];
		for (int k = 0; k < face.mNumIndices; ++k)
			indices.push_back(face.mIndices[k]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		std::vector<Texture> diffuse_maps  = load_material_textures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE,  "diffuse");
		std::vector<Texture> specular_maps = load_material_textures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(),  diffuse_maps.begin(),  diffuse_maps.end());
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name)
{
	std::vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString name;
		mat->GetTexture(type, i, &name);
		
		bool skip = false;
		for (int j = 0; j < textures_loaded.size(); ++j)	// keep track of loaded textures so we don't have to reload the same texture
		{
			if (strcmp(textures_loaded[j].path.c_str(), name.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;
			texture.id = texture_from_file(name.C_Str(), directory);
			texture.type = std::string(type_name);
			texture.path = name.C_Str();

			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}

	return textures;
}

unsigned int texture_from_file(const char* name, std::string dir)
{
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	int width, height, nr_channels;
	stbi_set_flip_vertically_on_load(true);
	std::string path = dir + "/" + std::string(name);

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "!!! ERROR: failed to load texture" << std::endl;
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

unsigned int load_cubemap(std::vector<std::string> textures)
{
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		unsigned char* data = stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			std::cout << "!!! ERROR: cubemap texture failed to load at " << textures[i] << std::endl;
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return tex;
}
