// -----------------------------------------------------------------------------
// Class function definition
// -----------------------------------------------------------------------------

#include "ghost.h"

/**
 *  Initializes Ghosts
 *
 *  @param    x  - Initialization pos X
 *  @param    y  - Initializaiton pos Y
 *  @param    ai - upsates AI to define as Ghost
 *  @see      Character:: getRandomAIdir();
 *  @see      Character:: characterInit();
 */
Ghost::Ghost(int x, int y, bool ai) {
    XYpos[0] = x, XYpos[1] = y;
    AI = ai;
    printf("PreDir\n");
    dir = ghostGetRandomDir();
    dir = 9;
    printf("PostDir\n");
    prevDir = dir;
    AIdelay = dir;
    printf("PreCharInit\n");
    Character::characterInit();
    printf("PostCharInit\n");
    ghostAnimate();
    printf("PostAnimate\n");
};

/**
 *  Handles direction change requests
 *
 *  @see      Character:: getRandomAIdir();
 *  @see      Character:: getLegalDir(int dir);
 *  @see      Character:: getLerpCoords();
 */
void Ghost::changeDir() {
    if (AIdelay == 0) { dir = ghostGetRandomDir(); AIdelay = ((rand() + 4) % 10); }
    else { AIdelay--; };

    if (1.0f <= lerpProg) {  //redundancy due to this never getting called if its falselerpProg <= 0 || 
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
void Ghost::updateLerp() {
    if (lerpProg > 1 || lerpProg < 0) { changeDir(); }
    else { lerpProg += lerpStep; }
    ghostUpdateVertice();
}

/**
 *  Handles LERP updates
 *
 *  @see      Character:: changeDir();
 *  @see      Character:: checkPellet();
 *  @see      Character:: AIupdateVertice();
 */
void Ghost::compileGhostShader() {
    shaderProgram = CompileShader(  ghostVertexShaderSrc,
                                    ghostFragmentShaderSrc);
}

/**
 *  Checks if pacman is crashing with ghost
 *
 *  @see      Character:: AIgetXY();
 *  @see      Character:: AIgetLerpPog();
 *  @return   returns wheter or not pacman and ghost are crashing
 */
bool Ghost::checkGhostCollision(float pacX, float pacY) {
    std::pair<float,float> pacCoords   = { (pacX + (XYshift.first /2)),
                                            (pacY + (XYshift.second /2))};
    std::pair<float,float> ghostCoords = { (vertices[0] + (XYshift.first / 2)),
                                            (vertices[1] + (XYshift.second / 2)) };
    std::pair<float,float> math        = { (pacCoords.first - ghostCoords.first),
                                            (pacCoords.second - ghostCoords.second)};
    float length = sqrt((math.first * math.first) + (math.second * math.second));
    if (length < 0) { length *= -1; }
    if (length < ((XYshift.first + XYshift.second) / 3)) { return true; }
    else { false; }
}

/**
 *  Bruteforces a legal direction for AI
 *
 *  @see      Character::getLegalDir(int dir);
 *  @return   returns a legal direction for the AI to take
 */
int Ghost::ghostGetRandomDir() {
    int temp = 0;
    time_t t;
    srand((unsigned)time(&t));
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
void Ghost::ghostUpdateVertice() {
    for (int f = 0; f < (4 * 5); f += 5) {
        for (int k = f; k < (f + 3); k++) {
            if (k == f) {
                vertices[k] = (((1 - lerpProg) * lerpStart[0]) + (lerpProg * lerpStop[0]));
            }
            else if (k == (f + 1)) {
                vertices[k] = (((1 - lerpProg) * lerpStart[1]) + (lerpProg * lerpStop[1]));
            }
            switch (k) {
            case 0:   vertices[k];                  break;
            case 1:   vertices[k];                  break;

            case 5:   vertices[k];                  break;
            case 6:   vertices[k] += XYshift.second;break;

            case 10:  vertices[k] += XYshift.first; break;
            case 11:  vertices[k] += XYshift.second;break;

            case 15:  vertices[k] += XYshift.first; break;
            case 16:  vertices[k];                  break;
            default:  vertices[k] = 0.0f;           break;
            }
        }
    }
}

/**
 *  Returns the AIs lerpProg
 *
 *  @return   AI lerpProg
 */
float Ghost::ghostGetLerpPog() {
    return lerpProg;
}

/**
 *  Returns the AIs XY
 *
 *  @return   AI XY
 */
int Ghost::ghostGetXY(int xy) {
    return XYpos[xy];
}

/**
 *  Handles AI animation
 *
 *  @see  Character:: characterAnimate(float hMin, float wMin, float hMax, float wMax);
 */
void Ghost::ghostAnimate() {
    float wMod = (1.0f / 6.0f);
    float hMod = (1.0f / 4.0f);
    float mhMod = hMod, mwMod = wMod;
    if (!animFlip) {
        animFlip = true;
        wMod *= 5;
        mwMod = 1.0f;
    }
    else {
        animFlip = false;
        wMod *= 4;
        mwMod *= 5;
    }
    Character::characterAnimate(hMod, wMod, mhMod, mwMod);
};

/**
 *  Draws Ghost
 *
 *  @param shader - shaderprogram to use for drawing
 *
 *  @see compileVertices(std::vector<Character*> itObj)
 *  @see CleanVAO(GLuint& vao)
 */
void Ghost::drawGhosts(const int ghostCount) {
    GLuint gtexAttrib = glGetAttribLocation(shaderProgram, "gTexcoord");
    glEnableVertexAttribArray(gtexAttrib);
    glVertexAttribPointer(gtexAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    auto ghostTextureLocation = glGetUniformLocation(shaderProgram, "g_GhostTexture");
    glUseProgram(shaderProgram);
    CamHolder->applycamera(shaderProgram, (XYshift.first*3), (XYshift.second*3));

    glBindVertexArray(characterVAO);
    glUniform1i(ghostTextureLocation, 0);

    for (int g = 0; g < (ghostCount * 24); g += 24) {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)g);
    }
    CleanVAO(characterVAO);
}