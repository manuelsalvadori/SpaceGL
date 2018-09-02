
#include "Asteroid.h"

Asteroid::Asteroid(): boundSphere(), rotX(), rotY(), rotZ(), alpha(), scaleK(), depthMap(){}
Asteroid::Asteroid(const glm::vec3 &lightPos, const glm::vec3 &camPos, const glm::mat4 &view_matrix,
		const glm::mat4 &projection_matrix, const glm::mat4 &lightSpaceMatrix, const glm::vec3 &ambientColor,
		const glm::vec3 &diffuseColor, unsigned int depthMap): alpha(0.0f), depthMap(depthMap)
{
	shader = Shader("src/shaders/matAsteroid.vs","src/shaders/matAsteroid.fs");
	shadow = Shader("src/shaders/shadowAst.vs","src/shaders/shadowMap.fs");
	shader.use();
	shader.setVec3("light.position", lightPos);
	shader.setVec3("lightPos", lightPos);
	shader.setVec3("viewPos", camPos);
	shader.setMat4("view", view_matrix);
	shader.setMat4("projection", projection_matrix);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	shader.setVec3("light.ambient", ambientColor);
	shader.setVec3("light.diffuse", diffuseColor);
	shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// material properties
	shader.setVec3("material.ambient", 0.3f, 0.3f, 0.3f);
	shader.setVec3("material.diffuse", 0.84f, 0.84f, 0.84f);
	shader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);
	shader.setFloat("material.shininess", 22.0f);
	shader.setFloat("alpha", 0.0f);

	// displacement parameters
	float p1 = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));
	float p2 = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));
	float p3 = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));
	float p4 = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));
	float noiseK = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));

	shader.setFloat("p1", p1);
	shader.setFloat("p2", p2);
	shader.setFloat("p3", p3);
	shader.setFloat("p4", p4);
	shader.setFloat("noiseK", noiseK);
	shader.setFloat("r", 0.1f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.9f))));

//	shadow.use();
//	shadow.setMat4("lightSpaceMatrix", lightSpaceMatrix);
//	shadow.setFloat("p1", p1);
//	shadow.setFloat("p2", p2);
//	shadow.setFloat("p3", p3);
//	shadow.setFloat("p4", p4);
//	shadow.setFloat("noiseK", noiseK);

	rotX = 0.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));
	rotY = 0.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));
	rotZ = 0.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));

	scaleK = 0.8f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.7f)));

	scale = glm::vec3();
	scale.y = 0.25f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.5f)));
	scale.x = 0.25f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.5f)));
	scale.z = 0.25f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.5f)));
	scale *= scaleK;
	transform = glm::mat4();
	position = glm::vec3();
	position.y = -5.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(11.0f)));
	position.x = -7.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(15.0f)));
	position.z = -141.f - count * 35.0f;

	boundSphere = 1.8f * scaleK * (scale.x + scale.y + scale.z)/3.f;
	shader.setFloat("sphere", boundSphere);

	count ++;
}

Asteroid::~Asteroid(){}

void Asteroid::updateTransform()
{
	position.z += 2.0f;
	if(position.z > 5)
	{
		hitted = false;
		alpha = 0.0f;
		position.z = -160.0f;
		position.y = -4.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(10.0f)));
		position.x = -7.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(15.0f)));

		scaleK = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.5f)));
		scale.y = 0.25f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.5f)));
		scale.x = 0.25f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.5f)));
		scale.z = 0.25f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.5f)));
		scale *= scaleK;
		shader.use();
		boundSphere = 1.8f * scaleK * (scale.x + scale.y + scale.z)/3.f;
		shader.setFloat("sphere", boundSphere);

		// displacement parameters
		float p1 = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));
		float p2 = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));
		float p3 = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));
		float p4 = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));
		float noiseK = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0f)));

		shader.setFloat("p1", p1);
		shader.setFloat("p2", p2);
		shader.setFloat("p3", p3);
		shader.setFloat("p4", p4);
		shader.setFloat("noiseK", noiseK);
		shader.setFloat("r", 0.1f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.9f))));

//		shadow.use();
//		shadow.setFloat("p1", p1);
//		shadow.setFloat("p2", p2);
//		shadow.setFloat("p3", p3);
//		shadow.setFloat("p4", p4);
//		shadow.setFloat("noiseK", noiseK);
	}

	if(alpha < 1.0)
	{
		if(position.z > -140)
			alpha += 0.04;
		else
			alpha += 0.01f;
	}

	//cout << "alpha: " << alpha  << " @ " << position.z << endl;
	transform = glm::mat4();
	transform = glm::translate(transform, position);
	transform = glm::rotate(transform, (float)glfwGetTime()*rotX, glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, (float)glfwGetTime()*rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, (float)glfwGetTime()*rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, scale);
}

void Asteroid::Draw(Model &model)
{
	if(hitted) return;
	shader.use();
	shader.setFloat("alpha", alpha);
	shader.setMat4("model", transform);
	shader.setMat3("normalMat", glm::inverseTranspose(glm::mat3(transform)));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	model.Draw(shader);
}

void Asteroid::DrawShadow(Model &model)
{
	if(hitted || position.z < -20.f) return;
	shadow.use();
	shadow.setMat4("model", transform);
	shader.setMat3("normalMat", glm::inverseTranspose(glm::mat3(transform)));
	model.Draw(shadow);
}

bool Asteroid::checkCollisionFalcon(const glm::vec3 &pos)
{
	if(hitted) return false;
	return hitted = (glm::distance(position, pos + glm::vec3(0, 0, 2.2f)) < (2 + boundSphere));
}

bool Asteroid::checkCollisionLaser(const glm::vec3 &pos)
{
	if(hitted) return false;
	return hitted = (glm::distance(position, pos) < (1.f + boundSphere));
}

int Asteroid::count = 0;

