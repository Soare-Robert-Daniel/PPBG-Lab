#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "lab_m1/tema2/lab_camera.h"

namespace tema2 {
	class Player
	{
	public:
		glm::vec3 pos;
		float speed;
		tema2::Camera *camera;
		std::string meshID;

		Player(glm::vec3 position, float speed, std::string meshID) {
			pos = position;
			this->speed = speed;
			this->meshID = meshID;
		}

		void move(float distance);
		void rotate(float angle);
		void shoot();

		glm::mat4 getModelMatrix();
	};
}


