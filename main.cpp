#include <iostream>
#include "renderer.h"

class Dragon : public Entity
{
public:
	Dragon(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader, Transform transform, Material material) : Entity(model, shader, transform, material) {}
	virtual void update(float delta_time)
	{
		transform.rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
		transform.rotation_angle += 20 * delta_time;	// continually rotate around y axis
	}
};

int main()
{
	if (!Renderer::setup(2560, 1440))
		return -1;

	// setup scene
	// -----------
	Direction_Light dir_light{
		glm::vec3(-0.2f, -1.0f, -0.3f),
		glm::vec3( 0.3f, 0.24f, 0.14f),
		glm::vec3( 0.7f, 0.42f, 0.26f),
		glm::vec3( 0.5f,  0.5f, 0.5f)
	};

	Camera camera{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)
	};

	std::vector<float> skybox_verts {        
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	std::vector<std::string> skybox_textures { "textures/right.jpg", "textures/left.jpg", "textures/top.jpg", "textures/bottom.jpg", "textures/front.jpg", "textures/back.jpg" };
	std::shared_ptr<Shader> skybox_shader = std::make_shared<Shader>("shaders/skyboxvert.sh", "shaders/skyboxfrag.sh");
    std::shared_ptr<Skybox> skybox = std::make_shared<Skybox>(skybox_verts, skybox_textures, skybox_shader);

	std::shared_ptr<Shader> phong = std::make_shared<Shader>("shaders/vert.sh", "shaders/frag.sh");
	std::shared_ptr<Shader> envmap = std::make_shared<Shader>("shaders/vert.sh", "shaders/envmapfrag.sh");
	std::shared_ptr<Model> dragon_model = std::make_shared<Model>("models/dragon_vrip.ply");

	Material dragon_mat;
	dragon_mat.color = glm::vec3(0.4f, 0.2f, 0.2f);
	dragon_mat.alpha = 1.0f;
	dragon_mat.shininess = 32.0f;
	dragon_mat.environment_map = skybox->tex_id;

	Transform dragon_trans;
	dragon_trans.position = glm::vec3(0.0f, -2.5f, -4.5f);
	dragon_trans.scale = glm::vec3(20.0f, 20.0f, 20.0f);
	dragon_trans.rotation_angle = 20.0f;
	dragon_trans.rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);

	std::shared_ptr<Dragon> dragon = std::make_shared<Dragon>(dragon_model, envmap, dragon_trans, dragon_mat);

	Scene scene;
	scene.camera = camera;
	scene.light = dir_light;
	scene.skybox = skybox;
	scene.entities.push_back(dragon);

	// game loop
	// ---------
	float last_time = 0;
	while (!Renderer::window_should_close())
	{
		float current_time = Renderer::get_time();
		float deltaTime = current_time - last_time;
		last_time = current_time;
		for (std::shared_ptr<Entity> e : scene.entities)
			e->update(deltaTime);

		Renderer::render(scene, false);
	}

	Renderer::cleanup();
	return 0;
}