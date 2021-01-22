#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 vert_tex_coords;

out vec3 world_normal;
out vec3 world_pos;
out vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	world_normal = mat3(transpose(inverse(model))) * normal;
	world_pos = vec3(model * vec4(pos, 1.0));
	tex_coords = vert_tex_coords;
}