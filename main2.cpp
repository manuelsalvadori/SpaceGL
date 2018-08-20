// Include standard headers
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

using namespace glm;

GLFWwindow* window;

int main( void )
{

	Utilities::GLWF_init();
	window = Utilities::createWindow();
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window,int h, int w){glfwGetWindowSize(window, &w, &h); glViewport(0, 0, w, h);}); //lambda
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	Utilities::GLEW_init();

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_CULL_FACE); // backface culling
	glEnable(GL_DEPTH_TEST); // z-buffer
	glDepthFunc(GL_LESS); // z-buffer

	Shader simpleShader = Shader( "src/SimpleVertexShader.vertexshader", "src/SimpleFragmentShader.fragmentshader");
	GLuint simpleShaderID = simpleShader.getID();

	GLuint MatrixID = glGetUniformLocation(simpleShaderID, "MVP"); // Get a handle for our "MVP" uniform

	glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), 16.0f/9.0f, 0.1f, 100.0f); // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

	glm::mat4 view_matrix = glm::lookAt(glm::vec3(0,0,15), glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat4 model_matrix = glm::translate(glm::mat4(), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 MVP = projection_matrix * view_matrix * model_matrix;

	Model nanosuit("src/nanosuit/nanosuit.obj");
	Model falcon("src/falcon/Millennium_Falcon.obj");
	Model land("src/falcon/cube.obj");
	float delta = -1.0f;
	float rot = 0.0f;

	while(glfwWindowShouldClose(window) == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

		glUseProgram(simpleShaderID); // Use our shader

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		projection_matrix = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
		MVP = projection_matrix * view_matrix * model_matrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE,glm::value_ptr(MVP));// &MVP[0][0] oppure glm::value_ptr(MVP)

		glm::mat4 model; // sarebbe interessante creare una classe transform che contiene le varie matrix di ogni modello
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		MVP = projection_matrix * view_matrix * model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(MVP));
		nanosuit.Draw(simpleShader);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			delta += 0.1f;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			delta -= 0.1f;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			rot += 0.1f;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			rot -= 0.1f;
		std::cout << delta << endl;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.0f, delta, -2.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, rot, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		MVP = projection_matrix * view_matrix * model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(MVP));
		falcon.Draw(simpleShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, -8.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		MVP = projection_matrix * view_matrix * model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(MVP));
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




