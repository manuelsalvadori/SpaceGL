
#ifndef SRC_LASER_H_
#define SRC_LASER_H_

#include "Shader.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <GLFW/glfw3.h>

class Laser {
public:
	Laser();
	Laser(const glm::mat4 &view_matrix, const glm::mat4 &projection_matrix, Shader & shader);
	virtual ~Laser();

	Shader shader;
	glm::vec3 position = glm::vec3(0.0);
	const glm::vec3 scale = glm::vec3(0.2, 0.2, 0.3);
	glm::mat4 transform;
	bool shooted = false;

	void Draw(Model&);
	void updateTransform();
	void shoot(const int &deltaX, const int &deltaY);
	void reset();

	const static int max;
	static int count;

};

#endif /* SRC_LASER_H_ */
