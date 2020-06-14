#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera() {};
	~Camera() {};

	glm::mat4 GetViewMatrix()
	{
		glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		view = glm::translate(view, glm::vec3(xTrans, yTrans, zTrans));
		view = glm::rotate(view, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));
		return view;
	}

public:
	float xRot = 15.0f;
	float yRot = 0.0f;
	float xTrans = -5.0;
	float yTrans = -10.0;
	float zTrans = -45.0;
	float fov = 60.0;

	int ox = 0;
	int oy = 0;
	int buttonState = 0;
	float xRotLength = 0.0f;
	float yRotLength = 0.0f;
};