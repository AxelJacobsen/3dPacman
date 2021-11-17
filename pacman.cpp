// -----------------------------------------------------------------------------
// Class function definition
// -----------------------------------------------------------------------------

#include "pacman.h"

/**
 *  Initializes Pacman
 *
 *  @param    x  - Initialization pos X
 *  @param    y  - Initializaiton pos Y
 *  @see      Character::characterInit();
 */
Pacman::Pacman(std::pair<int, int> XY) {
    dir = 9, prevDir = 3;
    XYpos[0] = XY.first, XYpos[1] = XY.second;
    Character::characterInit();
    pacAnimate();
};

/**
 *  Handles direction change requests
 *
 *  @see      Character:: getRandomAIdir();
 *  @see      Character:: getLegalDir(int dir);
 *  @see      Character:: getLerpCoords();
 */
void Pacman::changeDir() {
    bool legal = true;
    if (prevDir == 0) { prevDir = dir; }    //Ensures that prevDir can never be 0 even if you hard-code it :)
    legal = Character::getLegalDir(dir);

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

        Character::getLerpCoords();
        prevDir = dir;
    }
    if (legal && (lerpProg <= 0 || lerpProg >= 1)) {  //else this handles updating lerp
        lerpStart[0] = lerpStop[0];
        lerpStart[1] = lerpStop[1];
        Character::getLerpCoords();
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
void Pacman::updateLerp() {
    if (lerpProg > 1 || lerpProg < 0) { changeDir(); }
    else { lerpProg += lerpStep; }

    if (0.5f <= lerpProg && lerpProg <= 0.6 && !AI) {
        checkPellet();
    }
}

/**
 *  Handles LERP updates
 *
 *  @see      Character:: changeDir();
 *  @see      Character:: checkPellet();
 *  @see      Character:: AIupdateVertice();
 */
void Pacman::compilePacShader() {
    shaderProgram = CompileShader(  playerVertexShaderSrc,
                                    playerFragmentShaderSrc);

    GLint pposAttrib = glGetAttribLocation(shaderProgram, "pPosition");
    glEnableVertexAttribArray(pposAttrib);
    glVertexAttribPointer(pposAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
}

/**
 *  updatesDir with dir from key input
 *
 *  @param    outDir  - new dir from key input
 */
void Pacman::updateDir(int outDir) {
    dir = outDir;
}

/**
 *  Checks whether pacman is on a pellet
 *
 *  @see      Pellets:: checkCoords( int XY);
 *  @see      Pellets:: removePellet();
 */
void Pacman::checkPellet() {
    /*
    int test = 0;
    for (auto& it : Pellets) {
        int check = 0;
        for (int i = 0; i < 2; i++) {
            if (XYpos[i] == it->checkCoords(i) && it->isEnabled()) { check++; }
        }
        if (check == 2) {
            it->removePellet();
            break;
        }
        test++;
    }
    */
};

/**
 *  Checks if pacman is crashing with ghost
 *
 *  @see      Character:: AIgetXY();
 *  @see      Character:: AIgetLerpPog();
 *  @return   returns wheter or not pacman and ghost are crashing
 */
void Pacman::callGhostCollision() {
    /*
    for (auto& it : Ghosts) {
        if (!(it->checkGhostCollision(vertices[0], vertices[1]))) { run = false; }
    }
    */
}

/**
 *  Handles pacman animation
 *
 *  @see  Character:: characterAnimate(float hMin, float wMin, float hMax, float wMax);
 */
void Pacman::pacAnimate() {
    if (animFlip) { animVal++; }
    else { animVal--; }
    float wMod = (1.0f / 6.0f);
    float hMod = (1.0f / 4.0f);
    float mhMod = hMod;
    float mwMod = (wMod * animVal) + wMod;
    wMod *= animVal;

    Character::characterAnimate(hMod, wMod, mhMod, mwMod);
    if (animVal == 3) animFlip = false;
    else if (animVal == 0) animFlip = true;
}

/**
 *  Returns map value
 *
 *  @return returns if coord is wall or not
 */
void Pacman::updateCard(int newDir) {
    cardDir = newDir;
};

/**
 *  Returns map value
 *
 *  @return returns if coord is wall or not
 */
int  Pacman::getCard() {
    return cardDir;
}

/**
 *  Returns map value
 *
 *  @return returns if coord is wall or not
 */
void Pacman::loadPacSpriteSheet() {
    pacSpriteSheet = load_opengl_texture("assets/pacman.png", 0);
}

/**
 *  Returns map value
 *
 *  @return returns if coord is wall or not
 */
void Pacman::deletePacSpriteSheet() {
    glDeleteTextures(1, &pacSpriteSheet);
}

bool Pacman::updatePelletState(bool change) {
    if (change) {
        if (updatePellet) {
            updatePellet = false;
            return true;
        }
        else { updatePellet = true; return false; }
    }
    return updatePellet;
}

void Pacman::updateAnimDel(int val, bool set) {
    if (set)    { animDelay = val; }
    else        { animDelay += val; }
}

/**
 *  Compiles all verticie lists into a large vector and calls CreateObject
 *
 *  @param itObj - which type of object to iterate, here either Pacman or ghost
 *
 *  @see Character::getVertCoord(int index);
 *  @see GLuint CreateObject(GLfloat* object, int size, const int stride);
 *
 *  @return returns VAO gotten from CreateObject func
 */
GLuint Pacman::compilePacman() {
    std::vector<GLfloat> veticieList;
    int stride = 5;
    for (int i = 0; i < 20; i++) {
        veticieList.push_back(vertices[i]);
    }
    return CreateObject(&veticieList[0], veticieList.size() * sizeof(veticieList[0]), stride);
}

void Pacman::drawPacman() {
    setVAO(compilePacman());
    GLuint ptexAttrib = glGetAttribLocation(shaderProgram, "pTexcoord");
    glEnableVertexAttribArray(ptexAttrib);
    glVertexAttribPointer(ptexAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    auto playerTextureLocation = glGetUniformLocation(shaderProgram, "u_PlayerTexture");

    glUseProgram(shaderProgram);

    glBindVertexArray(characterVAO);
    glUniform1i(playerTextureLocation, 0);
    transformPacman();
    CamHolder->applycamera(shaderProgram, WidthHeight.first, WidthHeight.second);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);
    CleanVAO(characterVAO);
}

/**
*  Performs shader transformation for Pacman
*
*  @param shaderprogram - pacmans shaderprogram
*  @param lerpProg      - current pacman lerpprog
*  @param lerpStart     - pacman lerpstartXY
*  @param lerpStop      - pacman lerpstopXY
*/
void Pacman::transformPacman() {
    float newX = (((1 - lerpProg) * lerpStart[0]) + (lerpProg * lerpStop[0]));
    float newY = (((1 - lerpProg) * lerpStart[1]) + (lerpProg * lerpStop[1]));

    //LERP performed in the shader for the pacman object
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(newX, newY, 0.01f));
    moveCamera(newX - 1, newY);
    GLuint transformationmat = glGetUniformLocation(shaderProgram, "u_TransformationMat");

    glUniformMatrix4fv(transformationmat, 1, false, glm::value_ptr(translation));
}

/**
 *  Move Camera
 *
 *  @param itObj - which type of object to iterate, here either Pacman or ghost
 *
 *  @see Character::getVertCoord(int index);
 *  @see GLuint CreateObject(GLfloat* object, int size, const int stride);
 *
 *  @return returns VAO gotten from CreateObject func
 */
void Pacman::moveCamera(float x, float y) {
    glm::vec3 oldCam = CamHolder->getCamPos();
    oldCam[0] = (x + (XYshift.first  / 2));
    oldCam[1] = (y + (XYshift.second / 2));
    CamHolder->setCamPos(oldCam);
}

/**
 *  Move Camera
 *
 *  @param itObj - which type of object to iterate, here either Pacman or ghost
 *
 *  @see Character::getVertCoord(int index);
 *  @see GLuint CreateObject(GLfloat* object, int size, const int stride);
 *
 *  @return returns VAO gotten from CreateObject func
 */
void Pacman::checkForKeyUpdate() {
    setCard(CamHolder->getCard());

    int newPotDir = CamHolder->getNewDesDir();
    if (newPotDir != -10){
        if (getLegalDir(newPotDir)) {
            updateDir(newPotDir);
            changeDir();
        };
    }
}