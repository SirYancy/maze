// Created by eric on 11/12/18.
//

#include <cstdio>
#include "camera.h"

Camera::Camera(GameMap *map, vec3 p, vec3 c, vec3 u) : map(map), inventory(nullptr){

    pos = p;
//    pos = vec3(0,0,5); // DEBUGGGING STUPID SHIT
    dir = glm::normalize(c + pos);
    up = glm::normalize(u);
}

mat4 Camera::GetView(float dt, int forward, int strafe, int mouseX, int mouseY) {
    float camSpeed = dt * linSpeed;

    yaw += mouseX * angleSpeed;
    pitch -= mouseY * angleSpeed;

    vec3 nDir;
    nDir.y = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    nDir.z = sin(glm::radians(pitch));
    nDir.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    dir = normalize(nDir);
    vec3 moveDir = normalize(vec3(dir.x, dir.y, 0));

    vec3 dPos = vec3();

    dPos += (camSpeed * forward * moveDir);
    dPos += (camSpeed * strafe * glm::normalize(glm::cross(moveDir, up)));

    pos += dPos;

    // Do Wall Collisions
    for (auto &w : map->getWalls()) {
        bool c = w->CheckCollision(pos, radius);
        if (c) {
            //check direction
            vec3 toWall = normalize(w->getCenter() - pos);

            if (abs(toWall.x) > abs(toWall.y)) {
                pos.x -= dPos.x;
            } else if (abs(toWall.y) > abs(toWall.x)) {
                pos.y -= dPos.y;
            }
        }
    }

    // Do other collisions
    for (auto &e: map->getEntities()) {
        bool c = e->CheckCollision(pos, radius);
        if (c) {
            if (e->getType() == DOOR) {
                //check direction
                if(inventory != nullptr && e->getName() == toupper(inventory->getName()))
                {
                    map->RemoveEntity(e);
                    map->RemoveEntity(inventory);
                }
                else {
                    vec3 toDoor = normalize(e->getCenter() - pos);

                    if (abs(toDoor.x) > abs(toDoor.y)) {
                        pos.x -= dPos.x;
                    } else if (abs(toDoor.y) > abs(toDoor.x)) {
                        pos.y -= dPos.y;
                    }
                }
            } else if (e->getType() == KEY && (inventory == nullptr || e->getName() != inventory->getName())) {
//                map->RemoveEntity(inventory);
                inventory = e;
                printf("Added: %c\n", inventory->getName());
            } else if (e->getType() == GOAL)
            {
                win = true;
            }
        }
    }

    glm::mat4 view = glm::lookAt(pos, pos + dir, up);

    if(inventory != nullptr){
        vec3 newPos = pos + (dir * 0.5f);
        inventory->Move(newPos, glm::transpose(view));
    }

    return view;
}

const vec3 &Camera::getPos() const {
    return pos;
}

float Camera::getRadius() const {
    return radius;
}

bool Camera::isWin() const {
    return win;
}
