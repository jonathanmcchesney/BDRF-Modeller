#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <iostream>
#include <string>
#include <cmath>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <GL/glew.h>
#include <random>
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <stdio.h>
#include <stdlib.h>
#include "SOIL2/SOIL2.h"
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "Cube.h"
#include "Sphere.h"
#include "IBL.h"
#include "Object.h"
#include "stb_image.h"
#include "Text.h"
#include "Scenes.h"

// Relevant Input Manager methods
void keyTouch(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseMovement(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
void Update();

// Declaration of various object/class instances 
Camera camera(glm::vec3(0.0f, 40.0f, 3.0f)), staticCamera1(glm::vec3(0.0f, 40.0f, 3.0f)), staticCamera2(glm::vec3(-3.0f, 43.5f, 15.0f));
Camera cameras[3] = { camera, staticCamera1, staticCamera2};
Cube cube;
Sphere sphere;
IBL ibl;
Scenes scene;
Object obj;
Text text;

int main()
{
	// Region for setting up the system/ window etc.
#pragma Setup_Region
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Setting up the viewing window - disable cursor 
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ELE4014 Final Project - Jonathan McChesney", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyTouch);
    glfwSetCursorPosCallback(window, mouseMovement);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Make sure to set experimental to true, handle errors accordingly
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();

	// Enable death testing and the depth function, as well as blending functionality and seamless cubemaps func.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	
	// Set up the various shaders we will be using throughout the project.
	Shader pbrSimpleTex("res/shaders/pbrFinal.vertex", "res/shaders/pbrTextured.fragment");
	Shader pbrSimpleUntex("res/shaders/pbrFinal.vertex", "res/shaders/pbrUntextured.fragment");
	Shader pbrTex("res/shaders/pbrFinal.vertex", "res/shaders/pbrFinalTextured.fragment");
	Shader pbrUntex("res/shaders/pbrFinal.vertex", "res/shaders/pbrFinalUntextured.fragment");

	Shader cubemapConversionShader("res/shaders/cubemapAdvanced.vertex", "res/shaders/cubemapConversion.fragment");
	Shader convolutionIrradianceShader("res/shaders/cubemapAdvanced.vertex", "res/shaders/cubemapConvolution.fragment");
	Shader prefilterIBLShader("res/shaders/cubemapAdvanced.vertex", "res/shaders/pbrPrefilter.fragment");
	Shader brdfShader("res/shaders/brdf.vertex", "res/shaders/brdf.fragment");
	Shader backgroundShader("res/shaders/backgroundPBR.vertex", "res/shaders/backgroundPBR.fragment");
	Shader textShader("res/shaders/text.vertex", "res/shaders/text.fragment");
	Shader shader("res/shaders/pbrFinal.vertex", "res/shaders/pbrFinalUntextured.fragment");
	
	// Set up the models used throughout the project
	Model model01("res/models/Raptor/Raptor.obj");
	Model bulb("res/models/Bulb.obj");
	scene.bulb = bulb;

	// This short piece of code sets up the shaders in use when starting the program, as well as updating the on screen text as appropriate.
	if (textured == false && specularIBL == false) { pbrShader = pbrSimpleUntex; isTex = "Texture:- Off"; isSpec = "Specular IBL:- Off"; }
	else if (textured == false && specularIBL == true) { pbrShader = pbrUntex; isTex = "Texture:- Off"; isSpec = "Specular IBL:- On"; }
	else if (textured == true && specularIBL == false) { pbrShader = pbrSimpleTex; isTex = "Texture:- On"; isSpec = "Specular IBL:- Off"; }
	else if (textured == true && specularIBL == true) { pbrShader = pbrTex; isTex = "Texture On:-"; isSpec = "Specular IBL:- On"; }

	// Make sure to set the perspective projection
	projection = glm::perspective(cameras[currentCamera].GetZoom(), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// Set up textures & IBL - make sure to reset the perspective projection here also.
	texName = ibl.ChangeTexture(currentTexture);
	ibl.setUp(cubemapConversionShader, convolutionIrradianceShader, pbrShader, backgroundShader, shader, textured, &projection, brdfShader, prefilterIBLShader);
	projection = glm::perspective(cameras[currentCamera].GetZoom(), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// Enable blending mode as well as setting up freetype for displaying text. Update the viewport as appropriate.
	glEnable(GL_BLEND);
	text.SetupFreeType(textShader, &projection, SCR_WIDTH, SCR_HEIGHT);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
#pragma endregion Setup_Region

	// This is the main game loop, this renders the magic.
    while (!glfwWindowShouldClose(window))
    {
		// call the update method that handles the relevant parameters for each render pass
		Update();

		// This will render the model scene to screen if the current scene is active (2).
		if (currentScene == 2) { obj.RenderModel(pbrShader, model01, &projection, &view, cameras[currentCamera], roughness, metallic); }

		// For each pass we need to make sure the current shader is active to interact with IBL functionality.
		pbrShader = ibl.ChangeShader(pbrTex, pbrUntex, pbrShader, pbrSimpleTex, pbrSimpleUntex, textured, specularIBL, sphereColour);
		
		// Render the correct light scene (which ever one is currently active).
		if (lights == 0) {
			scene.RenderLightScene00(pbrShader);
		}
		else if (lights == 1) {
			scene.RenderLightScene01(pbrShader);
		}
		else if (lights == 2) {
			scene.RenderLightScene02(pbrShader);
		}
		else if (lights == 3) {
			scene.RenderLightScene03(pbrShader);
		}

		// Render the rows & column sphere scene if active.
		if (currentScene == 0 ) { scene.RenderSphereScene01(pbrShader); }

		// Render the single sphere scene when active.
		if (currentScene == 1) { scene.RenderSphereScene02(pbrShader, roughness, metallic, isRotate); }

		// If active (showIrradiance bool) then render the irradiance mapp to screen
		ibl.showMap(backgroundShader, showIrridance, &view, brdfShader);

		// Render the on screen text
		text.RenderTextScene(textShader, hideText, std::to_string(metallic), std::to_string(roughness),texName,isTex,isSpec);

		// Make sure to swap the buffers after each render pass
        glfwSwapBuffers(window);
    }

	// Termination
    glfwTerminate();
    return 0;
}

// Method which handles relevant update logic
void Update() {
	// Here we are making sure to initially disable faceculling, and updating the current frame.
	// Updating of the light positions are handled here also, as are events polled & movement is calculated.
	glDisable(GL_CULL_FACE);
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	lightPositions[0].x = 2 * sin(glfwGetTime()) * 3.0f;
	lightPositions[0].z = 2 * cos(glfwGetTime()) * 2.0f;
	
	lightPositions2[1].y = 2 * sin(glfwGetTime()) * 3.0f;
	lightPositions2[1].z = 2 * sin(glfwGetTime()) * 2.0f;
	
	glfwPollEvents();
	Do_Movement();

	// Clear colour and the buffers - very important
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Make the pbrShader the current active shader and update the various attributes as necessary - view, camPos and projection.
	pbrShader.UseCurrentShader();
	projection = glm::perspective(cameras[currentCamera].GetZoom(), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = cameras[currentCamera].GetViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniform3fv(glGetUniformLocation(pbrShader.Program, "camPos"), 1, &cameras[currentCamera].position[0]);
	glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

#pragma region INPUT_REGION
//This region handles input from various keyboard key input.
bool keys[1024];
bool keysPressed[1024];
void Do_Movement()
{
	// WASD movement - moves the camera around the scene
	if (keys[GLFW_KEY_W])
		cameras[currentCamera].ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		cameras[currentCamera].ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		cameras[currentCamera].ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		cameras[currentCamera].ProcessKeyboard(RIGHT, deltaTime);
	
	// Toggle if the objects are textured
	if (keys[GLFW_KEY_SPACE] && !keysPressed[GLFW_KEY_SPACE])
	{
		textured = !textured;
		if (textured) { isTex = "Texture:- On"; }
		else { isTex = "Texture:- Off"; }

		keysPressed[GLFW_KEY_SPACE] = true;
	}

	// Toggle if the objects have specular IBL applied
	if (keys[GLFW_KEY_ENTER] && !keysPressed[GLFW_KEY_ENTER])
	{
		specularIBL = !specularIBL;
		if (specularIBL) { isSpec = "Specular IBL:- On"; }
		else { isSpec = "Specular IBL:- Off"; }

		keysPressed[GLFW_KEY_ENTER] = true;
	}

	// Cycle through light scenes
	if (keys[GLFW_KEY_J] && !keysPressed[GLFW_KEY_J])
	{
		if (lights == 3) {
			lights = 0;
		}
		else {
			lights++;
		}

		keysPressed[GLFW_KEY_J] = true;
	}

	// Cycle through light scenes
	if (keys[GLFW_KEY_K] && !keysPressed[GLFW_KEY_K])
	{
		if (lights == 0) {
			lights = 3;
		}
		else {
			lights--;
		}

		keysPressed[GLFW_KEY_K] = true;
	}

	// Reset light Colours
	if (keys[GLFW_KEY_T] && !keysPressed[GLFW_KEY_T])
	{
		lightColors[0] = { 300.0f,300.0f,300.0f };
		lightColors1[1] = { 300.0f,300.0f,300.0f };
		lightColors2[1] = { 300.0f,300.0f,300.0f };
		lightColors3[1] = { 300.0f,300.0f,300.0f };


		keysPressed[GLFW_KEY_T] = true;
	}

	// Cycle through the applied untextured sphere base colour
	if (keys[GLFW_KEY_O] && !keysPressed[GLFW_KEY_O])
	{
		if (sphereColour > 8) {
			sphereColour = 0;
		}
		else {
			sphereColour++;
		}
		keysPressed[GLFW_KEY_O] = true;
	}

	// Cycle through the applied untextured sphere base colour
	if (keys[GLFW_KEY_P] && !keysPressed[GLFW_KEY_P])
	{
		if (sphereColour == 0) {
			sphereColour = 8;
		}
		else {
			sphereColour--;
		}
		keysPressed[GLFW_KEY_P] = true;
	}

	// Cycle through the active scene
	if (keys[GLFW_KEY_U] && !keysPressed[GLFW_KEY_U])
	{
		currentScene++;
		if (currentScene > 2) {
			currentScene = 0;
		}
		currentCamera = currentScene;
		keysPressed[GLFW_KEY_U] = true;
	}

	// Cycle through the active scene
	if (keys[GLFW_KEY_Y] && !keysPressed[GLFW_KEY_Y])
	{
		currentScene--;
		if (currentScene < 0) {
			currentScene = 2;
		}
		currentCamera = currentScene;
		keysPressed[GLFW_KEY_Y] = true;
	}

	// Toggle whether the irradiance map is displayed
	if (keys[GLFW_KEY_F] && !keysPressed[GLFW_KEY_F])
	{
		showIrridance = !showIrridance;

		keysPressed[GLFW_KEY_F] = true;
	}

	// Toggle whether on screen text is visible or not
	if (keys[GLFW_KEY_H] && !keysPressed[GLFW_KEY_H])
	{
		hideText = !hideText;

		keysPressed[GLFW_KEY_H] = true;
	}

	// Toggle if sphere is to be under constant rotation
	if (keys[GLFW_KEY_G] && !keysPressed[GLFW_KEY_G])
	{
		isRotate = !isRotate;

		keysPressed[GLFW_KEY_G] = true;
	}

	// Increase Roughness property of object
	if (keys[GLFW_KEY_RIGHT] && !keysPressed[GLFW_KEY_RIGHT])
	{
		roughness += 1;
		if (roughness > 10) 
		{
			roughness = 0;
		}

		keysPressed[GLFW_KEY_RIGHT] = true;
	}

	// Decrease Roughness property of object
	if (keys[GLFW_KEY_LEFT] && !keysPressed[GLFW_KEY_LEFT])
	{
		if (roughness == 0) {
			roughness = 10;
		}
		else {
			roughness -= 1;
		}
		keysPressed[GLFW_KEY_LEFT] = true;
	}

	// Increase Metalness property of object
	if (keys[GLFW_KEY_UP] && !keysPressed[GLFW_KEY_UP])
	{
		metallic += 1;
		if (metallic > 10) 
		{
			metallic = 0;
		}
		keysPressed[GLFW_KEY_UP] = true;
	}

	// Decrease Metalness property of object
	if (keys[GLFW_KEY_DOWN] && !keysPressed[GLFW_KEY_DOWN])
	{
		if (metallic == 0) {
			metallic = 10;
		}
		else {
			metallic -= 1;
		}
		
		keysPressed[GLFW_KEY_DOWN] = true;
	}

	// Cycle through active texture
	if (keys[GLFW_KEY_1] && !keysPressed[GLFW_KEY_1]) {
		currentTexture++;
		if (currentTexture > 20) { currentTexture = 0; }
			texName = ibl.ChangeTexture(currentTexture);
	}

	// Cycle through active texture
	if (keys[GLFW_KEY_2] && !keysPressed[GLFW_KEY_2]) {
		currentTexture--;
		if (currentTexture < 0) { currentTexture = 20; }
		texName =  ibl.ChangeTexture(currentTexture);
	}

	// Cycle through light Colours
	if (keys[GLFW_KEY_Q] && !keysPressed[GLFW_KEY_Q]) {
		lightColors[0] = { 1.0f,300.0f,1.0f };
		lightColors1[1] = { 1.0f,300.0f,1.0f };
		lightColors2[1] = { 1.0f,300.0f,1.0f };
		lightColors3[1] = { 1.0f,300.0f,1.0f };

	}

	// Cycle through light Colours
	if (keys[GLFW_KEY_E] && !keysPressed[GLFW_KEY_E]) {
		lightColors[0] = { 300.0f,1.0f,1.0f };
		lightColors1[1] = { 300.0f,1.0f,1.0f };
		lightColors2[1] = { 300.0f,1.0f,1.0f };
		lightColors3[1] = { 300.0f,1.0f,1.0f };

	}

	// Cycle through light Colours
	if (keys[GLFW_KEY_R] && !keysPressed[GLFW_KEY_R]) {
		lightColors[0] = { 1.0f,1.0f,300.0f };
		lightColors1[1] = { 1.0f,1.0f,300.0f };
		lightColors2[1] = { 1.0f,1.0f,300.0f };
		lightColors3[1] = { 1.0f,1.0f,300.0f };
	}
}


// Handle key touch utility method
void keyTouch(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key <= 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
            keysPressed[key] = false;
        }
    }
}

// Handle mouse touch as appropriate
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
void mouseMovement(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

	cameras[currentCamera].ProcessMouseMovement(xoffset, yoffset); // Reacts to current camera - initially planned func to switch active cameras
}
#pragma endregion
