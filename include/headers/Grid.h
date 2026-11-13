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

struct Vertex
{
	float x, y;
	float r, g, b;
};

class Grid
{
public:
	Grid();
	static std::vector<Vertex> generateGrid();
	void draw(Shader& shader) const;

	Grid(const Grid&) = delete;
	Grid& operator=(const Grid&) = delete;

private:
	GLuint gridVBO, gridVAO;
	int gridVertexCount;
};