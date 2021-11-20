/**
 *   Header til Kunde klassen.
 *
 *   @file     kunde.h
 *   @author   Axel Jacobsen
 */

#ifndef __CHARACTER_H
#define __CHARACTER_H

#include "globFunc.h"
#include "camera.h"
 // -----------------------------------------------------------------------------
 // Classes
 // -----------------------------------------------------------------------------
 /**
  *  Character class used for both Pacman and Ghosts
  *  Couldve Used two subclasses, but realized that most functions are used by both,
  *  so i decided to just keep them togheter
  */
class Character {
protected:
    //Shared Values
    float   lerpStart[2],               //Contains start coords of LERP
            lerpStop[2],                //Contains stop  coords of LERP
            speedDiv = 15.0f,           //Higher number = slower speed
            lerpStep = 1.0f / speedDiv, //Speed of LERP, also slowed by frequency in main
            lerpProg = lerpStep;        //defines progress as step to avoid hickups

    int     dir,                        //Direction character is heading
            prevDir,                    //Previous direction character was heading
            XYpos[2];                   //Value of characters current XY

    bool    animFlip = true;            //For ghosts flipflops between frames for pac decides which direction to animate
    GLfloat vertices[4 * 5] = { 0.0f }; //Holds character vertices,  X Y Z T1 T2
    GLuint  shaderProgram,
            characterVAO,
            textureSheet;
    std::pair<float, float> XYshift{ 0,0 };
    std::pair<int, int> WidthHeight{ 0,0 };
    //Ghost values
    bool    AI = false;                 //Decides whether object is pacman or ghost

    Camera* CamHolder;
public:
    Character() {};
    ~Character() {
        delete vertices;
        delete XYpos;
        delete lerpStart;
        delete lerpStop;
    };

    //Initialization functions
    virtual void changeDir();
    virtual void updateLerp();
    void    characterInit();
    void    convertToVert();
    bool    getLegalDir(int dir);
    void    getLerpCoords();
    GLfloat getVertCoord(int index);
    void    characterAnimate(float hMin, float wMin, float hMax, float wMax);
    void    cleanCharacter();
    void    setShader(const GLuint shaderProg);
    void    setVAO(const GLuint vao);
    GLuint  getShader();
    GLuint  getVAO();
    void    callCleanVAO()  { if (characterVAO) CleanVAO(characterVAO); };
    int     getVertSize()   { return sizeof(vertices) / sizeof(vertices[0]); }

    void    callCreateCharacterVao(GLfloat* object, int size, const int stride);
    void    setXYshift(std::pair<float, float> XYvalues)    { XYshift = XYvalues; };
    void    setWidthHeight(std::pair<int, int> widthHeidht) { WidthHeight = widthHeidht; };
    void    getCameraPointer(Camera* newCamera)             { CamHolder = newCamera; };
    std::pair<int, int> getXY() { std::pair<int, int> temp= { XYpos[0], XYpos[1] }; return temp; }
};

#endif;