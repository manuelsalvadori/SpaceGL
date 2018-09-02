#ifndef SRC_UTILITIES_H_
#define SRC_UTILITIES_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Laser.h"
#include "Shader.h"
#include "Model.h"

class Utilities
{
public:
	static void GLWF_init();
	static void GLEW_init();
	static GLFWwindow* createWindow();
	static void movementHandler(GLFWwindow *window, float &deltaTime, float &rotSpeed, float &deltaX, float &deltaY, float &rotX, float &rotZ);
	static void renderScene(Shader shader, Model &falcon, glm::mat4 &, Model &asteroid, glm::mat4 &ast_transform);
	static void renderFalcon(Shader shader, Model &falcon, glm::mat4 &falcon_transform);
	static void renderAsteroids(Shader shader, Model &asteroid, glm::mat4 &ast_transform);
	static void renderLand(Shader shader, Model &land, glm::mat4 &land_transform);
	static void renderHologram(Shader &holoShader, Model &model);
	static void asteroidsTransform(glm::mat4 &ast_transform, glm::vec3 traslate, float rotx, float roty, float rotz, glm::vec3 scale = glm::vec3(1.f));
	static void moveLight(GLFWwindow *window, glm::vec3 &lightPos);
	static void updateFalcon(glm::mat4 &falcon_transform, const float &deltaX, float &deltaY, float &rotX, float &rotY, float &rotZ);
	static void renderScore(Shader &holoShader, Model &tens, Model &units);
	static void renderQuad();
	static void debugDepthMap(Shader &debugDepthQuad, const int& near_plane, const int& far_plane, const int& depthMap);
	static vector<unique_ptr<Model>> loadNumbers();
	static vector<unique_ptr<Laser>> loadLasers(const glm::mat4 &view_matrix, const glm::mat4 &projection_matrix, Shader & shader);
	static void shoot(vector<unique_ptr<Laser>> &lasers, const int &deltaX, const int &deltaY);
	static vector<int> loadCounters(int ast);
	static vector<unsigned int> loadExplosionTextures();
	static unsigned int loadTexture(string const &path);
private:
	static int currentLaser;
};

#endif /* SRC_UTILITIES_H_ */
