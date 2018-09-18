
#include "Target.h"

Target::Target(const glm::mat4 &view_matrix, const glm::mat4 &projection_matrix)
{
	shader = Shader("src/shaders/matSky.vs","src/shaders/matSky.fs");
	text1 = Utilities::loadTexture("src/numbers/mirino01.png");
	shader.use();
	shader.setMat4("view", view_matrix);
	shader.setMat4("projection", projection_matrix);
}

Target::~Target()
{
	glDeleteProgram(shader.getID());
}

void Target::renderTarget(Model &model, const float &deltaX, float &deltaY)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glm::mat4 transform = glm::mat4();
	transform = glm::translate(transform, glm::vec3(deltaX, deltaY, -18));
	//transform = glm::scale(transform, glm::vec3(3.f));
	shader.use();
	shader.setMat4("model", transform);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text1);
	model.Draw(shader);

	transform = glm::translate(transform, glm::vec3(0, 0, 2));

	shader.setMat4("model", transform);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text1);
	model.Draw(shader);

}

