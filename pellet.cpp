// -----------------------------------------------------------------------------
// Pellet Class
// -----------------------------------------------------------------------------

#include "pellet.h"

/**
 *  Initializes pellet with x and y
 *
 *  @param    x - pellets designated x
 *  @param    y - pellets designated y
 *  @see      Pellet::initCoords();
 */
Pellet::Pellet(int x, int y) {
    XYpos[0] = x; XYpos[1] = y;
    initCoords();
};

/**
 *  Initializes pellet vertices
 *
 *  @see   GLfloat getCoordsWithInt(int y,   int x,   int type);
 */
void Pellet::initCoords() {
    int loop = 0;
    float Xquart = Xshift / 3.0f;
    float Yquart = Yshift / 3.0f;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 3; x++) {
            vertices[loop] = (getCoordsWithInt(XYpos[1], XYpos[0], loop, 0));
            switch (loop) {
            case 0:  vertices[loop] += Xquart; break;
            case 1:  vertices[loop] += Yquart; break;

            case 3:  vertices[loop] += Xquart; break;
            case 4:  vertices[loop] -= Yquart; break;

            case 6:  vertices[loop] -= Xquart; break;
            case 7:  vertices[loop] -= Yquart; break;

            case 9:  vertices[loop] -= Xquart; break;
            case 10: vertices[loop] += Yquart; break;
            default: vertices[loop] = 0.0f;   break;
            }
            loop++;
        }
    }
}

/**
 *  sets Pellet vertices to 0, effectively removing it
 */
void Pellet::removePellet() {
    if (enabled) {
        for (int i = 0; i < 12; i++) {
            vertices[i] = 0.0f;
        }
        enabled = false;
        permittPelletUpdate = true;
    }
}

/**
 *  Returns desired vertex from pellet
 *
 *  @param    index - desired vertex
 *
 *  @return   pellets desired vertex
 */
GLfloat Pellet::getVertCoord(int index) {
    return vertices[index];
}

/**
 *  Returns XYpos if pellet is enabled
 *
 *  @param    XY - either x or y
 *
 *  @return either pellet XY or -1 if disabled
 */
int Pellet::checkCoords(int XY) {
    if (enabled) return XYpos[XY];
    return -1;
}

/**
 *  Returns if pellet is enabled
 *
 *  @return if the pellet is enabeled or not
 */
bool Pellet::isEnabled() {
    return enabled;
}
