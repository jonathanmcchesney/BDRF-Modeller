#ifndef IBL_HEADER
#define IBL_HEADER
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

#include "stb_image.h"
#include "Cube.h"
#include "Quad.h"
#include "Constants.h"

class IBL
{

public:
	// Constructor and other relevant methods for the Image Based Lighting Class.
	IBL::IBL();
	void IBL::setUp(Shader &equirectangularToCubemapShader, Shader &irradianceShader, Shader &pbrShader, Shader &backgroundShader, Shader &shader, bool textured, glm::mat4 *projection, Shader &bdrfShader, Shader &prefilterShader);
	void IBL::showMap(Shader &backgroundShader, bool showIrradiance, glm::mat4 *view, Shader &bdrfShader);
	string IBL::ChangeTexture(int currentTexture);
	Shader IBL::ChangeShader(Shader &tex, Shader &unTex, Shader &pbrShader, Shader &simpleTex, Shader &simpleUntex, bool textured, bool specular, int colourPreset);
	void IBL::LoadTextures(); // Unused class - kept in to show development of a start of program texture handler - inefficient

private:
	// Use of the cube and quad class
	Cube IBL::cube;
	Quad IBL::quad;

	GLuint quadVAO = 0, quadVBO;
	/* 
	Inefficient way of switching textures: if I had more time I would have focused on this area,
	I would have developed an assetmanager and handled texture changing and setting more appropriately.
	*/
	GLuint albedo, normal, metallic = 0, roughness = 0, ao;
	GLuint albedo0, normal0, metallic0 = 0, roughness0 = 0, ao0;
	GLuint albedo1, normal1, metallic1 = 0, roughness1 = 0, ao1;
	GLuint albedo2, normal2, metallic2 = 0, roughness2 = 0, ao2;
	GLuint albedo3, normal3, metallic3 = 0, roughness3 = 0, ao3;
	GLuint albedo4, normal4, metallic4 = 0, roughness4 = 0, ao4;
	GLuint albedo5, normal5, metallic5 = 0, roughness5 = 0, ao5;
	GLuint albedo6, normal6, metallic6 = 0, roughness6 = 0, ao6;
	GLuint albedo7, normal7, metallic7 = 0, roughness7 = 0, ao7;
	GLuint albedo8, normal8, metallic8 = 0, roughness8 = 0, ao8;
	GLuint albedo9, normal9, metallic9 = 0, roughness9 = 0, ao9;
	GLuint albedo10, normal10, metallic10 = 0, roughness10 = 0, ao10;
	GLuint albedo11, normal11, metallic11 = 0, roughness11 = 0, ao11;
	GLuint albedo12, normal12, metallic12 = 0, roughness12 = 0, ao12;
	GLuint albedo13, normal13, metallic13 = 0, roughness13 = 0, ao13;
	GLuint albedo14, normal14, metallic14 = 0, roughness14 = 0, ao14;
	GLuint albedo15, normal15, metallic15 = 0, roughness15 = 0, ao15;
	GLuint albedo16, normal16, metallic16 = 0, roughness16 = 0, ao16;
	GLuint albedo17, normal17, metallic17 = 0, roughness17 = 0, ao17;
	GLuint albedo18, normal18, metallic18 = 0, roughness18 = 0, ao18;
	GLuint albedo19, normal19, metallic19 = 0, roughness19 = 0, ao19;
	GLuint albedo20, normal20, metallic20 = 0, roughness20 = 0, ao20;

	string texture;
	unsigned int prefilterMap, brdfLUTTexture, mipWidth, mipHeight, captureFBO, captureRBO, envCubemap, irradianceMap;

	glm::mat4 *view, *projection, model;
	Camera cam;
};
#endif IBL_HEADER
