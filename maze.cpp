#include <iostream>
#include <fstream>
#include <map>
#include "gamemap.h"
#include "glad/glad.h"
#include "camera.h"

#if defined(__APPLE__) || defined(__linux__)

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#endif

using std::ifstream;

bool fullscreen = false;
int screenWidth = 800;
int screenHeight = 600;

GLuint shaderProgram;
GLuint vao, vertexvbo, indexvbo;

GameMap *gameMap;
Camera *camera;
std::vector<float> dataBuffer;

// Colors for keys and doors
std::map<char, int> colorMap = {
        {'a',0}, {'A',0},
        {'b',1}, {'B',1},
        {'c',2}, {'C',2},
        {'d',3}, {'D',3},
        {'e',4}, {'E',4},
        {'G',5}
};

const char *mapfn = "../levels/level2.map";
const char *vertfn = "../shaders/vert.glsl";
const char *fragfn = "../shaders/frag.glsl";
const char *wallModel = "../models/cube.model";
const char *doorModel = "../models/door.model";
const char *keyModel = "../models/key.model";
const char *goalModel = "../models/knight.model";
const char *wallTexFN = "../textures/stone.bmp";
const char *wallNormalFN = "../textures/stoneN.bmp";
const char *floorNormalFN = "../textures/floorN.bmp";
const char *doorTexFN = "../textures/marble.bmp";
const char *keyTexFN = "../textures/gold.bmp";
const char *floorTexFN = "../textures/floor.bmp";

void SetupBuffers(float *d, unsigned int numVerts);

void LoadModel(const char *model, int *begin, int *numVerts);

int LoadShader(const char *filename, GLuint shaderID);

void CreateShaderProgram(GLuint &vertShader, GLuint &fragShader);

void LoadTexture(GLenum texture, GLuint *id, const char *filename, const char *samplerID, const int index);

void LoadFloor();

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

    //Print the version of SDL we are using
    SDL_version comp;
    SDL_version linked;
    SDL_VERSION(&comp);
    SDL_GetVersion(&linked);
    printf("\nCompiled against SDL version %d.%d.%d\n", comp.major, comp.minor, comp.patch);
    printf("Linked SDL version %d.%d.%d.\n", linked.major, linked.minor, linked.patch);

    //Ask SDL to get a recent version of OpenGL (3.2 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    //Create a window (offsetx, offsety, width, height, flags)
    SDL_Window *window = SDL_CreateWindow("My OpenGL Program", 100, 100,
                                          screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    if (!window) {
        printf("Could not create window: %s\n", SDL_GetError());
        return EXIT_FAILURE; //Exit as SDL failed
    }
    float aspect = screenWidth / (float) screenHeight; //aspect ratio needs update on resize

    SDL_GLContext context = SDL_GL_CreateContext(window); //Bind OpenGL to the window

    if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        printf("OpenGL loaded\n");
        printf("Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version:  %s\n", glGetString(GL_VERSION));
    } else {
        printf("ERROR: Failed to initialize OpenGL context.\n");
        return -1;
    }
    GLuint vertShader;
    GLuint fragShader;
    CreateShaderProgram(vertShader, fragShader);

    int begin = 0, numVerts = 0;

    gameMap = new GameMap(mapfn);

    // Load Walls
    LoadModel(wallModel, &begin, &numVerts);
    int wallBegin = begin;
    int wallNumV = numVerts;
    gameMap->GenerateMap(begin, numVerts);

    // Load Doors and Keys
    LoadModel(doorModel, &begin, &numVerts);
    int doorBegin = wallNumV;
    int doorNumV = numVerts;

    LoadModel(keyModel, &begin, &numVerts);
    int keyBegin = wallNumV + doorNumV;
    int keyNumV = numVerts;

    LoadModel(goalModel, &begin, &numVerts);
    int goalBegin = wallNumV + doorNumV + keyNumV;
    int goalNumV = numVerts;

    // Generate keys, doors, and goal
    gameMap->GenerateEntities(doorBegin, doorNumV, keyBegin, keyNumV, goalBegin, goalNumV);

    // Load the Floor
    LoadFloor();

    int floorBegin = wallNumV + doorNumV + keyNumV + goalNumV;
    int floorVerts = 6;

    // Setup Camera
    SDL_SetRelativeMouseMode(SDL_TRUE);
    vec3 camPos = gameMap->getCameraPos();
    camera = new Camera(gameMap, camPos, vec3(0,0,0), vec3(0,0,1.0f));

    float *d = dataBuffer.data();

    // Create Buffers
    SetupBuffers(d, dataBuffer.size() / 8);

    GLuint wallTex; //wall texture
    GLuint doorTex; //door texture
    GLuint goldTex; //gold texture
    GLuint floorTex; //floor stone texture
    GLuint wallNorm; //wall Normal Map
    GLuint floorNorm; //Floor Normal Map
    LoadTexture(GL_TEXTURE0, &wallTex, wallTexFN, "wallTex", 0);
    LoadTexture(GL_TEXTURE1, &doorTex, doorTexFN, "doorTex", 1);
    LoadTexture(GL_TEXTURE2, &goldTex, keyTexFN, "goldTex", 2);
    LoadTexture(GL_TEXTURE3, &floorTex, floorTexFN, "floorTex", 3);
    LoadTexture(GL_TEXTURE4, &wallNorm, wallNormalFN, "walLNorm", 4);
    LoadTexture(GL_TEXTURE5, &floorNorm, floorNormalFN, "floorNorm", 5);

    glEnable(GL_DEPTH_TEST);
    //Game Loop
    SDL_Event windowEvent;
    bool quit = false;

    int forward = 0;
    int strafe = 0;

    float dt = 0;
    float lastFrame = 0;

    while (!quit) {
        int x, y;
        SDL_GetRelativeMouseState(&x, &y);
        while (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) quit = true; //Exit Game Loop
            if (windowEvent.type == SDL_KEYDOWN) {
                switch (windowEvent.key.keysym.sym) {
                    case SDLK_w:
                        forward = 1;
                        break;
                    case SDLK_s:
                        forward = -1;
                        break;
                    case SDLK_a:
                        strafe = -1;
                        break;
                    case SDLK_d:
                        strafe = 1;
                        break;
                }
            }
            if (windowEvent.type == SDL_KEYUP) {
                switch (windowEvent.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        quit = true;
                        break;
                    case SDLK_f:
                        fullscreen = !fullscreen;
                        SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
                        break;
                    case SDLK_w:
                    case SDLK_s:
                        forward = 0;
                        break;
                    case SDLK_a:
                    case SDLK_d:
                        strafe = 0;
                        break;
                }
            }
        }

        glClearColor(.2f, .2f, .2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float currentFrame = SDL_GetTicks() / 1000.f;
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
        vec3 colVec(0.f, 0.7f, 0.f);
        glUniform3fv(uniColor, 1, glm::value_ptr(colVec));

        glm::mat4 model;
        GLint uniModel = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 view = camera->GetView(dt, forward, strafe, x, y);

        GLint uniView = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 proj = glm::perspective(3.14f / 4, aspect, 0.1f, 1000.0f);
        GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(vao);

        GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
        GLint colorID = glGetUniformLocation(shaderProgram, "colorID");

        glUniform1i(uniTexID, 0); //BRICKS

        const auto &world = gameMap->getWalls();
        for (auto wall : world) {
            model = wall->getMatrix();
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, wall->getStartVertex(), wall->getNumVertices());
        }

        const auto &entities = gameMap->getEntities();
        for (auto entity : entities) {
            char name = entity->getName();
            switch (entity->getType()) {
                case DOOR:
                    glUniform1i(uniTexID, 1);
                    glUniform1i(colorID, colorMap[name]);
                    break;
                case KEY:
                    glUniform1i(uniTexID, 2);
                    glUniform1i(colorID, colorMap[name]);
                    break;
                case GOAL:
                    glUniform1i(uniTexID, 1);
                    glUniform1i(colorID, colorMap[name]);
                    break;
                default:
                    continue; //This shouldn't happen
            }
            model = entity->getMatrix();
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, entity->getStartVertex(), entity->getNumVertices());
        }

        // Render the floor
        glUniform1i(uniTexID, 3);
        model = glm::mat4();
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, floorBegin, floorVerts);

        if(camera->isWin()){
            quit = true; //This is a terrible way to win the game.
        }

        SDL_GL_SwapWindow(window);
    }

    glDeleteProgram(shaderProgram);
    glDeleteShader(fragShader);
    glDeleteShader(vertShader);
    glDeleteBuffers(1, &indexvbo);
    glDeleteBuffers(1, &vertexvbo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_Quit();

    printf("CONGRATULATIONS!! You've Won...Something..\n");

    return 0;
}

