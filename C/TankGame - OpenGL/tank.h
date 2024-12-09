#include "utils/glm_utils.h"
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"
using namespace std;
class CannonBall;
class Tank
	{
	public:
		Tank(int id, float initialX, vector<glm::vec2>& heightMap);
		int FindPositionIndex(vector<glm::vec2>& heightMap, int targetX);
		void InitializePosition(float initialX, vector<glm::vec2>& heightMap);
		void MoveTank(vector<glm::vec2>& heightMap, float deltaTime, int speed);
		void GenerateModelMatrix();
		void findAngle(vector<glm::vec2>& heightMap, int index);
		void RotateGun(float deltaTime, int angVelocity);
		void LaunchBall(float speed, vector<CannonBall*>& cannonBalls, bool isSpecial);
		int heightMapIndex;
		glm::vec2 position;
		glm::vec2 ballPosition;
		glm::mat3 modelMatrixTank;
		glm::mat3 modelMatrixGun;
		glm::mat3 modelMatrixBall;
		glm::mat3 modelMatrixHpBar;
		glm::mat3 modelMatrixHpFrame;
		float hitCircleRadius = 40;
		float tankAngle;
		float gunAngle = glm::pi<float>() / 2;
		float hp = 100;
		int id;
		std::vector<CannonBall*> cannonBalls;
		Mesh* tankMesh;
		Mesh* gun;
		Mesh* hpFrame;
		Mesh* hpBar;
		bool hasSpecial = false;
		int hitCount = 0;
		int specialCount = 3;
	};