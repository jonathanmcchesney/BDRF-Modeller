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

#include <ft2build.h>
#include FT_FREETYPE_H  

class Text
{
	// This class is the header for Text output-  it uses the 3rd party library Freetype to achieve optimal results.
	// Public and private access is handled accordingly as are a few predefined scenes for text output.
public:
	Text::Text();
	void Text::SetupFreeType(Shader &textShader, glm::mat4 *projection, int WIDTH, int HEIGHT);
	void Text::RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour);
	void Text::RenderTextScene(Shader &textShader, bool hideText, std::string metalness, std::string roughness, std::string texture, std::string isTextured, std::string isSpecular);
	void Text::RenderTextScene02(Shader &textShader);

private:
	Shader shader;
	struct Character {
		GLuint GlyphTextureID;
		glm::ivec2 glyphSize;
		glm::ivec2 baselineOffset;
		GLuint horizontalOffset;
	};
	std::map<GLchar, Character> Characters;
	GLuint	textVertexArrayObject = 0, textVertexBufferObject;

	glm::mat4 *view, *projection, model;
	Camera cam;
};



