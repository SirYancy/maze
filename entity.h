//
// Created by eric on 11/13/18.
//

#ifndef MAZE_ENTITY_H
#define MAZE_ENTITY_H

#include <glad.h>
#include "glm/glm.hpp"
#include "camera.h"

using namespace glm;

enum Type {
    GOAL,
    WALL,
    DOOR,
    KEY
};

class Camera;

class Entity {

private:
    Type type;
    vec3 center;
    char name;
    glm::mat4 matrix;
    int startVertex;
    int numVertices;

    vec3 Clamp(vec3 diff, vec3 min, vec3 max);

public:
    Entity(int i, int j, int v, int n, char name, Type t);

    const mat4 &getMatrix() const;

    int getStartVertex() const;

    int getNumVertices() const;

    Type getType() const;

    const vec3 &getCenter() const;

    char getName() const;

    bool CheckCollision(const vec3 &cameraPos, const float radius);

    void Move(const vec3 pos, const mat4 lookAt);
};

#endif //MAZE_ENTITY_H
