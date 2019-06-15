#include "Scenes.h"

Scenes::Scenes() {
}
glm::vec3 lightPositions[] = { glm::vec3(-10.0f,  50.0f, 10.0f), glm::vec3(10.0f,  50.0f, 10.0f) };
glm::vec3 lightPositions1[] = { glm::vec3(0.0f,  46.0f, 6.0f), glm::vec3(0.0f,  36.0f, 6.0f) };
glm::vec3 lightPositions2[] = { glm::vec3(-10.0f,  50.0f, 5.0f), glm::vec3(-10.0f,  50.0f, 5.0f) };
glm::vec3 lightPositions3[] = { glm::vec3(-8.0f,  43.0f, 0.0f), glm::vec3(8.0f,  37.0f, 4.0f) };

glm::vec3 lightColors[] = { glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f) };
glm::vec3 lightColors1[] = { glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f) };
glm::vec3 lightColors2[] = { glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f) };
glm::vec3 lightColors3[] = { glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f) };

void Scenes::RenderSphereScene02(Shader &pbrShader, int roughness, int metalness, bool rotate) {
	
	glUniform1f(glGetUniformLocation(pbrShader.Program, "metallic"), (float)metalness/ (float)10);
	glUniform1f(glGetUniformLocation(pbrShader.Program, "roughness"), glm::clamp((float)roughness/ (float)10, 0.05f, 1.0f));
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 40.0f, -2.0f));
	if (rotate == true) 
	{
		model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(0, 1, 0));
		model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(1, 0, 0));
		//model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(0, 0, 1));
	}
	glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	sphere.renderSphere();
	model = glm::mat4();
}


void Scenes::RenderSphereScene01(Shader &pbrShader) {
	for (int row = 0; row < nrRows; ++row)
	{
		glUniform1f(glGetUniformLocation(pbrShader.Program, "metallic"), (float)row / (float)nrRows);
		for (int col = 0; col < nrColumns; ++col)
		{
			glUniform1f(glGetUniformLocation(pbrShader.Program, "roughness"), glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(
				(float)(col - (nrColumns / 2)) * spacing,
				(float)(row - (nrRows / 2)) * spacing + 40,
				-2.0f
			));
			glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			sphere.renderSphere();
		}
	}
}

void Scenes::RenderLightScene00(Shader &pbrShader) {
	for (int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[i];
		glUniform3fv(glGetUniformLocation(pbrShader.Program, ("lightPositions[" + std::to_string(i) + "]").c_str()), 1, &newPos[0]);
		glUniform3fv(glGetUniformLocation(pbrShader.Program, ("lightColors[" + std::to_string(i) + "]").c_str()), 1, &lightColors[i][0]);

		model = glm::mat4();
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bulb.Draw(pbrShader);
	}
}

void Scenes::RenderLightScene02(Shader &pbrShader) {
	for (int i = 0; i < sizeof(lightPositions2) / sizeof(lightPositions2[0]); ++i)
	{
		glm::vec3 newPos = lightPositions2[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions2[i];
		glUniform3fv(glGetUniformLocation(pbrShader.Program, ("lightPositions[" + std::to_string(i) + "]").c_str()), 1, &newPos[0]);
		glUniform3fv(glGetUniformLocation(pbrShader.Program, ("lightColors[" + std::to_string(i) + "]").c_str()), 1, &lightColors2[i][0]);

		model = glm::mat4();
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bulb.Draw(pbrShader);
	}
}

void Scenes::RenderLightScene01(Shader &pbrShader) {
	for (int i = 0; i < sizeof(lightPositions1) / sizeof(lightPositions1[0]); ++i)
	{
		glm::vec3 newPos = lightPositions1[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions1[i];
		glUniform3fv(glGetUniformLocation(pbrShader.Program, ("lightPositions[" + std::to_string(i) + "]").c_str()), 1, &newPos[0]);
		glUniform3fv(glGetUniformLocation(pbrShader.Program, ("lightColors[" + std::to_string(i) + "]").c_str()), 1, &lightColors1[i][0]);

		model = glm::mat4();
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bulb.Draw(pbrShader);
	}
}

void Scenes::RenderLightScene03(Shader &pbrShader) {
	for (int i = 0; i < sizeof(lightPositions3) / sizeof(lightPositions3[0]); ++i)
	{
		glm::vec3 newPos = lightPositions3[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions3[i];
		glUniform3fv(glGetUniformLocation(pbrShader.Program, ("lightPositions[" + std::to_string(i) + "]").c_str()), 1, &newPos[0]);
		glUniform3fv(glGetUniformLocation(pbrShader.Program, ("lightColors[" + std::to_string(i) + "]").c_str()), 1, &lightColors3[i][0]);

		model = glm::mat4();
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bulb.Draw(pbrShader);
	}
}