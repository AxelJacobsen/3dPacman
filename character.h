/**
 *   Header til Kunde klassen.
 *
 *   @file     kunde.h
 *   @author   Axel Jacobsen
 */

#ifndef __CHARACTER_H
#define __CHARACTER_H

 // -----------------------------------------------------------------------------
 // Classes
 // -----------------------------------------------------------------------------
 /**
  *  Character class used for both Pacman and Ghosts
  *  Couldve Used two subclasses, but realized that most functions are used by both,
  *  so i decided to just keep them togheter
  */
class Character {
private:
    //Shared Values
    float   lerpStart[2],               //Contains start coords of LERP
        lerpStop[2],                //Contains stop  coords of LERP
        surround[3][3][2],          //Coords for surrounding walls
        speedDiv = 20.0f,           //Higher number = slower speed
        lerpStep = 1.0f / speedDiv,   //Speed of LERP, also slowed by frequency in main
        lerpProg = lerpStep;        //defines progress as step to avoid hickups

    int     dir,                        //Direction character is heading
        cardDir,                    //Cardinal direction character is heading
        prevDir,                    //Previous direction character was heading
        XYpos[2],                   //Value of characters current XY
        animVal = 0;                //A number between 0 - 3 used to define

    bool    animFlip = true;            //For ghosts flipflops between frames for pac decides which direction to animate
    GLfloat vertices[4 * 5] = { 0.0f }; //Holds character vertices,  X Y Z T1 T2


    //AI values
    bool    AI = false;                 //Decides whether object is pacman or ghost
    int     AIdelay = dir;              //Delay for deciding direction of ghost

    //Initialization functions
    void characterInit();
    void convertToVert();
public:
    std::vector<std::vector<int>>   mapI; //Holds the level0 map in Pacman[0]
    std::vector<std::vector<float>> mapF; //Holds the level0 map coordinates in Pacman[0]
    Character() {};
    Character(int x, int y);
    Character(int x, int y, bool ai);
    ~Character() {
        delete vertices;
        delete XYpos;
        delete lerpStart;
        delete lerpStop;
    };
    //Shared Functions
    bool getLegalDir(int dir);
    void getLerpCoords();
    void changeDir();
    void Transform(const GLuint ShaderProgram);
    void updateLerp();
    void updateDir(int outDir);
    GLfloat getVertCoord(int index);
    void checkPellet();
    bool checkGhostCollision();
    void characterAnimate(float hMin, float wMin, float hMax, float wMax);
    void pacAnimate();
    void recieveMapInt(std::vector<std::vector<int>> lvlVectInt);
    void recieveMapfloat(float coord, int Y);
    int  getMapVal(int x, int y);
    void updateCard(int newDir);
    int  getCard();
    //AI functions
    int   getRandomAIdir();
    void  AIupdateVertice();
    float AIgetLerpPog();
    int   AIgetXY(int xy);
    void  AIanimate();
};

#endif
