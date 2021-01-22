#version 330 core
out vec4 frag_col;

void main()
{
	float near = 0.1;
	float far  = 100.0;
	
	float depth = gl_FragCoord.z;
	float ndc = depth * 2.0 - 1.0;
	float linear_depth = (2.0 * near * far) / (far + near - ndc * (far - near)) / far;
	frag_col = vec4(vec3(linear_depth), 1.0);
}