#include "Object.h"

// Object Model constructor
Object::Object() {
}

// This method renders the model scene, taking the relevant paramters to allow for the model to be rendered both textured and untextured
// as well as allowing the user to alter the metalness and roughness paramters accordingly with visual and realtime feedback.
void Object::RenderModel(Shader &pbrShader, Model ourModel, glm::mat4 *projection, glm::mat4 *view, Camera cam, int roughness, int metalness) {
	pbrShader.UseCurrentShader();

	*projection = glm::perspective(cam.GetZoom(), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	*view = cam.GetViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(*projection));
	glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(*view));

	model = glm::mat4();
	model = glm::rotate(model, float(0.1f), glm::vec3(0.0f,0.1f,0.1f));
	model = glm::translate(model, glm::vec3(0.0f, 39.75f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1f(glGetUniformLocation(pbrShader.Program, "metallic"), (float)metalness / (float)10);
	glUniform1f(glGetUniformLocation(pbrShader.Program, "roughness"), glm::clamp((float)roughness / (float)10, 0.05f, 1.0f));
	ourModel.Draw(pbrShader);

}

