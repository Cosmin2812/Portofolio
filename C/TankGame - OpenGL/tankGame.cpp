#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <iostream>
#include <glm/gtx/vector_angle.hpp>
#include "lab_m1/tema1/transform2D.h"
#include "lab_m1/tema1/object2D.h"
#include <random>
using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

int Tema1::FindPositionIndex(int targetX) {
    auto targetIt = std::lower_bound(heightMap.begin(), heightMap.end(), targetX,
        [](const glm::vec2& point, float x) { return point.x < x; });

    return std::distance(heightMap.begin(), targetIt - 1);
}

float MapFunction(float x) {
    return 250 + 50 * sin(0.04 * x) * sin(0.01 * x) + 80 * sin(0.02 * x + 5) + 60 * cos(0.05 * x);
}

vector<glm::vec2> GenerateHeightMap(int numIntervals, float xMin, float xMax, float (*fx)(float)) {
    vector<glm::vec2> heightMap;
    float step = (xMax - xMin) / numIntervals;
    for (int i = 0; i <= numIntervals; i++) {
        float x = xMin + step * i;
        float y = fx(x);
        heightMap.push_back(glm::vec2(x, y));
    }
    return heightMap;
}

void Tema1::ExplodeTerrain(int index, glm::vec2 explosionCenter) {
    float radius = 20;
    int left, right;

    if (heightMap[index].x - radius <= heightMap[0].x) {
        left = heightMap[0].x;
    } else {
        left = FindPositionIndex(heightMap[index].x - radius);
    }

    if (heightMap[index].x + radius >= heightMap[heightMap.size() - 1].x) {
        right = heightMap[heightMap.size() - 1].x;
    } else {
        right = FindPositionIndex(heightMap[index].x + radius) + 1;
    }

    for (int i = left; i <= right; i++) {
        float a = glm::asin(abs(explosionCenter.x - heightMap[i].x) / radius);
        float circleIntersectionY = explosionCenter.y - cos(a) * radius;
        if (circleIntersectionY < heightMap[i].y) {
            heightMap[i].y = circleIntersectionY;
        }
    }

    remodelFactor = 0;
    iterations = 0;
}

float fPreview(float x, float initialY, float initialX, glm::vec2 initialVelocity) {
    x = x - initialX;
    if (initialVelocity.x == 0) {
        return initialY + -(1 / 2) * gConstant * (x * x / initialVelocity.x * initialVelocity.x);
    }
    return initialY + (initialVelocity.y / initialVelocity.x) * x - (gConstant * x * x) / (2 * initialVelocity.x * initialVelocity.x);
}

