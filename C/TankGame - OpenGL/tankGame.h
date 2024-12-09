#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema1/tank.h"
#include "lab_m1/tema1/cannonBall.h"
#define gConstant 1000
#define ballReleaseSpeed 800
#define movementSpeed 200
#define angularSpeed 1
#define terrainInflSpeed 100
namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        int FindPositionIndex(int targetX);

        void ExplodeTerrain(int index, glm::vec2 explosionCenter);

        void DrawPreview(Tank* tank, float speed);

        void VerifyCollision(vector<glm::vec2>& heightMap);

        void RenderTerrain();
        void Init() override;

    private:
        void FrameStart() override;
        void RemodelTerrain(float deltaTime);
        void UpdateCannonBalls(float deltaTime);
        void ShakeCamera(float deltaTimeSeconds);
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
    protected:
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;
        glm::mat3 modelMatrix1 = glm::mat3(1);
        glm::mat3 modelMatrix2 = glm::mat3(1);
        glm::mat3 modelMatrix3 = glm::mat3(1);
        vector<CannonBall*> cannonBalls;
        vector<Tank*> tanks;
        float hp = 100;
        vector<glm::vec2> heightMap;
        float remodelFactor = 0;
        int maxIterations = 200;
        int iterations = 0;
        float shakeTime = 0;
        float maxShakeTime = 1;
    };
}   // namespace m1
