#ifndef Object_HEADER
#define Object_HEADER
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
#include "Constants.h"

#include "stb_image.h"
#include "Cube.h"



class Object
{
	// Class for rendering model object scenes - this class was created so that I can demonstrate the functionality in a seperate class.
public:
	Object::Object();
	void Object::RenderModel(Shader &pbrShader, Model ourModel, glm::mat4 *projection, glm::mat4 *view, Camera cam, int roughness, int metalness);

private:
	glm::mat4 *view, *projection, model;
	Camera cam;
};
#endif Object_HEADER
