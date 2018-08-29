
#include "Asteroid.h"

Asteroid::Asteroid(){}
Asteroid::Asteroid(const glm::vec3 &lightPos, const glm::vec3 &camPos, const glm::mat4 &view_matrix,
		const glm::mat4 &projection_matrix, const glm::mat4 &lightSpaceMatrix, const glm::vec3 &ambientColor,
		const glm::vec3 &diffuseColor, unsigned int depthMap): alpha(0.0f), depthMap(depthMap)
{
	shader = Shader("src/shaders/matAsteroid.vs","src/shaders/matAsteroid.fs");
	shader.use();
	shader.setVec3("light.position", lightPos);
	shader.setVec3("lightPos", lightPos);
	shader.setVec3("viewPos", camPos);
	shader.setMat4("view", view_matrix);
	shader.setMat4("projection", projection_matrix);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	shader.setVec3("light.ambient", ambientColor);
	shader.setVec3("light.diffuse", diffuseColor);
	cout << "r:" << diffuseColor.x << " g:" << diffuseColor.y << " b:" << diffuseColor.z << endl;
	shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// material properties
	shader.setVec3("material.ambient", 0.3f, 0.3f, 0.3f);
	shader.setVec3("material.diffuse", 0.84f, 0.84f, 0.84f);
	shader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);
	shader.setFloat("material.shininess", 22.0f);
	shader.setFloat("alpha", 0.0f);

	rotX = 0.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f-0.0f)));
	rotY = 0.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f-0.0f)));
	rotZ = 0.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f-0.0f)));

	scale = glm::vec3(1.f);
	transform = glm::mat4();
	position = glm::vec3();
	position.y = -3.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(6.0f)));
	position.x = -4.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(8.0f)));
	position.z = -160.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(20.0f)));
}

Asteroid::~Asteroid() {}

void Asteroid::updateTransform()
{
	position.z += 2.0f;
	if(position.z > 10)
	{
		alpha = 0.0f;
		position.z = -140.0f;
		position.y = -3.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(6.0f)));
		position.x = -4.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(8.0f)));
	}
	if(alpha < 1.0)
		alpha += 0.1f;

	transform = glm::mat4();
	transform = glm::translate(transform, position);
	transform = glm::rotate(transform, (float)glfwGetTime()*rotX, glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, (float)glfwGetTime()*rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, (float)glfwGetTime()*rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, scale);
}

void Asteroid::Draw(Model &model)
{
	shader.use();
	shader.setFloat("alpha", alpha);
	shader.setMat4("model", transform);
	shader.setMat3("normalMat", glm::inverseTranspose(glm::mat3(transform)));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	model.Draw(shader);
}

