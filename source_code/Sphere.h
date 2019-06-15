#ifndef Sphere_HEADER
#define Sphere_HEADER
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


class Sphere
{
	// This class is for use of the sphere object (programatically created) all relevnat access (private or public) is handled accordingly.
public:
	Sphere::Sphere();
	void Sphere::renderSphere();

private:
	unsigned int sphereVertexArrayObject = 0, indexCount;

	glm::mat4 *view, *projection, model;
	Camera cam;
};
#endif Sphere_HEADER
