#include "headers/Graph.h"
#include <iostream>
#include <format>

// Initialize the OpenGL buffers and configure the graph vertex attributes.
Graph::Graph()
	: VAO(0), VBO(0)
{
    // Generate the vertex array and vertex buffer objects.
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the VAO and VBO before configuring their vertex attributes.
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Configure the first attribute to receive the vertex's 2D position.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Configure the second attribute to receive the vertex's RGB color.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));

    // Unbind the VAO after completing its initial configuration.
    glBindVertexArray(0);
}

// Release the OpenGL resources allocated during graph initialization.
Graph::~Graph()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

// Evaluate a mathematical function at evenly spaced intervals and generate its graph vertices.
void Graph::setFunction(std::function<float(float)> func, float startX, float endX, int segments)
{
    // Ignore invalid functions or segment counts to prevent unnecessary processing.
    if (!func || segments <= 0)
        return;

    // Create a new graph container and associate it with the supplied function.
    GraphData graph;
    graph.function = func;

    // Evaluate the function at the beginning of the interval to establish the initial point.
    float previousX = startX;
    float previousY = func(previousX);

    // Check whether the initial point is already approximately on the X-axis.
    if (std::abs(previousY) < 0.0001f)
    {
        m_intersectionsStream << std::format("({}, 0)\n", previousX);
    }

    // Divide the interval into smaller segments to approximate the function's shape.
    for (int i = 1; i <= segments; i++)
    {
        // Calculate the interpolation factor and corresponding X coordinate.
        float t = static_cast<float>(i) / segments;
        float x = startX + t * (endX - startX);

        // Evaluate the function at the current coordinate.
        float y = func(x);

        // A change in sign between consecutive samples indicates a possible X-axis crossing.
        if ((previousY < 0 && y > 0) ||
            (previousY > 0 && y < 0))
        {
            // Estimate the root by interpolating between the two sampled points.
            float rootX = previousX - previousY * (x - previousX) / (y - previousY);

            // Store the estimated intersection coordinate.
            m_intersectionsStream << std::format("({}, 0)\n", rootX);
        }

        // Also account for sampled points that are already approximately equal to zero.
        else if (std::abs(y) < 0.0001f)
        {
            m_intersectionsStream << std::format("({}, 0)\n", x);
        }

        // Store the current point as a vertex, using red to distinguish the graph from the grid.
        graph.vertices.push_back({ x, y, 1.0f, 0.0f, 0.0f });

        // Update the previous sample so it can be compared with the next one.
        previousX = x;
        previousY = y;
    }

    // Store the completed graph so it can be rendered during subsequent frames.
    m_graphs.push_back(graph);
}

// Render all stored mathematical functions using an orthographic projection.
void Graph::draw(Shader& shader, int width, int height) const
{
    // Avoid unnecessary OpenGL operations when no functions have been generated.
	if (m_graphs.empty())
		return;

    // Activate the shader used to transform and color the graph vertices.
	shader.use();

    // Adapt the horizontal viewing range to the current framebuffer aspect ratio.
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    float viewHeight = 10.0f;
    float viewWidth = viewHeight * aspectRatio;

    // Construct the projection matrix used to map graph coordinates onto the screen.
    glm::mat4 projection = glm::ortho(-viewWidth, viewWidth, -viewHeight, viewHeight);

    // Send the transformation matrix to the vertex shader.
    shader.setMat4("uMVP", projection);

    // Bind the VAO containing the graph's vertex attribute configuration.
    glBindVertexArray(VAO);

    // Iterate through all stored functions and render their generated vertices.
    for (const auto& graph : m_graphs)
    {
        // Skip graphs that do not contain any vertices.
        if (graph.vertices.empty())
            continue;

        // Bind the VBO and upload the current graph's vertex data.
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            graph.vertices.size() * sizeof(Vertex),
            graph.vertices.data(),
            GL_STATIC_DRAW
        );

        // Connect consecutive vertices to approximate the continuous function curve.
        glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(graph.vertices.size()));
    }

    // Unbind the VAO after all stored functions have been rendered.
    glBindVertexArray(0);

    // Set the line thickness for the rendered graph.
    glLineWidth(2.0f);
}

// Return the accumulated coordinates of the graph's intersections with the X-axis.
std::string Graph::getIntersectionsWithX() const
{
    return m_intersectionsStream.str();
}