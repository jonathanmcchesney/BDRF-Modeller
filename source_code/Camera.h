#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#define GLEW_STATIC
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include <GL/glew.h>


enum Movement_Of_Camera
{
	FORWARD, BACKWARD, LEFT, RIGHT
};

// Default camera values for the variaous variables
const GLfloat cYaw = -90.0f, cPitch = 0.0f, cSpeed = 6.0f, cSensitivity = 0.25f, cZoom = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera movement options


	// Camera Constructor with the relevant vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = cYaw, GLfloat pitch = cPitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(cSpeed), mouseSensitivity(cSensitivity), zoomIntensity(cZoom)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	// Constructor with the relevant "scalar" values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(cSpeed), mouseSensitivity(cSensitivity), zoomIntensity(cZoom)
	{
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	// Returns view matrix calculated using Eular Angles and the LookAt Matrix respectively
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->front, this->up); // v. important
	}

	// Processes key input from main.cpp
	void ProcessKeyboard(Movement_Of_Camera direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->moveSpeed * deltaTime;

		if (direction == FORWARD)
		{
			this->position += this->front * velocity; // increase forward velocity
			//this->position.y = 0;
		}

		if (direction == BACKWARD)
		{
			this->position -= this->front * velocity; // increase backwards velocity
			//this->position.y = 0;

		}

		if (direction == LEFT)
		{
			this->position -= this->right * velocity; // increase left velocity
			//this->position.y = 0;

		}

		if (direction == RIGHT)
		{
			this->position += this->right * velocity; // increase right velocity
			//this->position.y = 0;

		}
	}

	void SwitchCameras () {
		this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	// Processes mouse input from main.cpp
	void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true)
	{
		xOffset *= this->mouseSensitivity; // detect/react x sensitivity
		yOffset *= this->mouseSensitivity; // detect/react y sensitivity

		this->yaw += xOffset;
		this->pitch += yOffset;

		// If pitch is out of bounds, d'ont flip screenflipped
		if (constrainPitch)
		{
			if (this->pitch < -89.0f)
			{
				this->pitch = -89.0f;
			}
			if (this->pitch > 89.0f)
			{
				this->pitch = 89.0f;
			}
		}
		// Update the Front, Right and Up Vectors using updated:- Eular angles
		this->updateCameraVectors(); // trigger the method here
	}
	// Processes mouse scroll input from main.cpp
	void MouseZoom(GLfloat yOffset)
	{
		if (this->zoomIntensity <= 1.0f)
		{
			this->zoomIntensity = 1.0f;
		}
		if (this->zoomIntensity >= 45.0f)
		{
			this->zoomIntensity = 45.0f;
		}
		if (this->zoomIntensity >= 1.0f && this->zoomIntensity <= 45.0f)
		{
			this->zoomIntensity -= yOffset;
		}
	}

	// return the camera front
	glm::vec3 GetFront()
	{
		return this->front;
	}
	//return the zoom intensity
	GLfloat GetZoom()
	{
		return this->zoomIntensity;
	}
	// return the current position
	glm::vec3 GetPosition()
	{
		return this->position;
	}

public:
	// Camera Attributes:-
	glm::vec3 position, front, up, right, worldUp;

	// Eular Angles:-
	GLfloat yaw, pitch;

	// Camera options: - 
	GLfloat moveSpeed, mouseSensitivity, zoomIntensity;

	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));// x calculations:- followed by y and z, using radians
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front = glm::normalize(front);

		this->right = glm::normalize(glm::cross(this->front, this->worldUp));  
		this->up = glm::normalize(glm::cross(this->right, this->front)); // Normalize as to reduce errors as they tend towards 0
	}
};

#endif 
