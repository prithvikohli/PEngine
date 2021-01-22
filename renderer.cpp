#include <iostream>
#include <map>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* Renderer::window = nullptr;		// have to define this here since it's a static member variable

bool Renderer::setup(int width, int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "PEngine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "!!! ERROR: failed to create GLFW window\n";
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "!!! ERROR: failed to initalize GLAD\n";
		return false;
	}

	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void Renderer::render(const Scene& scene, bool transparent_pass)
{
	Camera cam = scene.camera;
	glm::mat4 view(1.0f);
	view = glm::lookAt(cam.pos, cam.pos + cam.dir, cam.up);
	glm::mat4 projection = glm::perspective(glm::radians(70.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	if (!transparent_pass)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (scene.skybox != nullptr)
		{
			glDepthMask(GL_FALSE);
			scene.skybox->shader->use();
			glm::mat4 skybox_view = glm::mat4(glm::mat3(view));	// we don't want translation  part of view transformation so skybox doesn't move as camera moves (always stays centered on camera)
			scene.skybox->shader->set_mat4("view", view);
			scene.skybox->shader->set_mat4("projection", projection);
			scene.skybox->shader->set_int("skybox", 0);

			glBindVertexArray(scene.skybox->VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, scene.skybox->tex_id);
			glDrawArrays(GL_TRIANGLES, 0, scene.skybox->vertex_count);
			glDepthMask(GL_TRUE);
		}
	}
	
	// transparent entities to render after opaque ones (provided this isn't a transparent pass)
	std::map<float, std::shared_ptr<Entity>> transparent_entities;

	for (std::shared_ptr<Entity> e : scene.entities)
	{
		// if entity is at all transparent, add to transparent_entities and skip rendering it for now so we can render all opaque entities first (provided this isn't a transparent pass)
		if (!transparent_pass && e->material.alpha < 1.0f)
		{
			float distance = glm::length(e->transform.position - scene.camera.pos);
			transparent_entities[distance] = e;
			continue;
		}

		e->shader->use();

		for (Mesh m : e->model->meshes)
		{
			// configure textures
			// ------------------
			unsigned int i = 0;
			if (e->material.environment_map != -1)	// if entity's material has an associated environment map
			{
				e->shader->set_int("environment_map", 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, e->material.environment_map);
				++i;
			}
			unsigned int diffuse = 0;
			unsigned int specular = 0;
			for (i; i < m.textures.size(); ++i)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				std::string type = m.textures[i].type;
				std::string number;
				if (type == "diffuse")
					number = std::to_string(diffuse++);
				else if (type == "specular")
					number = std::to_string(specular++);

				e->shader->set_int(("material." + type + number).c_str(), i);
				glBindTexture(GL_TEXTURE_2D, m.textures[i].id);
			}

			// configure material, lights and camera
			// ---------------------------
			e->shader->set_vec3("material.color", e->material.color);
			e->shader->set_float("material.alpha", e->material.alpha);
			e->shader->set_float("material.shininess", e->material.shininess);

			Direction_Light light = scene.light;
			e->shader->set_vec3("dir_light.direction", light.direction);
			e->shader->set_vec3("dir_light.ambient", light.ambient);
			e->shader->set_vec3("dir_light.diffuse", light.diffuse);
			e->shader->set_vec3("dir_light.specular", light.specular);

			e->shader->set_vec3("view_pos", cam.pos);

			// configure transform matrices
			// ----------------------------
			glm::mat4 model(1.0f);
			model = glm::translate(model, e->transform.position);
			model = glm::rotate(model, glm::radians(e->transform.rotation_angle), e->transform.rotation_axis);
			model = glm::scale(model, e->transform.scale);

			e->shader->set_mat4("model", model);
			e->shader->set_mat4("view", view);
			e->shader->set_mat4("projection", projection);

			// draw
			// ----
			glBindVertexArray(m.VAO);
			glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}
	}

	if (!transparent_pass && transparent_entities.size() > 0)
	{
		// construct scene containing only the transparent entities we haven't rendered yet
		Scene transparent_scene;
		transparent_scene.camera = scene.camera;
		transparent_scene.light = scene.light;

		// add transparent entities to scene in order of most distant to least distant, so distant transparent entities are rendered first in the transparent pass
		for (std::map<float, std::shared_ptr<Entity>>::reverse_iterator i = transparent_entities.rbegin(); i != transparent_entities.rend(); ++i)
			transparent_scene.entities.push_back(i->second);

		render(transparent_scene, true);
		return;
	}
	
	glfwSwapBuffers(window);
	glfwPollEvents();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}