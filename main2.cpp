#include <stdio.h>
#include <stdlib.h>
#include "src/stb_image.h"
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
void renderQuad();

int main( void )
{

	Utilities::GLWF_init();
	window = Utilities::createWindow();
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window,int h, int w){glfwGetWindowSize(window, &w, &h); glViewport(0, 0, w, h);}); //lambda
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	int width = 1152, height = 648;

	Utilities::GLEW_init();

	glClearColor(0.1f, 0.0f, 0.1f, 0.0f);

	glEnable(GL_MULTISAMPLE); // anti-aliasing
	glEnable(GL_CULL_FACE); // backface culling
	glEnable(GL_DEPTH_TEST); // z-buffer
	glDepthFunc(GL_LESS); // z-buffer

	Shader simpleShader = Shader("src/shaders/matBlinnPhong.vs", "src/shaders/matBlinnPhong.fs");
	Shader landShader = Shader("src/shaders/matLand.vs","src/shaders/matLand.fs");
	Shader asteroidShader = Shader("src/shaders/matAsteroid.vs","src/shaders/matAsteroid.fs");
	Shader shadowShader = Shader("src/shaders/shadowMap.vs","src/shaders/shadowMap.fs");
	Shader skyShader = Shader("src/shaders/matSky.vs","src/shaders/matSky.fs");
	Shader blurShader = Shader("src/shaders/blur.vs","src/shaders/blur.fs");
	Shader bloomShader = Shader("src/shaders/bloom.vs","src/shaders/bloom.fs");
	//Shader debugDepthQuad = Shader("src/shaders/debug.vs","src/shaders/debug.fs");
	GLuint simpleShaderID = simpleShader.getID();

	glm::vec3 lightPos = glm::vec3(46,77,86);
	glm::vec3 camPos = glm::vec3(0,1,15);

	glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), 16.0f/9.0f, 0.1f, 100.0f); // Projection matrix : Field of View, screen ratio, display range : near <-> far planes
	glm::mat4 view_matrix = glm::lookAt(camPos, glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat3 normalMat;

	Model asteroid("src/asteroid/icosphere2.obj");
	glm::mat4 ast_transform;
	Model falcon("src/falcon/Millennium_Falcon.obj");
	glm::mat4 falcon_transform;
	Model land("src/falcon/cube.obj");
	glm::mat4 land_transform;
	Model sky("src/skybox/sky.obj");
	glm::mat4 sky_transform = glm::mat4();

	float deltaY = -4.0f;
	float deltaX = 0.0f;
	float rotY = 3.14159f;
	float rotX = 0.0f;
	float rotZ = 0.0f;
	float rotSpeed = 1.0f;

	float deltaTime = 0.0f, lastFrame = 0.0f;
	float shininess = 300.0f;

	bool bloom = true;

	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// configure (floating point) framebuffers
	// ---------------------------------------
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	unsigned int colorBuffers[2];
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}


	// shader configuration
	// --------------------
	skyShader.use();
	sky_transform = glm::mat4();
	sky_transform = glm::scale(sky_transform, glm::vec3(20.5f, 20.5f, 1.f));
	sky_transform = glm::translate(sky_transform, glm::vec3(0.f, 0.f, -140.f));
	skyShader.setMat4("model", sky_transform);
	simpleShader.use();
	simpleShader.setInt("shadowMap", 0);
	landShader.use();
	landShader.setInt("shadowMap", 0);
	blurShader.use();
	blurShader.setInt("image", 0);
	bloomShader.use();
	bloomShader.setInt("scene", 0);
	bloomShader.setInt("bloomBlur", 1);
	// visual debug shadow
	//	debugDepthQuad.use();
	//	debugDepthQuad.setInt("depthMap", 0);

	// game loop -----------------------------------------------------------------------------------------------------------

	float z = 0.f;
	while(glfwWindowShouldClose(window) == 0)
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//cout << 1.f/deltaTime << endl;

		// transformation matrices
		ast_transform = glm::mat4();
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		{
			z ++;
		}
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		{
			z --;
		}
		//std::cout << z << endl;
		Utilities::asteroidsTransform(ast_transform, glm::vec3(0.f, 0.f,z), (float)glfwGetTime(), (float)glfwGetTime()*0.5f, (float)glfwGetTime()*0.2f, glm::vec3(1.f,0.6f,1.f));
		//Utilities::asteroidsTransform(ast_transform, glm::vec3(0.f, 0.f,z), 0.f,0.f,0.f);

		// falcon movement
		Utilities::movementHandler(window, deltaTime, rotSpeed, deltaX, deltaY, rotX, rotZ);

		// falcon
		falcon_transform = glm::mat4();
		falcon_transform = glm::translate(falcon_transform, glm::vec3(deltaX, deltaY, 0.0f));
		falcon_transform = glm::rotate(falcon_transform, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
		falcon_transform = glm::rotate(falcon_transform, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
		falcon_transform = glm::rotate(falcon_transform, rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
		falcon_transform = glm::scale(falcon_transform, glm::vec3(0.01f, 0.01f, 0.01f));

		// land
		land_transform = glm::mat4();
		land_transform = glm::translate(land_transform, glm::vec3(0.0f, -11.f, 4.0f));
		land_transform = glm::rotate(land_transform, 0.1f, glm::vec3(1.0f, 0.f, 0.0f));
		land_transform = glm::scale(land_transform, glm::vec3(40.0f, 10.0f, 40.0f));

		Utilities::moveLight(window, lightPos);

		// depth texture

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 50.0f, far_plane = 275.0f;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		shadowShader.use();
		shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		Utilities::renderScene(shadowShader, falcon, falcon_transform, asteroid, ast_transform);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. render scene as normal using the generated depth/shadow map
		// regular rendering -------------------------------------------------------------------------------------------

		glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
		glfwGetWindowSize(window, &width, &height);
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// shader setting
		projection_matrix = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 180.0f);
		skyShader.use();
		skyShader.setMat4("view", view_matrix);
		skyShader.setMat4("projection", projection_matrix);

		simpleShader.use();
		simpleShader.setVec3("light.position", lightPos);
		simpleShader.setVec3("lightPos", lightPos);
		simpleShader.setVec3("viewPos", camPos);
		simpleShader.setMat4("view", view_matrix);
		simpleShader.setMat4("projection", projection_matrix);

		simpleShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// light properties
		glm::vec3 lightColor;
		lightColor.x = 1.0f;
		lightColor.y = 1.0f;
		lightColor.z = 0.95f;
		glm::vec3 diffuseColor = lightColor   * glm::vec3(0.8f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.4f); // low influence
		simpleShader.setVec3("light.ambient", ambientColor);
		simpleShader.setVec3("light.diffuse", diffuseColor);
		simpleShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		//		simpleShader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
		//		simpleShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
		//		simpleShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		simpleShader.setVec3("material.ambient", 0.3f, 0.3f, 0.3f);
		simpleShader.setVec3("material.diffuse", 0.74f, 0.74f, 0.74f);
		simpleShader.setVec3("material.specular", 0.6f, 0.6f, 0.6f);
		simpleShader.setFloat("material.shininess", shininess);
		simpleShader.setFloat("time", (float)glfwGetTime());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		Utilities::renderFalcon(simpleShader, falcon, falcon_transform);

		// render asteroids
		asteroidShader.use();
		asteroidShader.setVec3("light.position", lightPos);
		asteroidShader.setVec3("lightPos", lightPos);
		asteroidShader.setVec3("viewPos", camPos);
		asteroidShader.setMat4("view", view_matrix);
		asteroidShader.setMat4("projection", projection_matrix);
		asteroidShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		asteroidShader.setVec3("light.ambient", ambientColor);
		asteroidShader.setVec3("light.diffuse", diffuseColor);
		asteroidShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		//		simpleShader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
		//		simpleShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
		//		simpleShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		asteroidShader.setVec3("material.ambient", 0.3f, 0.3f, 0.3f);
		asteroidShader.setVec3("material.diffuse", 0.84f, 0.84f, 0.84f);
		asteroidShader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);
		asteroidShader.setFloat("material.shininess", 22.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		Utilities::renderAsteroids(asteroidShader, asteroid, ast_transform);

		// render land
		landShader.use();
		landShader.setVec3("light.position", lightPos);
		landShader.setVec3("lightPos", lightPos);
		landShader.setVec3("viewPos", camPos);
		landShader.setMat4("view", view_matrix);
		landShader.setMat4("projection", projection_matrix);

		landShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// light properties

		landShader.setVec3("light.ambient", ambientColor);
		landShader.setVec3("light.diffuse", diffuseColor);
		landShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties

		landShader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
		landShader.setVec3("material.diffuse", 0.84f, 0.84f, 0.84f);
		landShader.setVec3("material.specular", 0.2f, 0.2f, 0.2f);
		landShader.setFloat("material.shininess", shininess);
		landShader.setFloat("time", (float)glfwGetTime());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		Utilities::renderLand(landShader, land, land_transform);


		// sky render
		skyShader.use();
		sky.Draw(skyShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
			bloom = !bloom;

		// 2. blur bright fragments with two-pass Gaussian Blur
		// --------------------------------------------------
		bool horizontal = true, first_iteration = true;
		blurShader.use();
		for (unsigned int i = 0; i < 6; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			blurShader.setInt("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
			renderQuad();
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
		// --------------------------------------------------------------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bloomShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		bloomShader.setFloat("exposure", 1.0f);
		bloomShader.setBool("bloom", bloom);
		renderQuad();

		// render Depth map to quad for visual debugging
		// ---------------------------------------------
		//		debugDepthQuad.use();
		//		debugDepthQuad.setFloat("near_plane", near_plane);
		//		debugDepthQuad.setFloat("far_plane", far_plane);
		//		glActiveTexture(GL_TEXTURE0);
		//		glBindTexture(GL_TEXTURE_2D, depthMap);
		//		renderQuad();

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


unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] =
		{
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
