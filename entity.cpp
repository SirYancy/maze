//
// Created by eric on 11/13/18.
//

#include <cstdio>
#include "entity.h"

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/common.hpp"

Entity::Entity(int i, int j, int v, int n, char name, Type t) :
        startVertex(v), numVertices(n), name(name), type(t) {
    matrix = glm::mat4();
    center = vec3(i, j, 0);
    matrix = glm::translate(matrix, center);
}

const mat4 &Entity::getMatrix() const {
    return matrix;
}

int Entity::getStartVertex() const {
    return startVertex;
}

int Entity::getNumVertices() const {
    return numVertices;
}

Type Entity::getType() const {
    return type;
}

const vec3 &Entity::getCenter() const {
    return center;
}

char Entity::getName() const {
    return name;
}

bool Entity::CheckCollision(const vec3 &cameraPos, const float radius) {
    float dx = cameraPos.x;
    float dy = cameraPos.y;

    if (cameraPos.y < center.y - 0.5)
        dy = center.y - 0.5;
    else if (cameraPos.y > center.y + 0.5)
        dy = center.y + 0.5;
    if (cameraPos.x < center.x - 0.5)
        dx = center.x - 0.5;
    else if (cameraPos.x > center.x + 0.5)
        dx = center.x + 0.5;

    vec3 wallPoint = vec3(dx, dy, 0.f);

    float distance = glm::distance(cameraPos, wallPoint);

    return distance <= radius;
}

vec3 Entity::Clamp(vec3 diff, vec3 min, vec3 max) {
    float x = std::max(min.x, std::max(max.x, diff.x));
    float z = std::max(min.z, std::max(max.z, diff.z));
    return vec3(x, 0.5, z);
}

void Entity::Move(const vec3 pos, const mat4 rot) {
    center = pos;
    matrix = glm::mat4();
//    matrix = matrix + rot;
    matrix = glm::translate(matrix, center);
}


