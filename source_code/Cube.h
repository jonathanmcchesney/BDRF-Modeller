#ifndef Cube_HEADER
#define Cube_HEADER
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

class Cube
{
// Header class for rendering cube primitives, relevant accesses aswsigned
public:
	Cube::Cube();
	void Cube::renderCube();

private:
	GLuint cubeVAO = 0;
	GLuint cubeVBO = 0;
	glm::mat4 *view, *projection, model;
	Camera cam;

};
#endif Cube_HEADER
