#include "Text.h"

// Constructor for the text object
Text::Text() {
}

// This method sets up freetype for use within my project - the orthographic projection is set up here accordingly as is the text shader.
// The library header is initialised, the font is set, as are glyph sizes and storage - loaded in the relevant buffer.
// Character information is then set up - i.e. the textures themselves that represent visual text on screen.
void Text::SetupFreeType(Shader &textShader, glm::mat4 *projection, int WIDTH, int HEIGHT) {
	*projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
	textShader.UseCurrentShader();
	glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(*projection));

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not initialize FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "res/images/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failure in loading font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &textVertexArrayObject);
	glGenBuffers(1, &textVertexBufferObject);
	glBindVertexArray(textVertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, textVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// This method is what actually renders the text on screen using text color attribute set in the shader, the relevant character, buffers and glyphs.
void Text::RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour)
{
	shader.UseCurrentShader();
	glUniform3f(glGetUniformLocation(shader.Program, "textColor"), colour.x, colour.y, colour.z); 
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVertexArrayObject);

	std::string::const_iterator bravo;
	for (bravo = text.begin(); bravo != text.end(); bravo++)
	{
		Character character = Characters[*bravo];

		GLfloat xpos = x + character.baselineOffset.x * scale;
		GLfloat ypos = y - (character.glyphSize.y - character.baselineOffset.y) * scale;
		GLfloat h = character.glyphSize.y * scale;
		GLfloat w = character.glyphSize.x * scale;

		GLfloat textVertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		glBindTexture(GL_TEXTURE_2D, character.GlyphTextureID);
		glBindBuffer(GL_ARRAY_BUFFER, textVertexBufferObject);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(textVertices), textVertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (character.horizontalOffset >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// This scene simple renders a portion of the text on screen using the relevant calls and parameters
void Text::RenderTextScene(Shader &textShader, bool hideText, std::string metalness, std::string roughness, std::string texture, std::string isTextured, std::string isSpecular) {
	glEnable(GL_CULL_FACE);

	if (!hideText) {
		RenderText(textShader, "Control Scheme: -", 25.0f, 700.0f, 0.35f, glm::vec3(0.0, 1.0f, 1.0f));

		RenderText(textShader, "W / A / S / D :- Movement", 25.0f, 665.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "Mouse :- Move Camera", 25.0f, 640.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "*****************************************", 25.0f, 620.0f, 0.3f, glm::vec3(1.0, 1.0f, 1.0f));


		RenderText(textShader, "Up / Down :- Change Metalness", 25.0f, 600.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "Right / LEFT :- Change Roughness", 25.0f, 575.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "*****************************************", 25.0f, 555.0f, 0.3f, glm::vec3(1.0, 1.0f, 1.0f));

		RenderText(textShader, "SPACE :- Toggle Textured Scene", 25.0f, 535.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "1 / 2 :- Toggle Textures", 25.0f, 510.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "*****************************************", 25.0f, 490.0f, 0.3f, glm::vec3(1.0, 1.0f, 1.0f));

		RenderText(textShader, "Q / E / R :- Toggle Light Colour", 25.0f, 470.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "T :- Reset Light Colour", 25.0f, 445.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "F :- Toggle Irradiance Map", 25.0f, 420.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "*****************************************", 25.0f, 400.0f, 0.3f, glm::vec3(1.0, 1.0f, 1.0f));


		RenderText(textShader, "Y / U :- Next/Prev Scene", 25.0f, 380.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "O / P :- Next/Prev Sphere Base Colour", 25.0f, 355.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "G :- Enable/Disable Sphere Rotation", 25.0f, 330.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "*****************************************", 25.0f, 310.0f, 0.3f, glm::vec3(1.0, 1.0f, 1.0f));

		RenderText(textShader, "ENTER :- Toggle Specular IBL", 25.0f, 285.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));
		RenderText(textShader, "J / K :- Next/Prev Lighting Preset", 25.0f, 260.0f, 0.3f, glm::vec3(1.0, 0.4f, 0.0f));

		RenderText(textShader, "Information : -", 1000.0f, 700.0f, 0.35f, glm::vec3(1.0, 1.0f, 1.0f));

		RenderText(textShader, "Metalness : - " + metalness, 1000.0f, 665.0f, 0.3f, glm::vec3(1.0, 1.0f, 1.0f));
		RenderText(textShader, "Roughness : - " + roughness, 1000.0f, 640.0f, 0.3f, glm::vec3(1.0, 1.0f, 1.0f));
		RenderText(textShader, isTextured, 1000.0f, 615.0f, 0.3f, glm::vec3(1.0, 1.0f, 1.0f));
		if (isTextured == "Texture:- On")
		RenderText(textShader, "Texture : -   " + texture, 1000.0f, 590.0f, 0.3f, glm::vec3(1.0, 1.0f, 1.0f));
		RenderText(textShader,  isSpecular, 1000.0f, 565.0f, 0.3f, glm::vec3(1.0, 1.0f, 1.0f));

	}
}

// This scene simple renders a portion of the text on screen using the relevant calls and parameters
void Text::RenderTextScene02(Shader &textShader) {
			glEnable(GL_CULL_FACE);
			RenderText(textShader, "Loading Textures...Please Wait....", 25.0f, 700.0f, 0.55f, glm::vec3(1.0, 0.0f, 0.0f));
	}
