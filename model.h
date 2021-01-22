#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "mesh.h"

class Model
{
private:
	std::vector<Texture> textures_loaded;
	std::string			 directory;

	void process_node(aiNode* node, const aiScene* scene);
	Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name);
public:
	std::vector<Mesh> meshes;
	Model(std::string path);
};

unsigned int load_cubemap(std::vector<std::string> textures);