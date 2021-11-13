#include "character.h"

// -----------------------------------------------------------------------------
// Class function definition
// -----------------------------------------------------------------------------

/**
 *  Initializes Pacman
 *
 *  @param    x  - Initialization pos X
 *  @param    y  - Initializaiton pos Y
 *  @see      Character::characterInit();
 */
Character::Character(int x, int y) {
    dir = 9, prevDir = 3;
    XYpos[0] = x, XYpos[1] = y;
    characterInit();
};

/**
 *  Initializes Ghosts
 *
 *  @param    x  - Initialization pos X
 *  @param    y  - Initializaiton pos Y
 *  @param    ai - upsates AI to define as Ghost
 *  @see      Character:: getRandomAIdir();
 *  @see      Character:: characterInit();
 */
Character::Character(int x, int y, bool ai) {
    XYpos[0] = x, XYpos[1] = y;
    AI = ai;
    dir = getRandomAIdir();
    prevDir = dir;
    AIdelay = dir;
    characterInit();
};

/**
 *  Initializes LERP coords
 *
 *  @see      Character:: convertToVert();
 *  @see      Character:: pacAnimate();
 *  @see      Character:: AIanimate();
 */
void Character::characterInit() {
    convertToVert();
    lerpStart[0] = vertices[0];
    lerpStart[1] = vertices[1];
    if (dir == 2) {
        lerpStop[0] = vertices[5];
        lerpStop[1] = vertices[6];
    }
    else if (dir == 4) {
        lerpStop[0] = lerpStart[0];
        lerpStop[1] = lerpStart[1];
        lerpStop[1] -= Yshift;
    }
    else if (dir == 3) {
        lerpStop[0] = lerpStart[0];
        lerpStop[1] = lerpStart[1];
        lerpStop[0] -= Xshift;
    }
    else if (dir == 9) {
        lerpStop[0] = vertices[15];
        lerpStop[1] = vertices[16];
    }
    if (!AI) {
        pacAnimate();
    }
    else {
        AIanimate();
    }
}

/**
 *  Initializes verticies list
 *
 *  @see      GLfloat getCoordsWithInt(int y,   int x,   int type);
 */
void Character::convertToVert() {
    int loop = 0, callCount = 0;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 3; x++) {
            vertices[loop] = (getCoordsWithInt(XYpos[1], XYpos[0], callCount, 0));
            loop++; callCount++;
        }
        loop += 2;
    }
}

/**
 *  Checks if requested drection is legal/wall or not
 *
 *  @param    dir  - pacmans requested direction
 *  @return   bool whether is is a legal direction or not
 */
bool Character::getLegalDir(int dir) {
    int testPos[2] = { XYpos[0], XYpos[1] };
    switch (dir) {
    case 2: testPos[1] += 1; break;      //UP test
    case 4: testPos[1] -= 1; break;      //DOWN test
    case 3: testPos[0] -= 1; break;      //LEFT test
    case 9: testPos[0] += 1; break;      //RIGHT test
    }
    if ((testPos[0] < width && testPos[1] < height) && (0 <= testPos[0] && 0 <= testPos[1])) {
        if (Pacman[0]->getMapVal(testPos[1], testPos[0]) != 1) { return true; }
        else { return false; }
    }
    else { return false; }    //incase moving outside map illegal untill further notice
    return false;
};

/**
 *  Updates LERP coords
 */
void Character::getLerpCoords() {
    switch (dir) {
    case 2: XYpos[1] += 1; break;     //UP
    case 4: XYpos[1] -= 1; break;     //DOWN
    case 3: XYpos[0] -= 1; break;     //LEFT
    case 9: XYpos[0] += 1; break;     //RIGHT
    }

    lerpStop[0] = (XYpos[0] * Xshift);
    lerpStop[1] = ((XYpos[1] * Yshift) - 1);
    if (AI) lerpStop[0] -= 1;
};

/**
 *  Handles direction change requests
 *
 *  @see      Character:: getRandomAIdir();
 *  @see      Character:: getLegalDir(int dir);
 *  @see      Character:: getLerpCoords();
 */
