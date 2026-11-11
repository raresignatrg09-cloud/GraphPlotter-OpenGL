#pragma once

#include <vector>
#include <functional>

#include "Shader.h"
#include "Grid.h"

class Graph 
{
public:
	Graph();
	
	void generate(float startX, float endX, int segments = 100);;
	void setFunction(std::function<float(float)> func);
	void draw(Shader& shader);

private:
	GLuint VAO, VBO;
	int vertexCount;

	std::function<float(float)> m_function;
	std::vector<Vertex> m_vertices;
};