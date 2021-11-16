/**
 *   Header til Kunde klassen.
 *
 *   @file     ghost.h
 *   @author   Axel Jacobsen
 */

#ifndef __GHOST_H
#define __GHOST_H

#include "character.h"

 // -----------------------------------------------------------------------------
 // Ghost
 // -----------------------------------------------------------------------------
class Ghost : public Character {
private:
    int AIdelay = dir;  //Delay for deciding direction of ghost
public:
    Ghost() {};
    Ghost(int x, int y, bool ai);
    ~Ghost() {};

    virtual void changeDir();
    virtual void updateLerp();
    virtual void callCompileShader();
    

    bool  checkGhostCollision(float pacX, float pacY);
    int   ghostGetRandomDir();
    void  ghostUpdateVertice();
    float ghostGetLerpPog();
    int   ghostGetXY(int xy);
    void  ghostAnimate();
    void  drawGhosts(const int ghostCount);
};

#endif
