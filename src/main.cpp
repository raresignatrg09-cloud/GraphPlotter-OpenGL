#include "headers/Shader.h"
#include "headers/Grid.h"
#include "headers/Window.h"
#include "headers/Graph.h"

#include <unordered_map>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
const char* TITLE = "Grid Lines";

static void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
	Window window(WIDTH, HEIGHT, TITLE);

	window.setFramebufferSizeCallback(framebuffer_size_callback);

	Grid grid;

	Graph graph;
	graph.setFunction([](float x) { return std::sin(x); },-10.0f, 10.0f);
	graph.setFunction([](float x) { return x*x; },-10.0f, 10.0f);
	

	Shader shader(RESOURCES_PATH"shaders/shader.vert", RESOURCES_PATH"shaders/shader.frag");

	while (!window.shouldClose())
	{
		processInput(window.getGLFWwindow());

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int width, height;
		window.getFramebufferSize(width, height);

		shader.use();

		grid.draw(shader);
		graph.draw(shader);

		window.swapBuffers();
		window.pollEvents();
	}
}