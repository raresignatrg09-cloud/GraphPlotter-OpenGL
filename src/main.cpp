#include "headers/Shader.h"
#include "headers/Grid.h"
#include "headers/Window.h"
#include "headers/Graph.h"
#include "headers/Parser.hpp"

#include <unordered_map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Define the initial dimensions and title of the application window.
constexpr int WIDTH  = 800;
constexpr int HEIGHT = 800;
const char* TITLE    = "Graph Plotter";

// Store the background colors used by the application's light and dark themes.
glm::vec4 LightModeColor = { 255, 255, 255, 255 };
glm::vec4 DarkModeColor  = { 40, 41, 46, 255 };

// Convert an RGBA color from the 0-255 integer range to OpenGL's 0.0-1.0 range.
static glm::vec4 fromRGBAtoFloat(int r, int g, int b, int a)
{
	return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

// Apply the selected background color and clear the current framebuffer.
static void clearColor(const glm::vec4& color)
{
	glm::vec4 colorFloat = fromRGBAtoFloat(static_cast<int>(color.r), static_cast<int>(color.g), static_cast<int>(color.b), static_cast<int>(color.a));
	
    glClearColor(colorFloat.r, colorFloat.g, colorFloat.b, colorFloat.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

// Handle application-level keyboard input, allowing the user to close the window with ESC.
static void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Keep the OpenGL viewport synchronized with the actual framebuffer dimensions.
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Initialize the ImGui context and connect its GLFW and OpenGL backends.
static void initImGui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Use the dark theme as the initial interface style.
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

int main()
{
    // Create the application window and initialize its OpenGL context.
	Window window(WIDTH, HEIGHT, TITLE);

    // Register a callback to handle framebuffer resizing.
	window.setFramebufferSizeCallback(framebuffer_size_callback);

    // Initialize the graphical user interface after creating the OpenGL context.
    initImGui(window.getWindow());

    // Create the objects responsible for rendering the coordinate grid and function graph.
	Grid grid;
	Graph graph;
	
    // Provide an initial mathematical expression and a buffer for user input.
    char expressionBuffer[256] = "x*x + x - 4";
    std::string expressionError;
    
    // Parse the initial expression and use it to configure the graph.
    Expression expression(expressionBuffer);

    // Pass the evaluator as a callable object so the graph can calculate its points.
    graph.setFunction(
        [expression](float x) {
            return static_cast<float>(expression.evaluate(x));
        },
        -10.0f,
        10.0f
    );

    // Load the vertex and fragment shaders used for rendering.
	Shader shader(RESOURCES_PATH"shaders/shader.vert", RESOURCES_PATH"shaders/shader.frag");

    // Initialize the application with the dark background and matching interface theme.
	glm::vec4 clearColorValue = DarkModeColor;
    static bool enableDarkMode = true;

    // Main application loop: process input, render the scene, and update the interface.
	while (!window.shouldClose())
	{
        // Check for keyboard input before rendering the next frame.
        processInput(window.getWindow());

        // Clear the previous frame using the currently selected background color.
        clearColor(clearColorValue);

        // Retrieve the framebuffer dimensions for rendering and displaying window information.
        int width, height;
        window.getFramebufferSize(width, height);

        // Activate the shader program before drawing the scene.
        shader.use();

        // Render the coordinate grid first, followed by the mathematical function.
        grid.draw(shader, width, height);
        graph.draw(shader, width, height);

        // Begin a new ImGui frame using the GLFW and OpenGL backends.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Build the settings panel containing the application's interactive controls.
        ImGui::Begin("Settings");

        // Allow the user to switch between light and dark interface themes.
        if (ImGui::Checkbox("Dark mode", &enableDarkMode))
        {
            if (enableDarkMode)
            {
                // Restore the dark interface style and background color.
                ImGui::StyleColorsDark();
                clearColorValue = DarkModeColor;
            }
            else
            {
                // Switch to the light interface style and background color.
                ImGui::StyleColorsLight();
                clearColorValue = LightModeColor;
            }
        }

        ImGui::Text("Grid Lines");
        ImGui::Separator();

        // Display the input field used to enter a mathematical function.
        ImGui::Text("Function");

        ImGui::SetNextItemWidth(250.0f);
        ImGui::InputText("##Expression", expressionBuffer, sizeof(expressionBuffer));

        // Attempt to parse and plot the expression whenever the user requests it.
        if (ImGui::Button("Plot"))
        {
            // Remove the previously generated graph vertices before updating the function.
			graph.clearVertices(); 
            
            try
            {
                // Create a new expression from the current input.
                Expression newExpression(expressionBuffer);

                // Compile once and capture the expression for subsequent graph evaluations.
                graph.setFunction(
                    [newExpression](float x) {
                        return static_cast<float>(newExpression.evaluate(x));
                    },
                    -10.0f,
                    10.0f
                );

                // Clear any previous parsing errors after a successful update.
                expressionError.clear();
            }
            catch (const std::exception& e)
            {
                // Preserve the error message so it can be displayed in the settings panel.
                expressionError = e.what();
            }
        }

        // Display parsing or evaluation-related errors without interrupting the application.
        if (!expressionError.empty())
        {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: %s", expressionError.c_str());
        }

		ImGui::Separator();

        // Display the calculated intersections between the plotted function and the X-axis.
		ImGui::Text("The intersections of the graph with the X-axis are: \n %s", graph.getIntersectionsWithX().c_str());
        
        ImGui::Separator();

        // Display the current framebuffer dimensions for reference.
        ImGui::Text("Window: %d x %d", width, height);

        ImGui::End();

        // Finalize the ImGui frame and submit its draw commands to OpenGL.
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Present the completed frame and process pending window events.
        window.swapBuffers();
        window.pollEvents();
	}

    // Release ImGui backend resources and destroy its context before exiting.
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}