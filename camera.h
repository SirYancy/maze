//
// Created by eric on 11/12/18.
//

#ifndef MAZE_CAMERA_H
#define MAZE_CAMERA_H

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "gamemap.h"

using namespace glm;

class GameMap;
class Entity;

class Camera {
private:
    bool win = false;
    GameMap *map;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float radius = 0.2f;
    Entity *inventory;
    vec3 pos;
    vec3 dir;
    vec3 up;

    float linSpeed = 1.8f;
    float angleSpeed = 0.5f;

public:
    Camera(GameMap *map, vec3 p, vec3 c, vec3 u);

    mat4 GetView(float dt, int forward, int strafe, int mouseX, int mouseY);

    const vec3 &getPos() const;

    float getRadius() const;

    bool isWin() const;

};


#endif //MAZE_CAMERA_H
