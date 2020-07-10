#pragma once
#include "pch.h"
#include "Shader.h"

class Screen
{
public:
	Screen();
	~Screen();

	GLuint GetVao();
	void shaderVAOQuad();

private:
	GLuint vao, vbo, ebo;
	Shader* screenShader;

};