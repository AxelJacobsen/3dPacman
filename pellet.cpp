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
    float Xquart = XYshift.first  / 2.0f;
    float Yquart = XYshift.second / 2.0f;
    for (int x = 0; x < 3; x++) {
        vertices[loop] = (pCamHolder->getCoordsWithInt(XYpos[1], XYpos[0], loop, 0.0f, XYshift));
        switch (loop) {
        case 0:  vertices[loop] += Xquart; break;
        case 1:  vertices[loop] += Yquart; break;
        default: vertices[loop] =  Yquart; break;
        }
        loop++;
    }
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

/**
 *  CAlls compileshader with pellets shaders and sets vertecie values
 *
 *  @see CompileShader(const std::string& vertexShaderSrc,
                     const std::string& fragmentShaderSrc,
                     const std::string& geometryShaderSrc)
 */
void Pellet::callCompilePelletShader() {
    pelletShaderProgram = CompileShader(pelletVertexShaderSrc,
            pelletFragmentShaderSrc, pelletGeometryShaderSrc);

    GLint pelposAttrib = glGetAttribLocation(pelletShaderProgram, "pelPosition");
    glEnableVertexAttribArray(pelposAttrib);
    glVertexAttribPointer(pelposAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0); 
}

/**
 *  Draws Pellets
 *
 *  @see Camera::applycamera(const GLuint shader, const float width, const float height)
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
    glDrawArrays(GL_POINTS, 0, int(6 * size));
}

/**
 *  Cleans all pellet values
 *
 *  @see CleanVAO(GLuint& vao)
 */
void Pellet::cleanPellets() {
    glDeleteProgram(pelletShaderProgram);
    CleanVAO(pelletVAO);
}

/**
 *  Cleans pellet VAO
 *
 *  @see CleanVAO(GLuint& vao)
 */
void Pellet::cleanPelletVAO() { 
    CleanVAO(pelletVAO); 
};

/**
 *   Calls createObject for pellet
 *
 *  @see CreateObject(GLfloat* object, int size, const int stride, bool noEbo)
 */
void Pellet::callCreatePelletVAO(GLfloat* object, int size, const int stride) {
    pelletVAO = CreateObject(object, size, stride, true);
};

/**
 *  Sets pelletVAO
 */
void Pellet::setVAO(const GLuint vao) {
    pelletVAO = vao;
}

/**
 *  returns pelletVAO
 */
GLuint Pellet::getVAO() {
    return pelletVAO;
}

/**
 *  returns pelletShader
 */
GLuint Pellet::getShader() {
    return pelletShaderProgram;
}