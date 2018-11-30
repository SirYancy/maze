//
// Created by eric on 11/6/18.
//

#ifndef MAZE_MAP_H
#define MAZE_MAP_H

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "entity.h"

#if defined(__APPLE__) || defined(__linux__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif

class Entity;

class GameMap {

private:
    int width, height;
    glm::vec3 cameraPos;
    std::vector<std::vector<char> > map;
    std::vector<Entity *> walls;
    std::vector<Entity *> entities;

public:
    explicit GameMap(std::string filename);

    void GenerateMap(int vBegin, int numVerts);

    void GenerateEntities(int doorBegin, int doorVerts, int keyBegin, int keyVerts, int goalBegin, int goalVerts);

    const glm::vec3 &getCameraPos() const;

    const std::vector<Entity *> &getWalls() const;

    const std::vector<Entity *> &getEntities() const;

    const std::vector<float> getFloor() const;

    bool RemoveEntity(Entity *e);

};

#endif //MAZE_MAP_H
