#include "cannonBall.h"
#include "core/engine.h"
#include "utils/gl_utils.h"
#include <lab_m1/lab3/transform2D.h>
#include "tank.h"
#include <iostream>
using namespace std;


CannonBall::CannonBall(float speed, glm::vec2 tankPosition, Tank* tankParent, bool isSpecial)
{
	this->tankPosition = tankPosition;
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(tankPosition.x, tankPosition.y);
	modelMatrix *= transform2D::Scale(8, 8);
	modelMatrix *= transform2D::Rotate(tankParent->tankAngle);
	glm::vec3 firstPos = glm::vec3(0.0f, 3.875f, 1.0f);
	firstPos = modelMatrix * firstPos;
	ballPosition = glm::vec2(firstPos.x, firstPos.y);
	float angle = glm::pi<float>() / 2 + tankParent->gunAngle;
	velocity = glm::vec2 (cos(angle), sin(angle)) * speed;
	initialTankAngle = tankParent->tankAngle;
	this->tankParent = tankParent;
	this->isSpecial = isSpecial;
}



