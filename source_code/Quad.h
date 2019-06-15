#ifndef Quad_HEADER
#define Quad_HEADER
#include <random>

#include "Model.h"
#include "Camera.h"
#include "Texture.h"

#include "Model.h"
#include "Camera.h"
#include <GL\glew.h>
#include "glm.hpp"
#include "gtc\type_ptr.hpp"
#include <GLFW\glfw3.h>
#include "Texture.h"

class Quad
{
	// This class is for rendering a quad ( a simple primitive ) to screen.
public:
	Quad::Quad();
	void Quad::RenderQuad();

private:
	GLuint quadVAO = 0, quadVBO = 0;

	glm::mat4 *view, *projection, model;
	Camera cam;
};
#endif Quad_HEADER