void Character::changeDir() {
    bool legal = true;

    if (AI && AIdelay == 0) { dir = getRandomAIdir(); AIdelay = ((rand() + 4) % 10); }
    else {
        AIdelay--;
        legal = getLegalDir(dir);
    }

    if (prevDir == 0) { prevDir = dir; }    //Ensures that prevDir can never be 0 even if you hard-code it :)

    int modDir = prevDir;                   //New mod value used to ensure snappier movement
    if (legal && (dir < prevDir) && (prevDir % dir == 0)) { modDir = dir; } //This fixes problem where smaller numbers got mod'd by a larger number

    if (legal && (dir % modDir == 0) && dir != prevDir && !AI) {   //Incase you are trying to turn 180 degrees this procs
        float coordHolder[2];

        coordHolder[0] = lerpStop[0];      coordHolder[1] = lerpStop[1];
        lerpStop[0] = lerpStart[0];     lerpStop[1] = lerpStart[1];
        lerpStart[0] = coordHolder[0];   lerpStart[1] = coordHolder[1];

        if (lerpProg < 0.0f) { lerpProg = 1.0f; }            //This stops skiping a tile if lerpProg is over 1 or under 0
        else if (1.0f < lerpProg) { lerpProg = lerpStep / 2.0f; }
        else { lerpProg = (1 - lerpProg); }

        getLerpCoords();
        prevDir = dir;
    }
    if (legal && (lerpProg <= 0 || lerpProg >= 1)) {  //else this handles updating lerp
        lerpStart[0] = lerpStop[0];
        lerpStart[1] = lerpStop[1];
        getLerpCoords();
        lerpProg = lerpStep / 2.0f;
        prevDir = dir;
    }
};

/**
 *  passes Transform request onwards with aditional parameters
 *
 *  @param    ShaderProgram  - objects shaderprogram
 *  @see      void TransformPlayer(const GLuint shaderprogram, float lerpProg, float lerpStart[], float lerpStop[])
 */
void Character::Transform(const GLuint ShaderProgram) {
    TransformPlayer(ShaderProgram, lerpProg, lerpStart, lerpStop);
};

/**
 *  Handles LERP updates
 *
 *  @see      Character:: changeDir();
 *  @see      Character:: checkPellet();
 *  @see      Character:: AIupdateVertice();
 */
void Character::updateLerp() {
    if (lerpProg > 1 || lerpProg < 0) { changeDir(); }
    else { lerpProg += lerpStep; }

    if (0.5f <= lerpProg && lerpProg <= 0.6 && !AI) {
        checkPellet();
    }
    if (AI) { AIupdateVertice(); }
}

/**
 *  updatesDir with dir from key input
 *
 *  @param    outDir  - new dir from key input
 */
void Character::updateDir(int outDir) {
    dir = outDir;
}

/**
 *  Returns specific vertice
 *
 *  @param    index - desired index
 *  @return   returns the desired vertex
 */
GLfloat Character::getVertCoord(int index) {
    return vertices[index];
};

/**
 *  Checks whether pacman is on a pellet
 *
 *  @see      Pellets:: checkCoords( int XY);
 *  @see      Pellets:: removePellet();
 */
void Character::checkPellet() {
    int test = 0;
    for (auto& it : Pellets) {

        int check = 0;
        for (int i = 0; i < 2; i++) {
            if (XYpos[i] == it->checkCoords(i) && it->isEnabled()) { check++; }
        }
        if (check == 2) {
            it->removePellet();
            //Pellets.erase(it);
            break;
        }
        test++;
    }
};

/**
 *  Checks if pacman is crashing with ghost
 *
 *  @see      Character:: AIgetXY();
 *  @see      Character:: AIgetLerpPog();
 *  @return   returns wheter or not pacman and ghost are crashing
 */
bool Character::checkGhostCollision() {
    int check = 0;
    for (auto& it : Ghosts) {
        check = 0;
        for (int u = 0; u < 2; u++) {
            if (it->AIgetXY(u) == XYpos[u]) { check++; }
        }
        if (check == 2) {
            if (AIgetLerpPog() <= (lerpProg + lerpStep) && (lerpProg - lerpStep) <= AIgetLerpPog()) { return true; }
        }
    }
    return false;
}

/**
 *  Updates texture coordinates
 *
 *  @param    hMin - smallets height point on spritesheet
 *  @param    wMin - smallets width  point on spritesheet
 *  @param    hMax - largest  height point on spritesheet
 *  @param    wMax - largest  height point on spritesheet
 */
void Character::characterAnimate(float hMin, float wMin, float hMax, float wMax) {
    vertices[3] = wMin;   vertices[4] = hMax; // Bot Left
    vertices[8] = wMin;   vertices[9] = hMin; // Top Left
    vertices[13] = wMax;   vertices[14] = hMin; // Top Right
    vertices[18] = wMax;   vertices[19] = hMax; // Bot Right
}

/**
 *  Handles pacman animation
 *
 *  @see  Character:: characterAnimate(float hMin, float wMin, float hMax, float wMax);
 */
