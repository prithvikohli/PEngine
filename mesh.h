#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

struct Texture {
	unsigned int id;
	std::string type;	// "diffuse" or "specular"
	std::string path;	// file path of texture
};

class Mesh
{
private:
	unsigned int VBO, EBO;
	std::vector<Vertex> vertices;

public:
	unsigned int VAO;
	std::vector<unsigned int> indices;
	std::vector<Texture>	  textures;
	Mesh(std::vector<Vertex>  vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
};