float WrapAngleTo2Pi(float angle) {
    float twoPi = glm::two_pi<float>();
    angle = fmod(angle, twoPi);

    if (angle < 0) {
        angle += twoPi;
    }
    return angle;
}
float normalizeAngle(float angle) {
    while (angle < -glm::pi<float>()) {
        angle += 2 * glm::pi<float>();
    }

    while (angle >= glm::pi<float>()) {
        angle -= 2 * glm::pi<float>();
    }

    return angle;
}
void Tema1::VerifyCollision(vector<glm::vec2>& heightMap) {
    float epsilon = 2;
    for (CannonBall* ball : cannonBalls) {
        // intersection with the tank's hit circle
        for (Tank* tank : tanks) {
            float normTankAng = normalizeAngle(tank->tankAngle);
            float normGunAng = normalizeAngle(tank->gunAngle);
            float angDiff = normalizeAngle(normTankAng- normGunAng);
            if ((glm::length(ball->ballPosition - tank->position) < tank->hitCircleRadius && ball->isLaunched) || 
                ((angDiff < - 4 * glm::pi<float>() / 5 || angDiff > 4 * glm::pi<float>() / 5) && tank == ball->tankParent)) {
                ball->hitTerrain = true;
                if (ball->isSpecial) {
                    tank->hp -= 20;
                }else {
                    tank->hp -= 10;
                    if (ball->tankParent->hitCount < ball->tankParent->specialCount) {
                        ball->tankParent->hitCount++;
                    }
                }
                if (tank->hp <= 70 && tank->hp >= 40) {
                    tank->hpBar = meshes["hpBarMed"];
                }
                else if (tank->hp < 40) {
                    tank->hpBar = meshes["hpBarLow"];
                }
                continue;
            }
            if (glm::length(ball->ballPosition - ball->tankParent->position) > ball->tankParent->hitCircleRadius) {
                ball->isLaunched = true;
            }
        }
        if (ball->hitTerrain) {
            continue;
        }


        // intersection with the terrain
        // check map boundries
        if ((ball->ballPosition.x >= heightMap[heightMap.size() - 2].x || ball->ballPosition.x <= heightMap[1].x)) {
            if (ball->ballPosition.x > heightMap[heightMap.size() - 1].x + 20 || ball->ballPosition.x < heightMap[0].x - 20) {
                ball->hitTerrain = true;
            }
            continue;
        }
        int index = FindPositionIndex(ball->ballPosition.x);
        float y;
        if (ball->ballPosition.x == heightMap[index].x) {
            y = heightMap[index].y;
        }
        else {
            y = ((heightMap[index + 1].y - heightMap[index].y) / (heightMap[index + 1].x - heightMap[index].x)) * (ball->ballPosition.x - heightMap[index].x) + heightMap[index].y;
        }
        if (ball->ballPosition.y <= y + epsilon && !ball->isSpecial) {
            ball->hitTerrain = true;
            ExplodeTerrain(index, ball->ballPosition);
        }
    }
    cannonBalls.erase(remove_if(this->cannonBalls.begin(), this->cannonBalls.end(), [](CannonBall* x) {if (x->hitTerrain) { delete x; return true; } return false; }), this->cannonBalls.end());
}

void Tema1::RenderTerrain() {
    for (int i = 0; i < heightMap.size() - 1; i++) {
        float step = heightMap[i + 1].x - heightMap[i].x;
        float height = max(heightMap[i].y, heightMap[i + 1].y);
        float shearY = (heightMap[i + 1].y - heightMap[i].y) / (heightMap[i + 1].x - heightMap[i].x);
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(heightMap[i].x, heightMap[i].y);
        modelMatrix *= transform2D::Shear(0, shearY);
        modelMatrix *= transform2D::Scale(step, height);
        RenderMesh2D(meshes["squareUnit"], shaders["VertexColor"], modelMatrix);
    }
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);
    std::random_device rd;
    std::mt19937 gen(rd());
    int terrainIntervals = resolution.x;
    heightMap = GenerateHeightMap(terrainIntervals, 0, resolution.x, MapFunction);
    glm::vec3 corner = glm::vec3(0, -1, 0);
    glm::vec3 tankCorner = glm::vec3(-2.5f, -0.5f, -0.1f);
    glm::vec3 gunCorner = tankCorner + glm::vec3(2.25f, 4.375f, -0.1f);
    float squareSide = 1;
    uniform_int_distribution<> distrib(200, resolution.x - 200);
    int initialPosition1 = distrib(gen);
    int initialPosition2 = distrib(gen);
    tanks.push_back(new Tank(0, initialPosition1, heightMap));
    tanks.push_back(new Tank(1, initialPosition2, heightMap));
    vector<CannonBall*> cannonBalls;
    
    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;

    // Initialize sx and sy (the scale factors)
    scaleX = 1;
    scaleY = 1;

    // Initialize angularStep
    angularStep = 0;

    Mesh* hpBarHigh = object2D::CreateRectangle("hpBarHigh", glm::vec3(-3, 7, 1.0f), 6, 1, glm::vec3(0, 0.6f, 0), true);
    Mesh* hpBarMed = object2D::CreateRectangle("hpBarMed", glm::vec3(-3, 7, 1.0f), 6, 1, glm::vec3(0.8f, 0.8f, 0), true);
    Mesh* hpBarLow = object2D::CreateRectangle("hpBarLow", glm::vec3(-3, 7, 1.0f), 6, 1, glm::vec3(0.6f, 0, 0), true);
    
    Mesh* hpFrame = object2D::CreateRectangle("hpFrame", glm::vec3(-3, 7, 1), 6, 1, glm::vec3(1, 1, 1), false);
    Mesh* tankMesh = object2D::CreateTank("tank", tankCorner);
    Mesh* gun = object2D::CreateRectangle("gun", gunCorner, 0.5f, 4, glm::vec3(0.22f, 0.22f, 0.23f), true);
    Mesh* specialPreview = object2D::CreateRectangle("specialPreview", gunCorner, 0.5f, 30, glm::vec3(1, 0.5f, 0), true);
    Mesh* cannonBall = object2D::CreateSphere("ball", glm::vec3(0, 0, 0), 5.0f, glm::vec3(0, 0, 0), true);
    Mesh* unit = object2D::CreateSquare("squareUnit", corner, squareSide, glm::vec3(0.80, 0.80, 0.18), true);
    Mesh* previewUnit = object2D::CreateRectangle("previewUnit", glm::vec3(0, -1, -0.3f), 1, 2, glm::vec3(1, 1, 1), true);
    Mesh* specialInd = object2D::CreateSphere("specialInd", glm::vec3(0, 11, 1.2f), 0.4f, glm::vec3(0.6f, 0.6f, 0), true);
    
    AddMeshToList(unit);
    AddMeshToList(tankMesh);
    AddMeshToList(gun);
    AddMeshToList(cannonBall);
    AddMeshToList(hpBarHigh);
    AddMeshToList(hpBarMed);
    AddMeshToList(hpBarLow);
    AddMeshToList(hpFrame);
    AddMeshToList(previewUnit);
    AddMeshToList(specialInd);
    AddMeshToList(specialPreview);
    for (Tank* t : tanks) {
        t->hpBar = meshes["hpBarHigh"];
    }
}

