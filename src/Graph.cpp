#include "headers/Graph.h"
#include <iostream>
#include <format>

Graph::Graph()
	: VAO(0), VBO(0)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

Graph::~Graph()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

#include <cmath>
#include <iostream>
#include <format>

void Graph::setFunction(std::function<float(float)> func, float startX, float endX, int segments)
{
    if (!func || segments <= 0)
        return;

    GraphData graph;
    graph.function = func;

    float previousX = startX;
    float previousY = func(previousX);

    // Check if the first point is already a root
    if (std::abs(previousY) < 0.0001f)
    {
        std::cout << std::format("X-axis intersection: ({}, 0)", previousX) << '\n';
		m_intersectionsStream << std::format("X-axis intersection: ({}, 0)\n", previousX);
    }

    for (int i = 1; i <= segments; i++)
    {
        float t = static_cast<float>(i) / segments;
        float x = startX + t * (endX - startX);
        float y = func(x);

        // Detect a sign change
        if ((previousY < 0 && y > 0) ||
            (previousY > 0 && y < 0))
        {
            // Estimate the root using linear interpolation
            float rootX = previousX -
                previousY * (x - previousX) / (y - previousY);

            std::cout << std::format("X-axis intersection: ({}, 0)",rootX) << '\n';
			m_intersectionsStream << std::format("X-axis intersection: ({}, 0)\n", rootX);
        }

        // Check if the current point is approximately zero
        else if (std::abs(y) < 0.0001f)
        {
            std::cout << std::format("X-axis intersection: ({}, 0)", x) << '\n';
			m_intersectionsStream << std::format("X-axis intersection: ({}, 0)\n", x);
        }

        graph.vertices.push_back({ x, y, 1.0f, 0.0f, 0.0f });

        previousX = x;
        previousY = y;
    }

    m_graphs.push_back(graph);
}

void Graph::draw(Shader & shader) const
{
	if (m_graphs.empty())
		return;

	shader.use();

	glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f);

	shader.setMat4("uMVP", projection);

	glBindVertexArray(VAO);

	for (const auto& graph : m_graphs)
	{
		if (graph.vertices.empty())
			continue;

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, graph.vertices.size() * sizeof(Vertex), graph.vertices.data(), GL_STATIC_DRAW);

		glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(graph.vertices.size()));
	}
	glBindVertexArray(0);
}

std::string Graph::getIntersectionsWithX() const
{
    return m_intersectionsStream.str();
}