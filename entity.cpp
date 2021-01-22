#include "entity.h"

Skybox::Skybox(std::vector<float> vertices, std::vector<std::string> textures, std::shared_ptr<Shader> shader)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	vertex_count = vertices.size();
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	tex_id = load_cubemap(textures);
	this->shader = shader;
}