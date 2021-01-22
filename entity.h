#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "model.h"

struct Transform {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation_axis;
	float rotation_angle;
};

struct Material {
	glm::vec3 color;
	float alpha;
	float shininess;
	unsigned int environment_map;
};

class Entity
{
public:
	std::shared_ptr<Model>  model;
	std::shared_ptr<Shader> shader;
	Transform transform;
	Material material;
	Entity(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader, Transform transform, Material material) : model(model), shader(shader), transform(transform), material(material) {}
	virtual void update(float delta_time) {}
};

struct Camera {
	glm::vec3 pos;
	glm::vec3 up;
	glm::vec3 dir;
};

struct Direction_Light {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class Skybox 
{
public:
	unsigned int tex_id;
	unsigned int VAO, VBO;
	int vertex_count;
	std::shared_ptr<Shader> shader;
	Skybox(std::vector<float> vertices, std::vector<std::string> textures, std::shared_ptr<Shader> shader);
};

struct Scene {
	Camera camera;
	Direction_Light light;
	std::shared_ptr<Skybox> skybox;
	std::vector<std::shared_ptr<Entity>> entities;
};