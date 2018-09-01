
#include "laser.h"

const int laser::max = 10;
int laser::count = max;

laser::laser() {}

laser::~laser() {}

laser::laser(const glm::mat4 &view_matrix, const glm::mat4 &projection_matrix, Shader & shader): shader(shader)
{
	shader.use();
	shader.setMat4("view", view_matrix);
	shader.setMat4("projection", projection_matrix);
}

void laser::updateTransform()
{
	if(!shooted)
		return;

	transform = glm::mat4();
	position.z -= 2.0f;
	if(position.z < -40)
	{
		shooted = false;
		count ++;
	}

	transform = glm::translate(transform, position);
	transform = glm::scale(transform, scale);
}

void laser::shoot(const int &deltaX, const int &deltaY)
{
	if(count == 0) return;
	count --;
	shooted = true;
	position = glm::vec3(deltaX, deltaY, -2.f);
}

void laser::Draw(Model &model)
{
	if(!shooted) return;
	shader.use();
	shader.setMat4("model", transform);
	shader.setMat3("normalMat", glm::inverseTranspose(glm::mat3(transform)));

	model.Draw(shader);
}

