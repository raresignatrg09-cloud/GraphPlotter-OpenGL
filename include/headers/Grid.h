#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <numbers>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

// Represent a single vertex using its 2D position and RGB color.
struct Vertex
{
	float x, y;
	float r, g, b;
};

class Grid
{
public:
    // Initialize the grid and upload its generated vertices to the GPU.
    Grid();

    // Generate the vertices required to construct the coordinate grid.
    static std::vector<Vertex> generateGrid();

    // Render the grid using the provided shader and framebuffer dimensions.
    void draw(Shader& shader, int width, int height) const;

    // Prevent copying to avoid multiple objects managing the same OpenGL resources.
    Grid(const Grid&) = delete;
    Grid& operator=(const Grid&) = delete;

private:
    // Store the vertex buffer and vertex array used for grid rendering.
    GLuint gridVBO, gridVAO;

    // Keep track of the total number of vertices submitted for rendering.
    int gridVertexCount;
};