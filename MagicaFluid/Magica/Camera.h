#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera() {};
	~Camera() {};

	glm::mat4 GetProjectionMatrix(float width,float height)
	{
		return glm::perspective(glm::radians(fov), width / height, near, far);
	}

	glm::mat4 GetInverseProjectionMatrix(float width, float height)
	{
		return glm::inverse(glm::perspective(glm::radians(fov), width / height, near, far));
	}

	glm::mat4 GetViewMatrix()
	{
		glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		view = glm::translate(view, glm::vec3(xTrans, yTrans, zTrans));
		view = glm::rotate(view, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));
		return view;
	}

	glm::vec3 GetPosition()
	{
		return glm::vec3(xTrans, yTrans, zTrans);
	}

	void update()
	{
		if (buttonState == 1)
		{
			xRot += (xRotLength - xRot) * 0.1f;
			yRot += (yRotLength - yRot) * 0.1f;
		}
	}

public:
	float xRot = 15.0f;
	float yRot = 0.0f;
	float xTrans = -5.0;
	float yTrans = -10.0;
	float zTrans = -45.0;
	float fov = 60.0;
	float near = 0.1;
	float far = 100.0;

	int ox = 0;
	int oy = 0;
	int buttonState = 0;
	float xRotLength = 0.0f;
	float yRotLength = 0.0f;
};