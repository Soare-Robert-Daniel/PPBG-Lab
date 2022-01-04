#include "player.h"

void tema2::Player::move(float distance)
{
	pos += camera->forward * distance;
}

void tema2::Player::rotate(float a)
{
	camera->RotateFirstPerson_OY(a);
}

void tema2::Player::shoot()
{
	// TODO: Add shooting
}

glm::mat4 tema2::Player::getModelMatrix()
{
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, pos);
	// TODO: Add rotation
	return modelMatrix;
}
