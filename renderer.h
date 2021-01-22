#pragma once

#include "entity.h"
#include <GLFW/glfw3.h>

class Renderer
{
public:
	static GLFWwindow* window;
	static bool setup(int width, int height);
	static void render(const Scene& scene, bool transparent_pass);
	static bool window_should_close() { return glfwWindowShouldClose(window); }
	static float get_time() { return glfwGetTime(); }
	static void cleanup() { glfwTerminate(); };
};