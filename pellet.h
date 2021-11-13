/**
 *   @file     pellet.h
 *   @author   Axel Jacobsen
 */

#ifndef __PELLET_H
#define __PELLET_H

#include "globFunc.h"

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
public:
    Pellet() {};
    Pellet(int x, int y);
    ~Pellet() {
        delete vertices;
        delete XYpos;
    };
    void initCoords();
    GLfloat getVertCoord(int index);
    void removePellet();
    int checkCoords(int XY);
    bool isEnabled();
};

#endif
