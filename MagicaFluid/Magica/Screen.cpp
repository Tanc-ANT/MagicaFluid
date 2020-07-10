#include "Screen.h"

Screen::Screen()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
}

Screen::~Screen()
{

}

GLuint Screen::GetVao()
{
	return vao;
}

void Screen::shaderVAOQuad()
{
	GLfloat vertices[] = {
			1.0f, 1.0f,	   // Top Right
			1.0f, -1.0f,   // Bottom Right
			-1.0f, -1.0f,  // Bottom Left
			-1.0f, 1.0f	   // Top Left 
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3,	// First Triangle
		1, 2, 3	// Second Triangle
	};

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);
}