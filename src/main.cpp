#define STB_IMAGE_IMPLEMENTATION

#include "headers/stb_image.h"
#include "headers/Shader.h"
#include "headers/Grid.h"
#include "headers/Window.h"

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
	Shader shader(RESOURCES_PATH"shaders/grid.vert", RESOURCES_PATH"shaders/grid.frag");

	while (!window.shouldClose())
	{
		processInput(window.getGLFWwindow());

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int width, height;
		window.getFramebufferSize(width, height);

		shader.use();

		grid.draw(shader);

		window.swapBuffers();
		window.pollEvents();
	}
}