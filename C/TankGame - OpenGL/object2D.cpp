#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateRectangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    float width,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, width, 0), color),
        VertexFormat(corner + glm::vec3(0, width, 0), color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

Mesh* object2D::CreateTank(const std::string& name, glm::vec3 leftBottomCorner)
{
    glm::vec3 corner = leftBottomCorner;
    glm::vec3 colorBottom(0.45f, 0.39f, 0.30f);
    glm::vec3 colorTop(0.8f, 0.68f, 0.53f);
    float scRadius = 1.75f;
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, colorBottom), // 0
        VertexFormat(corner + glm::vec3(5, 0, 0), colorBottom), // 1
        VertexFormat(corner + glm::vec3(-1, 1, 0), colorBottom), // 2
        VertexFormat(corner + glm::vec3(6, 1, 0), colorBottom), // 3
        VertexFormat(corner + glm::vec3(-2.5f, 1, 0), colorTop), // 4
        VertexFormat(corner + glm::vec3(-1.25f, 3.5f, 0), colorTop), // 5
        VertexFormat(corner + glm::vec3(7.5f, 1, 0), colorTop), // 6
        VertexFormat(corner + glm::vec3(6.25f, 3.5f, 0), colorTop), // 7

        // Semicircle centre
        VertexFormat(corner + glm::vec3(2.5f, 3.5f, 0), colorTop) // 8
    };

    int division = 100;
    glm::vec3 center = corner + glm::vec3(2.5f, 3.5f, 0);
    float angle = glm::pi<float>() / division;
    for (int i = 0; i <= division; i++) {
        float pointX = center.x + cos(i * angle) * scRadius;
        float pointY = center.y + sin(i * angle) * scRadius;
        glm::vec3 point(pointX, pointY, 0);
        vertices.push_back(VertexFormat(glm::vec3(pointX, pointY, 0), colorTop));
    }

    Mesh* tankMesh = new Mesh(name);
    std::vector<unsigned int> indices = {
        0, 1, 2, 
        1, 3, 2,
        4, 6, 5,
        5, 6, 7};

    for (int i = 0; i < division; i++) {
        indices.push_back(8);
        indices.push_back(i + 9);
        indices.push_back(i + 10);
    }

    tankMesh->InitFromData(vertices, indices);
    return tankMesh;
}


Mesh* object2D::CreateSphere(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill) {
    int division = 200;
    float angle = 2 * glm::pi<float>() / division;
    std::vector<VertexFormat> vertices = {VertexFormat(center, color)};
    for (int i = 0; i <= division; i++) {
        float pointX = center.x + cos(i * angle) * radius;
        float pointY = center.y + sin(i * angle) * radius;
        glm::vec3 point(pointX, pointY, 0);
        vertices.push_back(VertexFormat(glm::vec3(pointX, pointY, 0), color));
    }
    std::vector<unsigned int> indices = {};
    for (int i = 0; i < division; i++) {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }

    Mesh* sphereMesh = new Mesh(name);
    sphereMesh->InitFromData(vertices, indices);
    return sphereMesh;
}
