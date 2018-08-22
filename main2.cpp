#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "src/Utilities.h"
#include "src/Shader.h"
#include "src/Model.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace glm;

GLFWwindow* window;

int main( void )
{

	Utilities::GLWF_init();
	window = Utilities::createWindow();
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window,int h, int w){glfwGetWindowSize(window, &w, &h); glViewport(0, 0, w, h);}); //lambda
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	int width, height;

	Utilities::GLEW_init();

	glClearColor(0.1f, 0.0f, 0.1f, 0.0f);

	glEnable(GL_CULL_FACE); // backface culling
	glEnable(GL_DEPTH_TEST); // z-buffer
	glDepthFunc(GL_LESS); // z-buffer

	Shader simpleShader = Shader( "src/matBlinnPhong.vs", "src/matBlinnPhong.fs");
	GLuint simpleShaderID = simpleShader.getID();

	glm::vec3 ligthPos = glm::vec3(3,2,2);
	glm::vec3 camPos = glm::vec3(0,1,15);

	glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), 16.0f/9.0f, 0.1f, 100.0f); // Projection matrix : Field of View, screen ratio, display range : near <-> far planes
	glm::mat4 view_matrix = glm::lookAt(camPos, glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat3 normalMat;

	Model asteroid("src/asteroid/icosphere.obj");
	Model falcon("src/falcon/Millennium_Falcon.obj");
	Model land("src/falcon/cube.obj");

	float deltaY = -4.0f;
	float deltaX = 0.0f;
	float rotY = 3.14159f;
	float rotX = 0.0f;
	float rotZ = 0.0f;
	float rotSpeed = 1.0f;

	float deltaTime = 0.0f, lastFrame = 0.0f;
	float shininess = 300.0f;

	// game loop -----------------------------------------------------------------------------------------------------------

	while(glfwWindowShouldClose(window) == 0)
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
		glfwGetWindowSize(window, &width, &height);

		projection_matrix = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

		glm::mat4 transform;
		transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		transform = glm::scale(transform, glm::vec3(2.0f, 2.0f, 2.0f));
		normalMat = glm::inverseTranspose(glm::mat3(transform));

		simpleShader.use();
		simpleShader.setVec3("light.position", ligthPos);
		simpleShader.setVec3("viewPos", camPos);

		// light properties
		glm::vec3 lightColor;
		lightColor.x = 1.0f;
		lightColor.y = 1.0f;
		lightColor.z = 1.0f;
		glm::vec3 diffuseColor = lightColor   * glm::vec3(0.8f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.4f); // low influence
		simpleShader.setVec3("light.ambient", ambientColor);
		simpleShader.setVec3("light.diffuse", diffuseColor);
		simpleShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		simpleShader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
		simpleShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
		simpleShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			shininess ++;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			shininess --;
		std::cout << "s: " << shininess << std::endl;
		simpleShader.setFloat("material.shininess", shininess);

		// matrices
		//transform = glm::rotate(transform, (float)glfwGetTime()*9, glm::vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		//transform = glm::rotate(transform, (float)glfwGetTime()*12, glm::vec3(0.0f, 0.0f, 1.0f));
		normalMat = glm::inverseTranspose(glm::mat3(transform));

		simpleShader.setMat4("model", transform);
		simpleShader.setMat4("view", view_matrix);
		simpleShader.setMat4("projection", projection_matrix);
		simpleShader.setMat3("normalMat", normalMat);
		asteroid.Draw(simpleShader);

		// falcon movement
		Utilities::movementHandler(window, deltaTime, rotSpeed, deltaX, deltaY, rotX, rotZ);

		// falcon
		transform = glm::mat4();
		transform = glm::translate(transform, glm::vec3(deltaX, deltaY, 0.0f));
		transform = glm::rotate(transform, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
		transform = glm::rotate(transform, rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(0.01f, 0.01f, 0.01f));
		normalMat = glm::inverseTranspose(glm::mat3(transform));
		simpleShader.use();
		simpleShader.setMat4("model", transform);
		simpleShader.setMat4("view", view_matrix);
		simpleShader.setMat4("projection", projection_matrix);
		simpleShader.setMat3("normalMat", normalMat);
		falcon.Draw(simpleShader);

		// land
		transform = glm::mat4();
		transform = glm::translate(transform, glm::vec3(0.0f, -8.0f, -1000+glfwGetTime()*100));
		transform = glm::scale(transform, glm::vec3(1000.0f, 30.0f, 1000.0f));
		normalMat = glm::inverseTranspose(glm::mat3(transform));
		simpleShader.use();
		simpleShader.setMat4("model", transform);
		simpleShader.setMat4("view", view_matrix);
		simpleShader.setMat4("projection", projection_matrix);
		simpleShader.setMat3("normalMat", normalMat);
		land.Draw(simpleShader);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	// Cleanup VBO and shader
	glDeleteProgram(simpleShaderID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}




