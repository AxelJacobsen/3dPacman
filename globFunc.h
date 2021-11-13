/**
 *   Header til map klassen.
 *
 *   @file     globfunc.h
 *   @author   Axel Jacobsen
 */

#ifndef __GLOBFUNC_H
#define __GLOBFUNC_H

//Inclusions
#include "globals.h"

// -----------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// -----------------------------------------------------------------------------

void    callMapCoordinateCreation(std::vector<std::vector<int>> levelVect, std::vector<float>* map);
void    CleanVAO(GLuint& vao);
GLuint  CompileShader(const std::string& vertexShader, const std::string& fragmentShader);
GLuint  CreateMap(std::vector<GLfloat>* map, GLfloat* mapObj);
GLuint  CreateObject(GLfloat *object, int size, const int stride);
void    drawGhosts( const GLuint shader);
void    drawMap(    const GLuint shader, const GLuint vao, const int mapSize);
void    drawPacman( const GLuint shader);
void    drawPellets(const GLuint shader, const GLuint vao);
GLfloat getCoordsWithInt(int y,   int x,   int type, float layer);
GLuint  getIndices(      int out, int mid, int in);
GLuint  load_opengl_texture(const std::string& filepath, GLuint slot);
void    TransformMap(   const GLuint);
void    TransformPlayer(const GLuint, float lerpProg, float lerpStart[], float lerpStop[]);
void    applycamera(const GLuint shader);
void    moveCamera(float x, float y);
int     findWhatWalls(std::vector<std::vector<int>> levelVect, int x, int y);
int     howManyWalls(int num);
void    spawnGhost(const int hallCount);
int     checkCardinal(const float xRot, const float yRot);

#endif