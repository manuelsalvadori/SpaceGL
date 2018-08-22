#ifndef SRC_UTILITIES_H_
#define SRC_UTILITIES_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Model.h"

class Utilities
{
public:
	static void GLWF_init();
	static void GLEW_init();
	static GLFWwindow* createWindow();
	static void movementHandler(GLFWwindow *window, float &deltaTime, float &rotSpeed, float &deltaX, float &deltaY, float &rotX, float &rotZ);
	static void renderScene(Shader shader, Model &falcon, glm::mat4 &, Model &land, glm::mat4 &, Model &asteroid, glm::mat4 &ast_transform);
};

#endif /* SRC_UTILITIES_H_ */
