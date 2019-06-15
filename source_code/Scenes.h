#ifndef Scenes_HEADER
#define Scenes_HEADER
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
#include "Shader.h"
#include "Sphere.h"
#include "Model.h"

class Scenes
{
	// This class sets up and renders the various scenes regarding the lighting and spheres with relevant access levels.
public:
	Scenes::Scenes();
	void Scenes::RenderSphereScene01(Shader &pbrShader);
	void Scenes::RenderSphereScene02(Shader &pbrShader, int roughness, int metalness, bool rotate);
	void Scenes::RenderLightScene00(Shader &pbrShader);
	void Scenes::RenderLightScene01(Shader &pbrShader);
	void Scenes::RenderLightScene02(Shader &pbrShader);
	void Scenes::RenderLightScene03(Shader &pbrShader);

	Sphere Scenes::sphere;
	Model bulb;

private:
	int nrRows = 5, nrColumns = 5; // increase or decrease for more or less rows/columns
	float spacing = 2.5;

	glm::mat4 *view, *projection, model;
	Camera cam;
};
#endif