#include "headers/Graph.h"

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

void Graph::setFunction(std::function<float(float)> func, float startX, float endX, int segments)
{
	if (!func || segments <= 0)
		return;

	GraphData graph;

	graph.function = func;

	for (int i = 0; i <= segments; i++)
	{
		float t = static_cast<float>(i) / segments;
		float x = startX + t * (endX - startX);
		float y = func(x);
		graph.vertices.push_back({ x, y, 1.0f, 0.0f, 0.0f });
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
