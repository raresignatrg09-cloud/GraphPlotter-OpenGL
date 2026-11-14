#pragma once

#include <vector>
#include <functional>
#include <sstream>

#include "Shader.h"
#include "Grid.h"

struct GraphData
{
	std::function<float(float)> function;
	std::vector<Vertex> vertices;
	int vertexCount = 0;
};

class Graph 
{
public:
	Graph();
	~Graph();

	void setFunction(std::function<float(float)> func,float startX,float endX,int segments=100);
	void draw(Shader& shader) const;
	std::string getIntersectionsWithX() const;

	Graph(const Graph&) = delete;
	Graph& operator=(const Graph&) = delete;

private:
	GLuint VAO, VBO;
	int vertexCount;

	std::vector<GraphData> m_graphs;
	std::stringstream m_intersectionsStream{};
};