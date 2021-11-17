/**
 *   @file     pellet.h
 *   @author   Axel Jacobsen
 */

#ifndef __PELLET_H
#define __PELLET_H

#include "include.h"
#include "camera.h"
 /**
  *  Pellet class used to store data about all pellets, and
  *  apply functions.
  */

class Pellet {
private:
    int     XYpos[2];                   //Pellets XY
    const int pVerticeAmmount = 4 * 3;
    bool    enabled = true;             //Decides whether to do collision or not
    GLfloat vertices[4 * 3] = { 0.0f }; //Hold pellets vertices X Y Z
    std::vector<int> test;
    GLuint pelletShaderProgram;
    GLuint pelletVAO;
    std::pair<float, float> XYshift{ 0,0 };
    std::pair<int, int> WidthHeight{ 0,0 };
    
    Camera* pCamHolder;

public:
    Pellet() {};
    Pellet(int x, int y, std::pair<float, float> shift);
    ~Pellet() {
        delete vertices;
        delete XYpos;
    };
    void initCoords();
    GLfloat getVertCoord(int index);
    void removePellet();
    int  checkCoords(int XY);
    bool isEnabled();
    void callCompilePelletShader();
    void drawPellets(const int size);
    void cleanPellets();
    void cleanPelletVAO();
    int  getVertSize() { return sizeof(vertices) / sizeof(vertices[0]); }
    void callCreatePelletVAO(GLfloat* object, int size, const int stride);
    void   setVAO(const GLuint vao);
    GLuint getVAO();
    GLuint getShader();
    void  getPelletCameraPointer(Camera* newCamera) { pCamHolder = newCamera; };
    void  pelletSetWidthHeight(std::pair<int, int> widthHeidht) { WidthHeight = widthHeidht; };
};

#endif