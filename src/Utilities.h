#ifndef SRC_UTILITIES_H_
#define SRC_UTILITIES_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Utilities
{
public:
	static void GLWF_init();
	static void GLEW_init();
	static GLFWwindow* createWindow();
};

#endif /* SRC_UTILITIES_H_ */
