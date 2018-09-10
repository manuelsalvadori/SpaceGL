#ifndef SRC_TARGET_H_
#define SRC_TARGET_H_

#include "Shader.h"
#include "Utilities.h"

class Target {
public:
	Target(const glm::mat4 &view_matrix, const glm::mat4 &projection_matrix);
	virtual ~Target();

	void renderTarget(Model &model, const float &deltaX, float &deltaY);

private:
	Shader shader;
	unsigned int text1;
};

#endif /* SRC_TARGET_H_ */
