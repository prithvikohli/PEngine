#version 330 core
in vec3 world_normal;
in vec3 world_pos;

out vec4 frag_col;

uniform samplerCube environment_map;
uniform vec3 view_pos;

void main()
{
	vec3 I = normalize(world_pos - view_pos);
	vec3 R = refract(I, normalize(world_normal), 1.0 / 1.52);
	frag_col = texture(environment_map, R);
}