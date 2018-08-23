#include "Utilities.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>


void Utilities::GLWF_init()
{
	if(!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		getchar();
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL
}

void Utilities::GLEW_init()
{
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		getchar();
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}

GLFWwindow* Utilities::createWindow()
{
	GLFWwindow* window = glfwCreateWindow(1152, 648, "OpenGL 3.3", NULL, NULL);
	if(window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		getchar();
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	return window;
}

void Utilities::movementHandler(GLFWwindow *window, float &deltaTime, float &rotSpeed, float &deltaX, float &deltaY, float &rotX, float &rotZ)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if(deltaY < 3.7f)
			deltaY += 7.5f * deltaTime;
		if(rotX < 0.3f)
			rotX += rotSpeed * deltaTime;
	}
	else
		if(rotX > 0.0f)
			rotX -= rotSpeed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if(deltaY > -5.5f)
			deltaY -= 7.5f * deltaTime;
		if(rotX > -0.3f)
			rotX -= rotSpeed * deltaTime;
	}
	else
		if(rotX < 0.0f)
			rotX += rotSpeed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		if(deltaX < 7.5f)
			deltaX += 7.5f * deltaTime;
		if(rotZ < 0.3f)
			rotZ += rotSpeed * deltaTime;
	}
	else
		if(rotZ > 0.0f)
			rotZ -= rotSpeed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if(deltaX > -7.5f)
			deltaX -= 7.5f * deltaTime;
		if(rotZ > -0.3f)
			rotZ -= rotSpeed * deltaTime;
	}
	else
		if(rotZ < 0.0f)
			rotZ += rotSpeed * deltaTime;
}

void Utilities::renderScene(Shader shader, Model &falcon, glm::mat4 &falcon_transform, Model &asteroid, glm::mat4 &ast_transform)
{

	glm::mat4 normalMat = glm::inverseTranspose(glm::mat3(ast_transform));
	shader.setMat4("model", ast_transform);
	shader.setMat3("normalMat", normalMat);
	asteroid.Draw(shader);

	ast_transform = glm::translate(ast_transform, glm::vec3(-2.f,0.f,0.f));
	normalMat = glm::inverseTranspose(glm::mat3(ast_transform));
	shader.setMat4("model", ast_transform);
	shader.setMat3("normalMat", normalMat);
	asteroid.Draw(shader);

	normalMat = glm::inverseTranspose(glm::mat3(falcon_transform));
	shader.setMat4("model", falcon_transform);
	shader.setMat3("normalMat", normalMat);
	falcon.Draw(shader);
}

void Utilities::renderFalcon(Shader shader, Model &falcon, glm::mat4 &falcon_transform)
{

	glm::mat4 normalMat = glm::inverseTranspose(glm::mat3(falcon_transform));
	shader.setMat4("model", falcon_transform);
	shader.setMat3("normalMat", normalMat);
	falcon.Draw(shader);
}

void Utilities::renderAsteroids(Shader shader, Model &asteroid, glm::mat4 &ast_transform)
{

	glm::mat4 normalMat = glm::inverseTranspose(glm::mat3(ast_transform));
	shader.setMat4("model", ast_transform);
	shader.setMat3("normalMat", normalMat);
	asteroid.Draw(shader);

	ast_transform = glm::translate(ast_transform, glm::vec3(-2.f,0.f,0.f));
	normalMat = glm::inverseTranspose(glm::mat3(ast_transform));
	shader.setMat4("model", ast_transform);
	shader.setMat3("normalMat", normalMat);
	asteroid.Draw(shader);

}

void Utilities::renderLand(Shader shader, Model &land, glm::mat4 &land_transform)
{

	glm::mat4 normalMat = glm::inverseTranspose(glm::mat3(land_transform));
	shader.setMat4("model", land_transform);
	shader.setMat3("normalMat", normalMat);
	land.Draw(shader);
}

