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
	holoShader.setFloat("m_GlowDistance", 0.2f);
	holoShader.setVec4("m_RimColor", glm::vec4(0.5f,0.8f,1.f,1.f));
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

void Utilities::renderScore(Shader &holoShader, Model &tens, Model &units)
{
	holoShader.use();

	glm::mat4 t = glm::mat4();
	t = glm::translate(t, glm::vec3(-9.0f,-1.5f,0.f));
	t = glm::scale(t, glm::vec3(1.5f, 1.5f, 1.5f));
	glm::mat4 normalMat = glm::inverseTranspose(glm::mat3(t));
	holoShader.setMat4("model", t);
	holoShader.setMat3("normalMat", normalMat);
	tens.Draw(holoShader);

	t = glm::mat4();
	t = glm::translate(t, glm::vec3(-8.0f,-1.5f,0.f));
	t = glm::scale(t, glm::vec3(1.5f, 1.5f, 1.5f));
	normalMat = glm::inverseTranspose(glm::mat3(t));
	holoShader.setMat4("model", t);
	holoShader.setMat3("normalMat", normalMat);
	units.Draw(holoShader);
}

vector<unique_ptr<Model>> Utilities::loadNumbers()
{
	vector<unique_ptr<Model>> numbers;
	numbers.push_back(unique_ptr<Model>(new Model("src/numbers/zero.obj")));
	numbers.push_back(unique_ptr<Model>(new Model("src/numbers/one.obj")));
	numbers.push_back(unique_ptr<Model>(new Model("src/numbers/two.obj")));
	numbers.push_back(unique_ptr<Model>(new Model("src/numbers/three.obj")));
	numbers.push_back(unique_ptr<Model>(new Model("src/numbers/four.obj")));
	numbers.push_back(unique_ptr<Model>(new Model("src/numbers/five.obj")));
	numbers.push_back(unique_ptr<Model>(new Model("src/numbers/six.obj")));
	numbers.push_back(unique_ptr<Model>(new Model("src/numbers/seven.obj")));
	numbers.push_back(unique_ptr<Model>(new Model("src/numbers/eight.obj")));
	numbers.push_back(unique_ptr<Model>(new Model("src/numbers/nine.obj")));

	return numbers;
}

vector<unique_ptr<Laser>> Utilities::loadLasers(const glm::mat4 &view_matrix, const glm::mat4 &projection_matrix, Shader & shader)
{
	vector<unique_ptr<Laser>> lasers;
	for(int i=0; i < Laser::max; i++)
		lasers.push_back(unique_ptr<Laser>(new Laser(view_matrix, projection_matrix, shader)));
	return lasers;
}

int Utilities::currentLaser = 0;

void Utilities::shoot(vector<unique_ptr<Laser>> &lasers, const int &deltaX, const int &deltaY)
{
	if(Laser::count == 0) return;
	lasers[currentLaser]->shoot(deltaX, deltaY);
	currentLaser++;
	if(currentLaser >= 10)
		currentLaser = 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void Utilities::renderQuad()
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

void Utilities::debugDepthMap(Shader &debugDepthQuad, const int& near_plane, const int& far_plane, const int& depthMap)
{
	debugDepthQuad.use();
	debugDepthQuad.setFloat("near_plane", near_plane);
	debugDepthQuad.setFloat("far_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	renderQuad();
}

vector<int> Utilities::loadCounters(int ast)
{
	vector<int> counters;
	for(int i=1; i<=ast; i++)
		counters.push_back(0);
	return counters;
}

vector<unsigned int> Utilities::loadExplosionTextures()
{
	vector<unsigned int> explosionTextures;
	for(int i=1; i<=64; i++)
		explosionTextures.push_back(Utilities::loadTexture("src/asteroid/particles/Particle"+to_string(i)+".png"));
	return explosionTextures;
}

unsigned int Utilities::loadTexture(string const &path)
{
//	string directory = path.substr(0, path.find_last_of('/'));
//	string filename = string(path);
//	filename = directory + '/' + filename;
	string filename = path;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;

}