void Tema1::DrawPreview(Tank* tank, float speed) {
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(tank->position.x, tank->position.y);
    modelMatrix *= transform2D::Scale(8, 8);
    modelMatrix *= transform2D::Rotate(tank->tankAngle);
    glm::vec3 firstPos = glm::vec3(0.0f, 3.875f, 1.0f);
    firstPos = modelMatrix * firstPos;
    float angle = glm::pi<float>() / 2 + tank->gunAngle /*+ tank->tankAngle*/;
    glm::vec2 initialVelocity = glm::vec2(cos(angle), sin(angle)) * speed;
    int index = -1;
    if (firstPos.x > heightMap[0].x) {
        index = FindPositionIndex(firstPos.x);
    }
    float yIndex = firstPos.y;
    glm::vec2 velocity = initialVelocity;
    float x = firstPos.x;
    float dt = 0.004;
    float y = firstPos.y;

    while (index >= 1 && index <= heightMap.size() - 2 && y >= 0) {
        // from the closest index in the heightMap continue to draw the line until u meet an i with heightMap[i].y > our y
        if (initialVelocity.x > 0) {
            while (x > heightMap[index].x && index <= heightMap.size() - 2) {
                index++;
            }
            y = fPreview(x, firstPos.y, firstPos.x, initialVelocity);
            float xNext = x + velocity.x * dt;
            float yNext = fPreview(xNext, firstPos.y, firstPos.x, initialVelocity);
            float angle = glm::atan2(yNext - y, xNext - x);
            float length = glm::length(glm::vec2(yNext - y, xNext - x));
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(x, y);
            modelMatrix *= transform2D::Rotate(angle);
            modelMatrix *= transform2D::Scale(length, 1);
            RenderMesh2D(meshes["previewUnit"], shaders["VertexColor"], modelMatrix);
            x = xNext;
            velocity -= glm::vec2(0, gConstant) * dt;
        }
        else {
            while (x < heightMap[index].x && index >= 1) {
                index--;
            }
            y = fPreview(x, firstPos.y, firstPos.x, initialVelocity);
            float xNext = x + velocity.x * dt;
            float yNext = fPreview(xNext, firstPos.y, firstPos.x, initialVelocity);
            float angle = glm::atan2(yNext - y, xNext - x);
            float length = glm::length(glm::vec2(yNext - y, xNext - x));
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(x, y);
            modelMatrix *= transform2D::Rotate(angle);
            modelMatrix *= transform2D::Scale(length, 1);
            //modelMatrix *= transform2D::Rotate(glm::pi<float>() / 2- angle);
            RenderMesh2D(meshes["previewUnit"], shaders["VertexColor"], modelMatrix);
            x = xNext;
            velocity -= glm::vec2(0, gConstant) * dt;
        }
    }
}

