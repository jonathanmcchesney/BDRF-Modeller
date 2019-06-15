#ifndef Constants_HEADER
#define Constants_HEADER

// Constants and variables class that contains relevant values.
const GLuint SCR_WIDTH = 1280, SCR_HEIGHT = 720;
bool textured = false, showIrridance = false, hideText = false, isRotate = false, specularIBL = true, texturesLoaded = false;
unsigned int envCubemap, irradianceMap, prefilterMap, brdfLUTTexture;
int currentCamera = 0, currentTexture = 0, currentScene = 0, sphereColour = 0, lights = 0;
GLuint albedo, normal, metallic = 0, roughness = 0, ao;
GLfloat deltaTime = 0.0f, lastFrame = 0.0f;
Shader pbrShader, pbrShader1, pbrShader2;
string texName, isTex, isSpec;

// matricies :: MVP
glm::mat4 model, view, projection;

// Light positions and colours: will change over time in certain scenes
glm::vec3 lightPositions[] =  { glm::vec3(-10.0f,  30.0f, 10.0f), glm::vec3(10.0f,  30.0f, 10.0f) }; 
glm::vec3 lightPositions1[] = { glm::vec3(0.0f,  26.0f, 6.0f), glm::vec3(0.0f,  16.0f, 6.0f) };
glm::vec3 lightPositions2[] = { glm::vec3(-10.0f,  30.0f, 5.0f), glm::vec3(-10.0f,  10.0f, 5.0f)};
glm::vec3 lightPositions3[] = { glm::vec3(-8.0f,  23.0f, 0.0f), glm::vec3(8.0f,  17.0f, 4.0f)};

glm::vec3 lightColors[] =  { glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f) }; 
glm::vec3 lightColors1[] = { glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f) };
glm::vec3 lightColors2[] = { glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f) };
glm::vec3 lightColors3[] = { glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f) };

#endif Constants_HEADER
