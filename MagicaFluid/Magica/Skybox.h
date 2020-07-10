#pragma once
#include "pch.h"
#include "Camera.h"
#include "Window.h"
#include "Shader.h"

class Skybox
{
public:
	Skybox();
	~Skybox();

	void draw(Window* window, Camera* camera);
	GLuint GetVao() { return skyboxVAO; }

private:
	unsigned int loadCubemap(std::vector<std::string> faces);

private:
	Shader* skyboxShader;
	GLuint skyboxVAO, skyboxVBO;
	GLuint skyboxTexture;
};