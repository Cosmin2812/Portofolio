#include "utils/glm_utils.h"
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"
using namespace std;

class Tank;
class CannonBall {
	public:
		CannonBall(float speed, glm::vec2 tankPosition, Tank* tankParent, bool isSpecial);
		glm::vec2 tankPosition;
		glm::vec2 velocity;
		glm::mat3 modelMatrix;
		float initialTankAngle;
		bool hitTerrain = false;
		glm::vec2 ballPosition;
		bool isLaunched = false;
		Tank* tankParent;
		bool isSpecial = true;
};