#include "headers/Grid.h"

// Generate the grid geometry and initialize the corresponding GPU buffers.
Grid::Grid() 
{
	// Generate the complete set of vertices before uploading them to the GPU.
	std::vector<Vertex> vertices = generateGrid();

	// Store the number of vertices needed by the drawing operation.
	gridVertexCount = static_cast<int>(vertices.size());

	// Create the vertex array and vertex buffer objects.
	glGenVertexArrays(1, &gridVAO);
	glGenBuffers(1, &gridVBO);

	// Bind the VAO to configure the vertex attribute layout.
	glBindVertexArray(gridVAO);

	// Bind the VBO that will store the grid vertex data.
	glBindBuffer(GL_ARRAY_BUFFER, gridVBO);

	// Upload the generated vertices to GPU memory.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// Configure the first vertex attribute to represent the 2D position.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Configure the second vertex attribute to represent the RGB color.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));
}

// Generate the horizontal and vertical lines that form the coordinate grid.
std::vector<Vertex> Grid::generateGrid()
{
	std::vector<Vertex> vertices;

	// Iterate across the coordinate range to generate equally spaced grid lines.
	for (int i = -10; i <= 10; i++)
	{
		// Use a neutral gray color for the regular grid lines.
		float r = 0.5f, g = 0.5f, b = 0.5f;
		float vr = r, vg = g, vb = b;

		// Highlight the vertical axis using green to distinguish it from the grid.
		if (i == 0)
		{
			vr = 0.0f;
			vg = 1.0f;
			vb = 0.0f;
		}

		// Define the two endpoints of the current vertical line.
		vertices.push_back({ (float)i,-10, vr, vg, vb });
		vertices.push_back({ (float)i, 10, vr, vg, vb });

		// Assign the default grid color to the horizontal line.
		float hr = r, hg = g, hb = b;

		// Highlight the horizontal axis using red.
		if (i == 0)
		{
			hr = 1.0f;
			hg = 0.0f;
			hb = 0.0f;
		}

		// Define the two endpoints of the current horizontal line.
		vertices.push_back({ -10,(float)i, hr, hg, hb });
		vertices.push_back({ 10,(float)i, hr, hg, hb });
	}

	// Return the generated vertex data for GPU upload.
	return vertices;
}

// Render the grid using an orthographic projection that preserves its coordinate system.
void Grid::draw(Shader& shader, int width, int height) const
{
	// Activate the shader program responsible for processing the grid vertices.
	shader.use();

	// Calculate the aspect ratio to adapt the projection to the framebuffer dimensions.
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	// Keep the vertical viewing range fixed and adjust the horizontal range accordingly.
	float viewHeight = 10.0f; 
	float viewWidth = viewHeight * aspectRatio;
	
	// Create an orthographic projection suitable for displaying a 2D coordinate plane.
	glm::mat4 projection = glm::ortho(-viewWidth, viewWidth, -viewHeight, viewHeight);

	// Pass the projection matrix to the vertex shader.
	shader.setMat4("uMVP", projection);

	// Bind the grid's VAO to restore its vertex attribute configuration.
	glBindVertexArray(gridVAO);

	// Render the grid vertices as independent line segments.
	glDrawArrays(GL_LINES, 0, gridVertexCount);

	// Unbind the VAO after completing the drawing operation.
	glBindVertexArray(0);

	// Set the line thickness used for subsequent rendering operations.
	glLineWidth(2.0f);
}