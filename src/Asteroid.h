
#ifndef SRC_ASTEROID_H_
#define SRC_ASTEROID_H_

#include "Shader.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <cstdlib>
#include <GLFW/glfw3.h>

class Asteroid
{
public:
	Asteroid();
	Asteroid(const glm::vec3 &lightPos, const glm::vec3 &camPos, const glm::mat4 &view_matrix, const glm::mat4 &projection_matrix, const glm::mat4 &lightSpaceMatrix, const glm::vec3 &ambientColor, const glm::vec3 &diffuseColor, unsigned int depthMap);
	virtual ~Asteroid();

	Shader shader;
	glm::vec3 position;
	glm::vec3 scale;
	glm::mat4 transform;
	float rotX;
	float rotY;
	float rotZ;
	float alpha;
	unsigned int depthMap;

	void Draw(Model&);
	void updateTransform();

};

#endif /* SRC_ASTEROID_H_ */
