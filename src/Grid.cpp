#include "headers/Grid.h"

Grid::Grid()
{
	std::vector<Vertex> vertices = generateGrid();

	gridVertexCount = static_cast<int>(vertices.size());

	glGenVertexArrays(1, &gridVAO);
	glGenBuffers(1, &gridVBO);

	glBindVertexArray(gridVAO);

	glBindBuffer(GL_ARRAY_BUFFER, gridVBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));
}

std::vector<Vertex> Grid::generateGrid()
{
	std::vector<Vertex> vertices;

	for (int i = -10; i <= 10; i++)
	{
		float r = 0.5f, g = 0.5f, b = 0.5f;
		float vr = r, vg = g, vb = b;

		if (i == 0)
		{
			vr = 0.0f;
			vg = 1.0f;
			vb = 0.0f;
		}

		//Vertical lines
		vertices.push_back({ (float)i,-10, vr, vg, vb });
		vertices.push_back({ (float)i, 10, vr, vg, vb });

		float hr = r, hg = g, hb = b;

		if (i == 0)
		{
			hr = 1.0f;
			hg = 0.0f;
			hg = 0.0f;
		}

		//Horizontal lines
		vertices.push_back({ -10,(float)i, hr, hg, hb });
		vertices.push_back({ 10,(float)i, hr, hg, hb });
	}

	return vertices;
}

void Grid::draw(Shader& shader) const
{
	shader.use();

	glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f);

	shader.setMat4("uMVP", projection);

	glBindVertexArray(gridVAO);
	glDrawArrays(GL_LINES, 0, gridVertexCount);
	glBindVertexArray(0);
}