void Character::pacAnimate() {
    if (animFlip) { animVal++; }
    else { animVal--; }
    float wMod = (1.0f / 6.0f);
    float hMod = (1.0f / 4.0f);
    float mhMod = hMod;
    float mwMod = (wMod * animVal) + wMod;
    wMod *= animVal;

    switch (dir) {
    case 2: hMod *= 1; mhMod *= 2; break;   //UP
    case 4: hMod *= 0; mhMod *= 1; break;   //DOWN
    case 3: hMod *= 2; mhMod *= 3; break;   //LEFT
    case 9: hMod *= 3; mhMod *= 4; break;   //RIGHT
    }
    characterAnimate(hMod, wMod, mhMod, mwMod);
    if (animVal == 3) animFlip = false;
    else if (animVal == 0) animFlip = true;
}

/**
 *  Recieves lvlVect in to Pacman[0]
 */
void Character::recieveMapInt(std::vector<std::vector<int>> lvlVectInt) {
    mapI = lvlVectInt;
};

/**
 *  Recieves lvlVect float in to Pacman[0]
 */
void Character::recieveMapfloat(float coord, int Y) {
    mapF[Y].push_back(coord);
};

/**
 *  Returns map value
 *
 *  @return returns if coord is wall or not
 */
int Character::getMapVal(int x, int y) {
    return mapI[x][y];
};

/**
 *  Returns map value
 *
 *  @return returns if coord is wall or not
 */
void Character::updateCard(int newDir) {
    cardDir = newDir;
};

/**
 *  Returns map value
 *
 *  @return returns if coord is wall or not
 */
int  Character::getCard() {
    return cardDir;
}

/**
 *  Bruteforces a legal direction for AI
 *
 *  @see      Character::getLegalDir(int dir);
 *  @return   returns a legal direction for the AI to take
 */
int Character::getRandomAIdir() {
    int temp = 0;
    do {
        temp = (rand() % 4);
        switch (temp)
        {
        case 0: temp = 2;    break;
        case 1: temp = 4;    break;
        case 2: temp = 3;    break;
        case 3: temp = 9;    break;
        }
    } while (!getLegalDir(temp));
    return temp;
}

/**
 *  Handles AI movement
 */
void Character::AIupdateVertice() {
    for (int f = 0; f < (4 * 5); f += 5) {
        for (int k = f; k < (f + 3); k++) {
            if (k == f) {
                vertices[k] = (((1 - lerpProg) * lerpStart[0]) + (lerpProg * lerpStop[0]));
            }
            else if (k == (f + 1)) {
                vertices[k] = (((1 - lerpProg) * lerpStart[1]) + (lerpProg * lerpStop[1]));
            }
            switch (k) {
            case 0:   vertices[k];            break;
            case 1:   vertices[k];            break;

            case 5:   vertices[k];            break;
            case 6:   vertices[k] += Yshift;  break;

            case 10:  vertices[k] += Xshift;  break;
            case 11:  vertices[k] += Yshift;  break;

            case 15:  vertices[k] += Xshift;  break;
            case 16:  vertices[k];            break;
            default:  vertices[k] = 0.0f;     break;
            }
        }
    }
}

/**
 *  Returns the AIs lerpProg
 *
 *  @return   AI lerpProg
 */
float Character::AIgetLerpPog() {
    return lerpProg;
}

/**
 *  Returns the AIs XY
 *
 *  @return   AI XY
 */
int Character::AIgetXY(int xy) {
    return XYpos[xy];
}

/**
 *  Handles AI animation
 *
 *  @see  Character:: characterAnimate(float hMin, float wMin, float hMax, float wMax);
 */
void Character::AIanimate() {
    float wMod = (1.0f / 6.0f);
    float hMod = (1.0f / 4.0f);
    float mhMod = hMod, mwMod = wMod;
    if (!animFlip) {
        animFlip = true;
        animVal = 1;
        wMod *= 5;
        mwMod = 1.0f;
    }
    else {
        animFlip = false;
        animVal = 0;
        wMod *= 4;
        mwMod *= 5;
    }
    switch (dir) {
    case 2: hMod *= 1; mhMod *= 2; break;   //UP
    case 4: hMod *= 0; mhMod *= 1; break;   //DOWN
    case 3: hMod *= 2; mhMod *= 3; break;   //LEFT
    case 9: hMod *= 3; mhMod *= 4; break;   //RIGHT
    }
    characterAnimate(hMod, wMod, mhMod, mwMod);
};