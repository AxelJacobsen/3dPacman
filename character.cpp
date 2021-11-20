#include "character.h"

// -----------------------------------------------------------------------------
// Class function definition
// -----------------------------------------------------------------------------

/**
 *  Initializes LERP coords
 *
 *  @see Character:: convertToVert();
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
        lerpStop[1] -= XYshift.second;
    }
    else if (dir == 3) {
        lerpStop[0] = lerpStart[0];
        lerpStop[1] = lerpStart[1];
        lerpStop[0] -= XYshift.first;
    }
    else if (dir == 9) {
        lerpStop[0] = vertices[15];
        lerpStop[1] = vertices[16];
    }
}

/**
 *  Initializes verticies list
 *
 *  @see GLfloat getCoordsWithInt(int y, int x, int type);
 */
void Character::convertToVert() {
    int loop = 0, callCount = 0;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 3; x++) {
            vertices[loop] = (CamHolder->getCoordsWithInt(XYpos[1], XYpos[0], callCount, 0, XYshift));
            loop++; callCount++;
        }
        loop += 2;
    }
}

/**
 *  Checks if requested drection is legal/wall or not
 *
 *  @param    dir  - pacmans requested direction
 * 
 *  @see      Camera::getCamMapVal(int x, int y)
 * 
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
    if ((testPos[0] < (WidthHeight.first) && testPos[1] < (WidthHeight.second-1)) && (0 <= testPos[0] && 0 <= testPos[1])) {
        if (CamHolder->getCamMapVal(testPos[0], testPos[1]) != 1) { return true; }
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
    lerpStop[0] = (XYpos[0] * XYshift.first);
    lerpStop[1] = ((XYpos[1] * XYshift.second) - 1);
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
    
    bool legal = getLegalDir(dir);

    if (prevDir == 0) { prevDir = dir; }    //Ensures that prevDir can never be 0 even if you hard-code it :)

    int modDir = prevDir;                   //New mod value used to ensure snappier movement
    if (legal && (dir < prevDir) && (prevDir % dir == 0)) { modDir = dir; } //This fixes problem where smaller numbers got mod'd by a larger number

    if (legal && (dir % modDir == 0) && dir != prevDir) {   //Incase you are trying to turn 180 degrees this procs
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
 *  Handles LERP updates
 *
 *  @see      Character:: changeDir();
 *  @see      Character:: checkPellet();
 *  @see      Character:: AIupdateVertice();
 */
void Character::updateLerp() {
    if (lerpProg > 1 || lerpProg < 0) { changeDir(); }
    else { lerpProg += lerpStep; }
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
 *  Updates texture coordinates
 *
 *  @param    hMin - smallets height point on spritesheet
 *  @param    wMin - smallets width  point on spritesheet
 *  @param    hMax - largest  height point on spritesheet
 *  @param    wMax - largest  height point on spritesheet
 */
void Character::characterAnimate(float hMin, float wMin, float hMax, float wMax) {
    
    switch (dir) {
    case 2: hMin *= 1; hMax *= 2; break;   //UP
    case 4: hMin *= 0; hMax *= 1; break;   //DOWN
    case 3: hMin *= 2; hMax *= 3; break;   //LEFT
    case 9: hMin *= 3; hMax *= 4; break;   //RIGHT
    }

    vertices[3] = wMin;   vertices[4] = hMax; // Bot Left
    vertices[8] = wMin;   vertices[9] = hMin; // Top Left
    vertices[13] = wMax;   vertices[14] = hMin; // Top Right
    vertices[18] = wMax;   vertices[19] = hMax; // Bot Right
}

/**
 *  cleans Character values shaderprogram and VAO
 *
 *  @see CleanVAO(GLuint& vao)
 */
void Character::cleanCharacter() {
    glDeleteProgram(shaderProgram);
    if (characterVAO) CleanVAO(characterVAO);
};

/**
 *  Sets shader to be recieved version
 */
void Character::setShader(const GLuint shaderProg) {
    shaderProgram = shaderProg;
}

/**
 *  Updates character VAO
 */
void Character::setVAO(const GLuint vao) {
    characterVAO = vao;
}

/**
 *  Retruns shaderprogram
 *
 *  @return returns shaderprogram
 */
GLuint Character::getShader() {
    return shaderProgram;
}

/**
 *  retruns characterVAO
 *
 *  @return Returns character VAO
 */
GLuint Character::getVAO(){
    return characterVAO;
}

/**
 *  cleans Character values shaderprogram and VAO
 *
 *  @see CreateObject(GLfloat* object, int size, const int stride);
 */
void  Character::callCreateCharacterVao(GLfloat* object, int size, const int stride) {
    characterVAO = CreateObject(object, size, stride);
}
