// -----------------------------------------------------------------------------
// Pellet Class
// -----------------------------------------------------------------------------

#include "pellet.h"
#include "globFunc.h"

/**
 *  Initializes pellet with x and y
 *
 *  @param    x - pellets designated x
 *  @param    y - pellets designated y
 *  @see      Pellet::initCoords();
 */
Pellet::Pellet(int x, int y, std::pair<float, float> shift) {
    XYshift = shift;
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
    float Xquart = XYshift.first  / 3.0f;
    float Yquart = XYshift.second / 3.0f;
    //float height = XYshift.second*2.0f / 3.0f;
    //for (int side = 0; side < 6; side++){
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 3; x++) {
                vertices[loop] = (pCamHolder->getCoordsWithInt(XYpos[1], XYpos[0], loop, 0.0f, XYshift));
                switch (loop) {
                case 0:  vertices[loop] += Xquart; break;   //BL
                case 1:  vertices[loop] += Yquart; break;

                case 3:  vertices[loop] += Xquart; break;   //TL
                case 4:  vertices[loop] -= Yquart; break;

                case 6:  vertices[loop] -= Xquart; break;   //TR
                case 7:  vertices[loop] -= Yquart; break;

                case 9:  vertices[loop] -= Xquart; break;   //BR
                case 10: vertices[loop] += Yquart; break;
                default: vertices[loop] = 0.0f;           break;
                }
                loop++;
            }
        }
   // }
}

/**
 *  sets Pellet vertices to 0, effectively removing it
 */
bool Pellet::removePellet() {
    if (enabled) {
        for (auto& it : vertices) {
            it = 0.0f;
        }
        enabled = false;
        return true;
    }
    return false;
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

void Pellet::callCompilePelletShader() {
    pelletShaderProgram = CompileShader(    pelletVertexShaderSrc,
                                            pelletFragmentShaderSrc);
    
    GLint pelposAttrib = glGetAttribLocation(pelletShaderProgram, "pelPosition");
    glEnableVertexAttribArray(pelposAttrib);
    glVertexAttribPointer(pelposAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
}

/**
 *  Draws Pellets
 *
 *  @param shader - shaderprogram to use for drawing
 *  @param vao    - vao of object
 */
void Pellet::drawPellets(const int size) {
    auto pelletVertexColorLocation = glGetUniformLocation(pelletShaderProgram, "u_Color");
    glUseProgram(pelletShaderProgram);
    pCamHolder->applycamera(pelletShaderProgram, WidthHeight.second, WidthHeight.first );
    glBindVertexArray(pelletVAO);
    glUniform4f(pelletVertexColorLocation, 0.8f, 0.8f, 0.0f, 1.0f);
    glDrawElements(GL_TRIANGLES, int(6 * size), GL_UNSIGNED_INT, (const void*)0);
}

void Pellet::cleanPellets() {
    glDeleteProgram(pelletShaderProgram);
    CleanVAO(pelletVAO);
}

void Pellet::cleanPelletVAO() { 
    CleanVAO(pelletVAO); 
};

void Pellet::callCreatePelletVAO(GLfloat* object, int size, const int stride) {
    pelletVAO = CreateObject(object, size, stride);
};

void Pellet::setVAO(const GLuint vao) {
    pelletVAO = vao;
}

GLuint Pellet::getVAO() {
    return pelletVAO;
}

GLuint Pellet::getShader() {
    return pelletShaderProgram;
}