void LoadFloor() {
    const auto &floor = gameMap->getFloor();
    dataBuffer.insert(std::end(dataBuffer), std::begin(floor), std::end(floor));
}

void LoadTexture(GLenum texture, GLuint *id, const char *filename, const char *samplerID, const int index) {

    SDL_Surface *surface = SDL_LoadBMP(filename);
    if (surface == nullptr) {
        printf("Error: \"%s\"\n", SDL_GetError());
    }

    glGenTextures(1, id);

    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, *id);

    glUniform1i(glGetUniformLocation(shaderProgram, samplerID), index);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(surface);
}

int LoadShader(const char *filename, GLuint shaderID) {
    ifstream file;
    file.open(filename, std::ios::in);
    if (!file) return -1;

    file.tellg();
    file.seekg(0, std::ios::end);
    int len = static_cast<int>(file.tellg());
    file.seekg(std::ios::beg);

    if (len == 0) return -1; //empty file

    GLchar *shaderSource = new GLchar[len + 1];
    shaderSource[len] = 0;

    int i = 0;
    while (file.good()) {
        shaderSource[i] = static_cast<GLchar>(file.get());
        if (!file.eof())
            i++;
    }

    shaderSource[i] = 0;
    file.close();

    glShaderSource(shaderID, 1, &shaderSource, NULL);
    glCompileShader(shaderID);

    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (!status) {
        char buffer[512];
        glGetShaderInfoLog(shaderID, 512, NULL, buffer);
        printf("Shader Compile Failed. Info:\n\n%s\n", buffer);
        return -1;
    }
    return 0;
}


void CreateShaderProgram(GLuint &vertShader, GLuint &fragShader) {
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    LoadShader(vertfn, vertShader);
    LoadShader(fragfn, fragShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
//     glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
}

void LoadModel(const char *model, int *begin, int *numVerts) {
    *begin = *numVerts;
    ifstream modelFile;
    modelFile.open(model);
    int numLines = 0;
    modelFile >> numLines;
    float v;
    for (int i = 0; i < numLines; i++) {
        modelFile >> v;
        dataBuffer.push_back(v);
    }
    *numVerts = numLines / 8;
    printf("Model: %d vertices loaded\n", numLines / 8);
}

void SetupBuffers(float *d, unsigned int numVerts) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertexvbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertexvbo);
    glBufferData(GL_ARRAY_BUFFER, numVerts * 8 * sizeof(float), d, GL_STATIC_DRAW);

    // Position
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    // Texture
    GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexCoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(texAttrib);

    // Normals
    GLint normAttrib = glGetAttribLocation(shaderProgram, "inNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(normAttrib);

}