void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.58f, 0.74f, 0.89f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

std::vector<float> CalculateLaplacian(const std::vector<glm::vec2>& heightMap) {
    std::vector<float> laplacianValues(heightMap.size(), 0.0f);

    for (size_t i = 1; i < heightMap.size() - 1; ++i) {
        float laplacian = heightMap[i].y - 0.5f * (heightMap[i - 1].y + heightMap[i + 1].y);
        laplacianValues[i] = laplacian;
    }

    return laplacianValues;
}

void ApplySmoothing(vector<glm::vec2>& heightMap, const vector<float>& laplacianValues, float threshold, float smoothingFactor) {
    for (int i = 1; i < heightMap.size() - 1; i++) {
        if (abs(laplacianValues[i]) > threshold) {
            // Applied smoothing only to points with high Laplacian (sharp edges)
            heightMap[i].y = heightMap[i].y * (1.0f - smoothingFactor)
                + 0.5f * (heightMap[i - 1].y + heightMap[i + 1].y) * smoothingFactor;
        }
    }
}

void Tema1::RemodelTerrain(float deltaTime) {
    // Applied a technique called Laplacian smoothing
    // Tried to use the remodeling alg described in the homework page, but it didn't work

    vector<float> laplacian = CalculateLaplacian(heightMap);
    ApplySmoothing(heightMap, laplacian, 0.075, deltaTime * 10);

    /*if (iterations < maxIterations) {
        for (int i = 0; i < heightMap.size() - 1; i++) {
            if (abs(heightMap[i].y - heightMap[i + 1].y) > 0.2) {
                if (heightMap[i].y > heightMap[i + 1].y) {
                    float eps = heightMap[i].y - heightMap[i + 1].y;
                    heightMap[i].y -= eps * deltaTime;
                    heightMap[i + 1].y += eps * deltaTime;
                } else {
                    float eps = heightMap[i + 1].y - heightMap[i].y;
                    heightMap[i + 1].y -= eps * deltaTime;
                    heightMap[i].y += eps * deltaTime;
                }
            }
        }
        iterations++;
    }*/
}

void Tema1::UpdateCannonBalls(float deltaTime) {
    for (CannonBall* ball : cannonBalls) {
        ball->ballPosition += ball->velocity * deltaTime;
        ball->velocity -= glm::vec2(0, 1000) * deltaTime;
        ball->modelMatrix = glm::mat3(1);
        ball->modelMatrix *= transform2D::Translate(ball->ballPosition.x, ball->ballPosition.y);
    }
}
void Tema1::ShakeCamera(float deltaTimeSeconds) {
    shakeTime += deltaTimeSeconds;
    auto camera = GetSceneCamera();
    glm::vec2 resolution = window->GetResolution();
    float left = 22 * exp(-15 * shakeTime) * sin(30000 * shakeTime);
    float right = 22 * exp(-15 * shakeTime) * cos(30000 * shakeTime);
    camera->SetOrthographic(left, (float)resolution.x + left, right, (float)resolution.y + right, 0.01f, 400);
    camera->Update();
}

