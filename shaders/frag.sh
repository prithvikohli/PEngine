#version 330 core
in vec3 world_normal;
in vec3 world_pos;
in vec2 tex_coords;

out vec4 frag_col;

struct Material {
	vec3 color;
	float alpha;
	float shininess;
};

struct Direction_Light {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Direction_Light dir_light;
uniform vec3 view_pos;

vec3 calc_dir_light(Direction_Light light, vec3 normal, vec3 view_dir);

void main()
{
	vec3 norm = normalize(world_normal);
	vec3 view_dir = normalize(view_pos - world_pos);

	vec3 result = calc_dir_light(dir_light, norm, view_dir);
	frag_col = vec4(result, material.alpha);
}

vec3 calc_dir_light(Direction_Light light, vec3 normal, vec3 view_dir)
{
	vec3 ambient = material.color * light.ambient;

	vec3 light_dir = normalize(-light.direction);
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = material.color * diff * light.diffuse;

	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = spec * light.specular;

	return ambient + diffuse + specular;
}