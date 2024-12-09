#include "tank.h"
#include "cannonBall.h"
#include "core/engine.h"
#include "utils/gl_utils.h"
#include <lab_m1/lab3/transform2D.h>
#include <iostream>


using namespace std;

int Tank::FindPositionIndex(vector<glm::vec2>& heightMap, int targetX) {
	auto targetIt = std::lower_bound(heightMap.begin(), heightMap.end(), targetX,
		[](const glm::vec2& point, float x) { return point.x < x; });

	return std::distance(heightMap.begin(), targetIt - 1);
}
void Tank::InitializePosition(float initialX, vector<glm::vec2>& heightMap) {
	int index = FindPositionIndex(heightMap, initialX);
	float initialY;

	if (initialX == heightMap[index].x) {
		initialY = heightMap[index].y;
	} else {
		initialY = ((heightMap[index + 1].y - heightMap[index].y) / (heightMap[index + 1].x - heightMap[index].x)) * (initialX - heightMap[index].x) + heightMap[index].y;
	}

	position.x = initialX;
	position.y = initialY;
	findAngle(heightMap, index);
	gunAngle = glm::pi<float>() / 2;
	heightMapIndex = index;
}

void Tank::MoveTank(vector<glm::vec2>&heightMap, float deltaTime, int speed) {
	findAngle(heightMap, heightMapIndex);
	float deltaX = deltaTime * speed * cos(this->tankAngle);
	float newX = deltaX + this->position.x;

	float newY;
	float newIndex;
	if (newX > heightMap[heightMap.size() - 2].x) {
		newX = heightMap[heightMap.size() - 2].x;
		newY = heightMap[heightMap.size() - 2].y;
		newIndex = heightMap.size() - 2;
	}
	else if (newX < heightMap[2].x) {
;		newX = heightMap[2].x;
		newY = heightMap[2].y;
		newIndex = 2;
	}
	else
	{
		newIndex = FindPositionIndex(heightMap, newX);
		newY = ((heightMap[newIndex + 1].y - heightMap[newIndex].y) / (heightMap[newIndex + 1].x - heightMap[newIndex].x)) * (newX - heightMap[newIndex].x) + heightMap[newIndex].y;
	}

	position.x = newX;
	position.y = newY;
	findAngle(heightMap, newIndex);
	heightMapIndex = newIndex;
}

void Tank::RotateGun(float deltaTime, int angVelocity) {
	float newAngle = this->gunAngle + deltaTime * angVelocity;
	this->gunAngle = newAngle;
}

void Tank::LaunchBall(float speed, vector<CannonBall*>& cannonBalls, bool isSpecial) {
	cannonBalls.push_back(new CannonBall(speed, this->position, this, isSpecial));
}

void Tank::findAngle(vector<glm::vec2>& heightMap, int index) {
	glm::vec2 v = heightMap[index + 1] - heightMap[index];
	this->tankAngle = atan2(v.y, v.x);
}

Tank::Tank(int id, float initialX, vector<glm::vec2>& heightMap) {
	InitializePosition(initialX, heightMap);
	this->id = id;
}

void Tank::GenerateModelMatrix() {
	modelMatrixTank = glm::mat3(1);
	modelMatrixTank *= transform2D::Translate(position.x, position.y);
	modelMatrixTank *= transform2D::Scale(8, 8);
	modelMatrixTank *= transform2D::Rotate(tankAngle);

	modelMatrixGun = modelMatrixTank;
	glm::vec3 translate2Origin = glm::vec3(-2.5f, -0.5f, 0.1f) + glm::vec3(2.5f, 4.375f, 0);
	modelMatrixGun *= transform2D::Translate(translate2Origin.x, translate2Origin.y);
	modelMatrixGun *= transform2D::Rotate(gunAngle);
	modelMatrixGun *= transform2D::Rotate(-tankAngle);
	modelMatrixGun *= transform2D::Translate(-translate2Origin.x, -translate2Origin.y);

	modelMatrixHpFrame = glm::mat3(1);
	modelMatrixHpFrame *= transform2D::Translate(position.x, position.y);
	modelMatrixHpFrame *= transform2D::Scale(8, 8);

	glm::vec3 initPos = glm::vec3(-3, 7, 1);
	initPos = modelMatrixHpFrame * initPos;
	modelMatrixHpBar = glm::mat3(1);
	modelMatrixHpBar *= transform2D::Translate(initPos.x, initPos.y);
	modelMatrixHpBar *= transform2D::Scale(hp / 100, 1);
	modelMatrixHpBar *= transform2D::Translate(-initPos.x, -initPos.y);
	modelMatrixHpBar *= modelMatrixHpFrame;
}

