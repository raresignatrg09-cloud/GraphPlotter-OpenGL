#include "headers/Graph.h"

Graph::Graph()
	: VAO(0), VBO(0), vertexCount(0), m_function(nullptr)
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

void Graph::generate(float startX, float endX, int segments)
{
	if (!m_function || segments <= 0)
		return;
	
	m_vertices.clear();

	for (int i = 0; i <= segments; i++)
	{
		float t = static_cast<float>(i) / segments;

		float x = startX + t * (endX - startX);
		float y = m_function(x);

		m_vertices.push_back({ x, y, 1.0f, 0.0f, 0.0f });
	}

	vertexCount = static_cast<int>(m_vertices.size());

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
}

void Graph::setFunction(std::function<float(float)> func)
{
	m_function = func;
}

void Graph::draw(Shader & shader)
{
	if (vertexCount == 0)
		return;

	shader.use();

	glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f);

	shader.setMat4("uMVP", projection);

	glBindVertexArray(VAO);

	glDrawArrays(GL_LINE_STRIP, 0, vertexCount);

	glBindVertexArray(0);
}
