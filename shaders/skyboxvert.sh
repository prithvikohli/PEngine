#version 330
layout (location = 0) in vec3 pos;
out vec3 tex_coords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	tex_coords = pos;
	gl_Position = projection * view * vec4(pos, 1.0);	// don't need world transform matrix since skybox always centered at (0,0,0) world coordinates, so its world coorinates = local coordinates
}