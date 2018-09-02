
#include "Laser.h"

const int Laser::max = 10;
int Laser::count = max;

Laser::Laser() {}

Laser::~Laser() {}

Laser::Laser(const glm::mat4 &view_matrix, const glm::mat4 &projection_matrix, Shader & shader): shader(shader)
{
	shader.use();
	shader.setMat4("view", view_matrix);
	shader.setMat4("projection", projection_matrix);
}

void Laser::updateTransform()
{
	if(!shooted)
		return;

	transform = glm::mat4();
	position.z -= 2.0f;
	if(position.z < -60)
	{
		shooted = false;
		count ++;
	}

	transform = glm::translate(transform, position);
	transform = glm::scale(transform, scale);
}

void Laser::shoot(const int &deltaX, const int &deltaY)
{
	if(count == 0) return;
	count --;
	shooted = true;
	position = glm::vec3(deltaX, deltaY, -2.f);
}

void Laser::Draw(Model &model)
{
	if(!shooted) return;
	shader.use();
	shader.setMat4("model", transform);
	shader.setMat3("normalMat", glm::inverseTranspose(glm::mat3(transform)));

	model.Draw(shader);
}

void Laser::reset()
{
	shooted = false;
	count ++;
}

