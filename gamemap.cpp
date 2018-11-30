//
// Created by eric on 11/6/18.
//

#include "gamemap.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace glm;
using namespace std;

GameMap::GameMap(string filename) {
    ifstream mapFile;
    mapFile.open(filename);
    mapFile >> height >> width;
    width += 2; // To create exterior walls
    height += 2;
    printf("Width: %d Height: %d\n", width, height);
    map = vector<vector<char> >(width, std::vector<char>(height));
    char node;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (i == 0 || j == 0 || i == width - 1 || j == height - 1) {
                node = 'W';
            } else {
                mapFile >> node;
            }
            map[i][j] = node;
        }
    }
    for(int i = 0; i < map.size(); i++){
        for(int j = 0; j < map[0].size(); j++){
            printf(" %c", map[i][j]);
        }
        printf("\n");
    }
}

void GameMap::GenerateMap(int vBegin, int numVerts) {
    char node;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            node = map[i][j];
            if (node == 'S') {
                cameraPos = vec3(i, j, 0);
            } else if (node == 'W') {
                walls.push_back(new Entity(i, j, vBegin, numVerts, node, WALL));
            }
        }
    }
}

void GameMap::GenerateEntities(int doorBegin, int doorVerts, int keyBegin, int keyVerts, int goalBegin, int goalVerts) {
    char node;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            node = map[i][j];
            switch (node) {
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                    entities.push_back(new Entity(i, j, doorBegin, doorVerts, node, DOOR));
                    break;
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                    entities.push_back(new Entity(i, j, keyBegin, keyVerts, node, KEY));
                    break;
                case 'G':
                    entities.push_back(new Entity(i, j, goalBegin, goalVerts, node, GOAL));
                    break;
                default:
                    continue;
            }
        }
    }
}

const vec3 &GameMap::getCameraPos() const {
    return cameraPos;
}

const vector<Entity *> &GameMap::getWalls() const {
    return walls;
}

const vector<Entity *> &GameMap::getEntities() const {
    return entities;
}

const std::vector<float> GameMap::getFloor() const {
    float w = (float) width;
    float h = (float) height;
    vector<float> floor{
            //X   Y    Z    U   V     NORMAL
            0.f, 0.f, -0.5f, 0.f, 0.f, 0.f, 0.f, 1.f,
            w, 0.f, -0.5f, w, 0.f, 0.f, 0.f, 1.f,
            w, h, -0.5f, w, h, 0.f, 0.f, 1.f,
            0.f, 0.f, -0.5f, 0.f, 0.f, 0.f, 0.f, 1.f,
            w, h, -0.5f, w, h, 0.f, 0.f, 1.f,
            0.f, h, -0.5f, 0.f, h, 0.f, 0.f, 1.f,
    };
    return floor;
}

bool GameMap::RemoveEntity(Entity *e) {
    auto it = std::find(entities.begin(), entities.end(), e);
    if (it != entities.end()) {
        entities.erase(it);
        printf("Erasing %c\n", e->getName());
        return true;
    }

    return false;
}

