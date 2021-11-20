/**
 *   Header til Pacman klassen.
 *
 *   @file     pacman.h
 *   @author   Axel Jacobsen
 */

#ifndef __PACMAN_H
#define __PACMAN_H

#include "character.h"

 // -----------------------------------------------------------------------------
 // Pacman Class
 // -----------------------------------------------------------------------------
class Pacman : public Character {
private:
    int cardDir     = 0,                //Cardinal direction character is heading
        collected   = 0,
        animDelay   = 10,
        animVal     = 0;                //A number between 0 - 3 used to define
    GLuint pacSpriteSheet;
    bool updatePellet = false,
         run = true;
public:
    Pacman() {};
    Pacman(std::pair<int,int> XY, std::pair<float, float> xyshift);
    ~Pacman() {};

    virtual void changeDir();
    virtual void updateLerp();
    void compilePacShader();
    void updateDir(int outDir);
    void pacAnimate();
    void updateCard(int newDir);
    int  getCard();
    void setCard(int card) { cardDir = card; };
    void loadPacSpriteSheet();
    void deletePacSpriteSheet();
    bool updatePelletState(bool change);
    void pickupPellet() { collected++; };
    int  getPellets()   { return collected; };
    void updateAnimDel(int val, bool set);
    int  getAnimDel()   { return animDelay; };
    bool getRun()       { return run; };
    float getLerpProg() { return lerpProg; }
    void setRun(const bool runUpdate) { run = runUpdate; };
    GLuint compilePacman();
    void drawPacman();
    void transformPacman();
    void moveCamera(float x, float y);
    void checkForKeyUpdate();
};

#endif
