#include <fstream>
#include <sstream>
#include <iostream>

#include "shader.h"

Shader::Shader(const char* vert_path, const char* frag_path)
{
	// read contents of shader files
	// -----------------------------
	std::ifstream vert_file, frag_file;
	std::string vert_code, frag_code;

	vert_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	frag_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vert_file.open(vert_path);
		frag_file.open(frag_path);
		std::stringstream vert_stream, frag_stream;

		vert_stream << vert_file.rdbuf();
		frag_stream << frag_file.rdbuf();

		vert_file.close();
		frag_file.close();

		vert_code = vert_stream.str();
		frag_code = frag_stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "!!! ERROR: shader file(s) not successfully read\n";
	}

	// compile shader code and link to build shader program
	// ----------------------------------------------------
	unsigned int vert, frag;
	int success;
	char infoLog[512];

	const char* vert_source = vert_code.c_str();
	const char* frag_source = frag_code.c_str();

	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vert_source, NULL);
	glCompileShader(vert);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vert, 512, NULL, infoLog);
		std::cout << "!!! ERROR: vertex shader compilation failed\n" << infoLog << "\n";
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &frag_source, NULL);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(frag, 512, NULL, infoLog);
		std::cout << "!!! ERROR: fragment shader compilation failed\n" << infoLog << "\n";
	}

	ID = glCreateProgram();
	glAttachShader(ID, vert);
	glAttachShader(ID, frag);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "!!! ERROR: shader program linking failed\n" << infoLog << "\n";
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
}