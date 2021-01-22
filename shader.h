#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:
	unsigned int ID;
public:
	Shader(const char* vert_path, const char* frag_path);
	void use() { glUseProgram(ID); }
	void set_int(const char* name, int val) { glUniform1i(glGetUniformLocation(ID, name), val); }
	void set_float(const char* name, float val) { glUniform1f(glGetUniformLocation(ID, name), val); }
	void set_vec3(const char* name, glm::vec3 vec) { glUniform3f(glGetUniformLocation(ID, name), vec.x, vec.y, vec.z); }
	void set_mat4(const char* name, glm::mat4 mat) { glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(mat)); }
};