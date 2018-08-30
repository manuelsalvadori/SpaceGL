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

void Utilities::updateFalcon(glm::mat4 &falcon_transform, const float &deltaX, float &deltaY, float &rotX, float &rotY, float &rotZ)
{
	falcon_transform = glm::translate(falcon_transform, glm::vec3(deltaX, deltaY, 0.0f));
	falcon_transform = glm::rotate(falcon_transform, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
	falcon_transform = glm::rotate(falcon_transform, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	falcon_transform = glm::rotate(falcon_transform, rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
	falcon_transform = glm::scale(falcon_transform, glm::vec3(0.01f, 0.01f, 0.01f));
}

void Utilities::renderScene(Shader shader, Model &falcon, glm::mat4 &falcon_transform, Model &asteroid, glm::mat4 &ast_transform)
{
	renderAsteroids(shader, asteroid, ast_transform);
	renderFalcon(shader, falcon, falcon_transform);
}

void Utilities::renderFalcon(Shader shader, Model &falcon, glm::mat4 &falcon_transform)
{
	glm::mat4 normalMat = glm::inverseTranspose(glm::mat3(falcon_transform));
	shader.setMat4("model", falcon_transform);
	shader.setMat3("normalMat", normalMat);
	falcon.Draw(shader);
}

void Utilities::renderHologram(Shader &holoShader, Model &model)
{
	holoShader.use();
	holoShader.setFloat("g_Time", (float)glfwGetTime());
	holoShader.setFloat("m_GlitchSpeed", 1.0f);
	holoShader.setFloat("m_GlitchIntensity", 1.0f);
	holoShader.setFloat("m_BarSpeed", 0.5f);
	holoShader.setFloat("m_BarDistance", 10.f);
	holoShader.setFloat("m_Alpha", 0.8f);
	holoShader.setFloat("m_FlickerSpeed", 1.5f);
	holoShader.setFloat("m_RimPower", 1.0f);
	holoShader.setFloat("m_GlowSpeed", 1.0f);
	holoShader.setFloat("m_GlowDistance", 0.3f);
	holoShader.setVec4("m_RimColor", glm::vec4(0.6f,0.8f,1.f,1.f));
	holoShader.setVec4("m_MainColor", glm::vec4(0.3f,0.5f,1.f,1.f));

	glm::mat4 t = glm::mat4();
	t = glm::translate(t, glm::vec3(-8.5f,-3.9f,0.f));
	t = glm::rotate(t, (float)glfwGetTime(), glm::vec3(0.f,1.f,0.f));
	t = glm::scale(t, glm::vec3(1.5f, 1.5f, 1.5f));
	glClear(GL_DEPTH_BUFFER_BIT);
	glm::mat4 normalMat = glm::inverseTranspose(glm::mat3(t));
	holoShader.setMat4("model", t);
	holoShader.setMat3("normalMat", normalMat);
	model.Draw(holoShader);
}

void Utilities::renderAsteroids(Shader shader, Model &asteroid, glm::mat4 &ast_transform)
{
	glm::mat4 normalMat = glm::inverseTranspose(glm::mat3(ast_transform));
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

void Utilities::asteroidsTransform(glm::mat4 &ast_transform, glm::vec3 traslate, float rotx, float roty, float rotz, glm::vec3 scale)
{
	ast_transform = glm::translate(ast_transform, traslate);
	ast_transform = glm::rotate(ast_transform, rotx, glm::vec3(1.0f, 0.0f, 0.0f));
	ast_transform = glm::rotate(ast_transform, roty, glm::vec3(0.0f, 1.0f, 0.0f));
	ast_transform = glm::rotate(ast_transform, rotz, glm::vec3(0.0f, 0.0f, 1.0f));
	ast_transform = glm::scale(ast_transform, scale);
}

void Utilities::moveLight(GLFWwindow *window, glm::vec3 &lightPos)
{
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		lightPos.z ++;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		lightPos.z --;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		lightPos.x ++;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		lightPos.x --;
	}
}