void Tema1::Update(float deltaTimeSeconds)
{
    RemodelTerrain(deltaTimeSeconds);
    UpdateCannonBalls(deltaTimeSeconds);
    VerifyCollision(heightMap);
    bool deletedTank = false;
    tanks.erase(remove_if(tanks.begin(), tanks.end(),[&deletedTank](Tank* x) {if (x->hp <= 0) { delete x; deletedTank = true;  return true; } return false; }), tanks.end());
    if (deletedTank) {
        shakeTime = 0;
        deletedTank = false;
    }

    if (shakeTime < maxShakeTime) {
        ShakeCamera(deltaTimeSeconds);
    }

    for (Tank* tank : tanks) {
        DrawPreview(tank, 800);
        tank->MoveTank(heightMap, deltaTimeSeconds, 0);
        tank->GenerateModelMatrix();
        RenderMesh2D(meshes["gun"], shaders["VertexColor"], tank->modelMatrixGun);
        RenderMesh2D(meshes["tank"], shaders["VertexColor"], tank->modelMatrixTank);
        RenderMesh2D(meshes["hpFrame"], shaders["VertexColor"], tank->modelMatrixHpFrame);
        RenderMesh2D(tank->hpBar, shaders["VertexColor"], tank->modelMatrixHpBar);
        if (tank->hitCount == tank->specialCount) {
            RenderMesh2D(meshes["specialInd"], shaders["VertexColor"], tank->modelMatrixHpFrame);
            RenderMesh2D(meshes["specialPreview"], shaders["VertexColor"], tank->modelMatrixGun);
        }
    }

    for (CannonBall* ball : cannonBalls) {
        RenderMesh2D(meshes["ball"], shaders["VertexColor"], ball->modelMatrix);
    }

    RenderTerrain();
}


void Tema1::FrameEnd()
{
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // When moving the tank we five it an initial speed + a factor given by the angle of the terrain:
    // if it's going uphill this factor will be negative
    // if it's going downhill this factor will be positive
    for (Tank* tank : tanks) {
        if (window->KeyHold(GLFW_KEY_A) && tank->id == 0) {
            tank->MoveTank(heightMap, deltaTime, -movementSpeed - terrainInflSpeed * sin(tank->tankAngle));
        }

        if (window->KeyHold(GLFW_KEY_D) && tank->id == 0) {
            tank->MoveTank(heightMap, deltaTime, movementSpeed - terrainInflSpeed * sin(tank->tankAngle));
        }

        if (window->KeyHold(GLFW_KEY_W) && tank->id == 0) {
            tank->RotateGun(deltaTime, angularSpeed);
        }

        if (window->KeyHold(GLFW_KEY_S) && tank->id == 0) {
            tank->RotateGun(deltaTime, -angularSpeed);
        }
        if (window->KeyHold(GLFW_KEY_LEFT) && tank->id == 1) {
            tank->MoveTank(heightMap, deltaTime, -movementSpeed - terrainInflSpeed * sin(tank->tankAngle));
        }

        if (window->KeyHold(GLFW_KEY_RIGHT) && tank->id == 1) {
            tank->MoveTank(heightMap, deltaTime, movementSpeed - terrainInflSpeed * sin(tank->tankAngle));
        }

        if (window->KeyHold(GLFW_KEY_UP) && tank->id == 1) {
            tank->RotateGun(deltaTime, angularSpeed);
        }

        if (window->KeyHold(GLFW_KEY_DOWN) && tank->id == 1) {
            tank->RotateGun(deltaTime, -angularSpeed);
        }
    }

}


void Tema1::OnKeyPress(int key, int mods)
{
    for (Tank* tank : tanks) {
        if (key == GLFW_KEY_SPACE && tank->id == 0) {
            tank->LaunchBall(ballReleaseSpeed, cannonBalls, false);
        }

        if (key == GLFW_KEY_ENTER && tank->id == 1) {
            tank->LaunchBall(ballReleaseSpeed, cannonBalls, false);
        }

        if (tank->hitCount == tank->specialCount && key == GLFW_KEY_LEFT_CONTROL && tank->id == 0) {
            tank->LaunchBall(5 * ballReleaseSpeed, cannonBalls, true);
            tank->hitCount = 0;
        }

        if (tank->hitCount == tank->specialCount && key == GLFW_KEY_RIGHT_CONTROL && tank->id == 1) {
            tank->LaunchBall(5 * ballReleaseSpeed, cannonBalls, true);
            tank->hitCount = 0;
        }
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    float sensivityOX = 0.001f;
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        for (Tank* tank : tanks) {
            if (tank->id == 0) {
                tank->gunAngle -= deltaX * sensivityOX;
            }
        }
        
    }

    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT))
    {
        for (Tank* tank : tanks) {
            if (tank->id == 1) {
                tank->gunAngle -= deltaX * sensivityOX;
            }
        }

    }
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
