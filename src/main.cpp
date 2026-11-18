#include "headers/Shader.h"
#include "headers/Grid.h"
#include "headers/Window.h"
#include "headers/Graph.h"
#include "headers/Parser.hpp"

#include <unordered_map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
const char* TITLE = "Grid Lines";

glm::vec4 LightModeColor = { 255, 255, 255, 255 };
glm::vec4 DarkModeColor  = { 40, 41, 46, 255 };

static glm::vec4 fromRGBAtoFloat(int r, int g, int b, int a)
{
	return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

static void clearColor(const glm::vec4& color)
{
	glm::vec4 colorFloat = fromRGBAtoFloat(static_cast<int>(color.r), static_cast<int>(color.g), static_cast<int>(color.b), static_cast<int>(color.a));
	glClearColor(colorFloat.r, colorFloat.g, colorFloat.b, colorFloat.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui::StyleColorsDark(); // Set dark theme

	ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	Grid grid;

	Graph graph;
	
    char expressionBuffer[256] = "x*x + x - 4";
    std::string expressionError;

    Expression expression(expressionBuffer);

    graph.setFunction(
        [expression](float x) {
            return static_cast<float>(expression.evaluate(x));
        },
        -10.0f,
        10.0f
    );

	Shader shader(RESOURCES_PATH"shaders/shader.vert", RESOURCES_PATH"shaders/shader.frag");

	glm::vec4 clearColorValue = DarkModeColor;
    static bool enableDarkMode = true;

	while (!window.shouldClose())
	{
        processInput(window.getGLFWwindow());

        // Clear framebuffer
        clearColor(clearColorValue);

        // Get framebuffer size
        int width, height;
        window.getFramebufferSize(width, height);

        // OpenGL rendering
        shader.use();

        grid.draw(shader, width, height);
        graph.draw(shader, width, height);

        // ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Settings
        ImGui::Begin("Settings");

        if (ImGui::Checkbox("Dark mode", &enableDarkMode))
        {
            if (enableDarkMode)
            {
                ImGui::StyleColorsDark();
                clearColorValue = DarkModeColor;
            }
            else
            {
                ImGui::StyleColorsLight();
                clearColorValue = LightModeColor;
            }
        }

        ImGui::Text("Grid Lines");
        ImGui::Separator();

        ImGui::Text("Function");

        ImGui::SetNextItemWidth(250.0f);
        ImGui::InputText("##Expression", expressionBuffer,
            sizeof(expressionBuffer));

        if (ImGui::Button("Plot"))
        {
			graph.clearVertices(); // Clear previous vertices before plotting a new function
            try
            {
                Expression newExpression(expressionBuffer);

                // Compile once, then capture the compiled expression.
                graph.setFunction(
                    [newExpression](float x) {
                        return static_cast<float>(newExpression.evaluate(x));
                    },
                    -10.0f,
                    10.0f
                );

                expressionError.clear();
            }
            catch (const std::exception& e)
            {
                expressionError = e.what();
            }
        }

        if (!expressionError.empty())
        {
            ImGui::TextColored(
                ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
                "Error: %s",
                expressionError.c_str()
            );
        }

		ImGui::Separator();

		ImGui::Text("The intersections of the graph with the X-axis are: \n %s", graph.getIntersectionsWithX().c_str());
        ImGui::Separator();

        ImGui::Text("Window: %d x %d", width, height);

        ImGui::End();

        // Render ImGui
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swapBuffers();
        window.pollEvents();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}