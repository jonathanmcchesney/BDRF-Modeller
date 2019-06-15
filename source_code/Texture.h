
#ifndef TEXTURE_H
#define TEXTURE_H
#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include "SOIL2\SOIL2.h"

using std::vector;

// Helper Texture class for loading in Textures
class TextureLoading
{

public:
	static GLuint LoadTexture(GLchar *path, GLboolean alpha);
	static GLuint LoadCubemap(vector<const GLchar * > faces);
};

#endif 
