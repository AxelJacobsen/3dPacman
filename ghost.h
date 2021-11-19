/**
 *   Header til Kunde klassen.
 *
 *   @file     ghost.h
 *   @author   Axel Jacobsen
 */

#ifndef __GHOST_H
#define __GHOST_H

#include "character.h"
#include "tiny_obj_loader.h"

 // -----------------------------------------------------------------------------
 // Ghost
 // -----------------------------------------------------------------------------
class Ghost : public Character {
private:
    int AIdelay = dir;  //Delay for deciding direction of ghost
    int modelSize = 0;
    GLuint modelShadowShader;
    GLuint ShadowShader;
    GLuint floorVAO;

    const GLuint SHADOW_WIDTH = 1;
    const GLuint SHADOW_HEIGHT = 1;

    GLuint  shadowmapFrameBuffer, 
            depthMap;

    //------------------------------------------------------------------------------
    // VERTEX STRUCT
    //------------------------------------------------------------------------------
    struct Vertex
    {
        glm::vec3 location;
        glm::vec3 normals;
        glm::vec2 texCoords;
    };


public:
    Ghost() {};
    Ghost(int x, int y, bool ai, std::pair<int, int> widthheight, std::pair<float, float> xyshift, Camera* campoint);
    ~Ghost() {};

    virtual void changeDir();
    virtual void updateLerp();
    void compileGhostShader();
    void compileGhostModelShader();
    void compileGhostShadowShader();
    
    bool  checkGhostCollision(float pacX, float pacY, std::pair<float, float> xyshift);
    int   ghostGetRandomDir();
    void  ghostUpdateVertice();
    float ghostGetLerpPog();
    int   ghostGetXY(int xy);
    void  ghostAnimate();
    int   getModelSize() { return modelSize; }
    void  drawGhosts(const int ghostCount);
    void  drawGhostsAsModels(float currentTime, const GLuint shadProg, std::pair<int, int> WH, const GLuint vao, const int size);
    void  transformGhost(GLuint shaderProg, float currentTime);
    void  deleteGhostSpriteSheet();
    void  callLoadModel() {
        std::pair<GLuint, int> VAOsize = LoadModel("assets/model/ghost", "/ghostModel.obj");
        characterVAO = VAOsize.first; modelSize = VAOsize.second;
        //floorVAO = CreateSquare();
    };
    std::pair<GLuint, int> LoadModel(const std::string path, const std::string objID);
    void ShadowMapping(GLuint vertexarray, GLuint modelShaderprogram, GLuint depthmapfb, int size, GLuint SHADOW_WIDTH, GLuint SHADOW_HEIGHT);
    void Light(
        const GLuint shaderprogram,
        const glm::vec3 pos = { 0.f, 0.f, 1.f },
        const glm::vec3 color = { 1.f,1.f,1.f },
        const glm::mat4 light_Projection = glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.01f, 2.f),
        const glm::vec3 look_at = { 0.f,0.f,0.f },
        const glm::vec3 up_vec = { 0.f,1.f,0.f },
        const float specularity = 0.2f
    );
    void Transform(
        const GLuint shaderprogram,
        const glm::vec3& translation = { 0.f,0.f,0.f },
        const float& radians = 0.f,
        const glm::vec3& rotation_axis = { 0.f,1.f,0.f },
        const glm::vec3& scale = { 0.5f,0.5f,0.5f }
    );
    std::tuple<GLuint, GLuint> createShadowmap(const GLuint SHADOW_WIDTH, const GLuint SHADOW_HEIGHT);
    GLuint CreateSquare();
};

#endif
