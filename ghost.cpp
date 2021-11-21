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
 *  @param    widthheight   - width and height of map
 *  @param    xyshift       - size of one square
 *  @param    campoint      - pointer to camrea object
 *  @see      Character::characterInit();
 *  @see      Ghost::ghostGetRandomDir();
 *  @see      Ghost::characterInit();
 */
Ghost::Ghost(int x, int y, bool ai, std::pair<int, int> widthheight, std::pair<float, float> xyshift, Camera* campoint) {
    CamHolder = campoint;
    XYpos[0] = x, XYpos[1] = y;
    XYshift = xyshift;
    AI = ai;
    WidthHeight = widthheight;
    dir = ghostGetRandomDir();
    prevDir = dir;
    AIdelay = dir;
    Character::characterInit();
    ghostAnimate();
};

/**
 *  Handles direction change requests
 *
 *  @see      Ghost:: ghostGetRandomDir();
 *  @see      Ghost:: getLegalDir(int dir);
 *  @see      Character:: getLerpCoords();
 */
void Ghost::changeDir() {
    bool legal = false;
    if (AIdelay == 0) { dir = ghostGetRandomDir(); AIdelay = ((rand() + 4) % 10); legal = true; }
    else {
        AIdelay--;
        legal = getLegalDir(dir);
    };

    if (legal && (lerpProg <= 0 || lerpProg >= 1)) {
        lerpStart[0] = lerpStop[0];
        lerpStart[1] = lerpStop[1];
        Character::getLerpCoords();
        lerpProg = lerpStep / 2.0f;
        prevDir = dir;
    }
};

/**
 *  Handles ghost LERP updates
 *
 *  @see      Character:: changeDir();
 *  @see      Character:: ghostUpdateVertice();
 */
void Ghost::updateLerp() {
    if (lerpProg >= 1.0f || lerpProg <= 0.0f) { changeDir(); }
    else { lerpProg += lerpStep; }
    ghostUpdateVertice();
}

/**
 *  compiles modelShader for ghost
 *
 *  @see CompileShader( const std::string& vertexShaderSrc,
                        const std::string& fragmentShaderSrc)
 */
void Ghost::compileGhostModelShader() {
    shaderProgram = CompileShader(  VertexShaderSrc,
                                    directionalLightFragmentShaderSrc);

    GLuint modtexAttrib = glGetAttribLocation(shaderProgram, "modTexcoord");
    glEnableVertexAttribArray(modtexAttrib);
    glVertexAttribPointer(modtexAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

}

/**
 *  Checks if pacman is crashing with ghost
 *
 *  @see      Camera:: getCoordsWithInt();
 *  @return   returns wheter or not pacman and ghost are crashing
 */
bool Ghost::checkGhostCollision(float pacX, float pacY, std::pair<float,float> xyshift) {

    float pacXpos = CamHolder->getCoordsWithInt(pacY, pacX, 0, 0.0f, xyshift),
          pacYpos = CamHolder->getCoordsWithInt(pacY, pacX, 1, 0.0f, xyshift);

    std::pair<float,float> pacCoords   = {  (pacXpos + (xyshift.first / 2.0f)),
                                            (pacYpos + (xyshift.second /2.0f))};
    std::pair<float,float> ghostCoords = {  (vertices[0] + (xyshift.first  / 2.0f)),
                                            (vertices[1] + (xyshift.second / 2.0f))};
    std::pair<float,float> math        = {  (pacCoords.first - ghostCoords.first),
                                            (pacCoords.second - ghostCoords.second)};
    float length = sqrt((math.first * math.first) + (math.second * math.second));
    if (length < 0) { length *= -1; }
    if (length < (xyshift.first + xyshift.second) / 5.0f) { return true; }
    else { false; }
    return false;
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
 *  @param currentTime - shaderprogram to use for drawing
 *  @param shadProg - shaderprogram to use for drawing
 *  @param WH - Width and height of map
 *  @param vao - vao used to draw object
 *  @param size - size of model
 *
 *  @see Camera::applycamera(const GLuint shader, const float width, const float height)
 *  @see CleanVAO(GLuint& vao)
 *  @see Ghost::transformGhost(GLuint shaderProg, float currentTime)
 *  @see Ghost::Light(
    const GLuint modelShaderprogram,
    const glm::vec3 pos,
    const glm::vec3 color,
    const glm::mat4 light_Projection,
    const glm::vec3 look_at,
    const glm::vec3 up_vec,
    const float spec
)
 */
void Ghost::drawGhostsAsModels(float currentTime, std::pair<int,int> WH) {

    glUseProgram(shaderProgram);

    auto vertexColorLocation = glGetUniformLocation(shaderProgram, "u_Color");
    glUniform4f(vertexColorLocation, 0.8f, 0.2f, 0.2f, 1.0f);

    auto modelTextureLocation = glGetUniformLocation(shaderProgram, "u_modelTexture");
    glUseProgram(shaderProgram);
    glUniform1i(modelTextureLocation, 1);

    CamHolder->applycamera(shaderProgram, WH.second, WH.first);
    transformGhost(shaderProgram, currentTime);
    Light(shaderProgram);

    glBindVertexArray(characterVAO);
    glDrawArrays(GL_TRIANGLES, 6, modelSize);
}

/**
 *  Loads 3d model
 *
 *  @see  LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
             std::vector<material_t> *materials, std::string *warn,
             std::string *err, const char *filename, const char *mtl_basedir,
             bool triangulate, bool default_vcols_fallback)
 *  @return returns VAO and size of model in a pair
 */
std::pair<GLuint, int> Ghost::LoadModel(const std::string path, const std::string objID) {
    //We create a vector of Vertex structs. OpenGL can understand these, and so will accept them as input.
    std::vector<Vertex> vertices;

    //Some variables that we are going to use to store data from tinyObj
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials; //This one goes unused for now, seeing as we don't need materials for this model.

    //Some variables incase there is something wrong with our obj file
    std::string warn;
    std::string err;

    //We use tinobj to load our models. Feel free to find other .obj files and see if you can load them.
    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, ("../../../../" + path + objID).c_str(), path.c_str());

    //For each shape defined in the obj file
    for (auto shape : shapes)
    {
        //We find each mesh
        for (auto meshIndex : shape.mesh.indices)
        {
            //And store the data for each vertice, including normals
            glm::vec3 vertice = {
                attrib.vertices[meshIndex.vertex_index * 3],
                attrib.vertices[(meshIndex.vertex_index * 3) + 1],
                attrib.vertices[(meshIndex.vertex_index * 3) + 2]
            };
            glm::vec3 normal = {
                attrib.normals[meshIndex.normal_index * 3],
                attrib.normals[(meshIndex.normal_index * 3) + 1],
                attrib.normals[(meshIndex.normal_index * 3) + 2]
            };
            glm::vec2 textureCoordinate = {                         //These go unnused, but if you want textures, you will need them.
                attrib.texcoords[meshIndex.texcoord_index * 2],
                attrib.texcoords[(meshIndex.texcoord_index * 2) + 1]
            };

            vertices.push_back({ vertice, normal, textureCoordinate }); //We add our new vertice struct to our vector

        }
    }

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //As you can see, OpenGL will accept a vector of structs as a valid input here
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

    //This will be needed later to specify how much we need to draw. Look at the main loop to find this variable again.
    std::pair<GLuint, int> VaoSize = { VAO, vertices.size() };
    return VaoSize;
}

/**
 *  Transforms ghost
 */
void Ghost::transformGhost(GLuint shaderProg, float currentTime){
    //LERP performed in the shader for the pacman object
    float height = sin(currentTime) / 100.0f;
    if (height < 0) { height *= -1; }
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(vertices[0]+(XYshift.first/2.0f), vertices[1] + (XYshift.second / 2.0f), height));
    //Rotate the object            base matrix      degrees to rotate   axis to rotate around
    glm::mat4 rotate = glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    float turn = 0.0f;

    switch (dir) {
    case 2: turn = 90.0f; break;
    case 4: turn = 270.0f; break;
    case 9: turn = 0.0f; break;
    case 3: turn = 180.0f; break;
    }

    glm::mat4 twist = glm::rotate(glm::mat4(1), glm::radians(turn), glm::vec3(0.0f, 1.0f, 0.0f));

    //Scale the object             base matrix      vector containing how much to scale along each axis (here the same for all axis)
    glm::mat4 scaling = glm::scale(glm::mat4(1), glm::vec3(0.03f, 0.02f, 0.03f));

    glm::mat4 transformation = translation * rotate * scaling * twist;

    GLuint transformationmat = glGetUniformLocation(shaderProg, "u_TransformationMat");

    if (transformationmat != -1)

    glUniformMatrix4fv(transformationmat, 1, false, glm::value_ptr(transformation));
}

/**
 *  loads Ghost sprite, just used to add soem texture to the ghosts
 *
 *  @see GLuint load_opengl_texture(const std::string& filepath, GLuint slot)
 */
void Ghost::loadGhostSpriteSheet() {
   textureSheet = load_opengl_texture("assets/ghostModelShader.png", 1);
}

// -----------------------------------------------------------------------------
// Code handling the Lighting
// -----------------------------------------------------------------------------
void Ghost::Light(
    const GLuint modelShaderprogram,
    const glm::vec3 pos,
    const glm::vec3 color,
    const glm::mat4 light_Projection,
    const glm::vec3 look_at,
    const glm::vec3 up_vec,
    const float spec
)
{

    //Get uniforms for our Light-variables.
    GLuint lightPos = glGetUniformLocation(modelShaderprogram, "u_LightPosition");
    GLuint lightColor = glGetUniformLocation(modelShaderprogram, "u_LightColor");
    GLuint lightDir = glGetUniformLocation(modelShaderprogram, "u_LightDirection");
    GLuint specularity = glGetUniformLocation(modelShaderprogram, "u_Specularity");
    GLuint lightSpace = glGetUniformLocation(modelShaderprogram, "u_LightSpaceMat");

    //Make some computations that would be cumbersome to inline
    //Here we figure out the combination of the projection and viewmatrixes for the lightsource
    glm::mat4 lightLookat = glm::lookAt(pos, look_at, glm::vec3(0.0f, 0.f, -1.f));
    glm::mat4 lightspacematrix = light_Projection * lightLookat;

    //Send Variables to our shader
    if (lightPos != -1)
        glUniform3f(lightPos, pos.x, pos.y, pos.z);             //Position of a point in space. For Point lights.
    if (lightDir != -1)
        glUniform3f(lightDir, 0 - pos.x, 0 - pos.y, 0 - pos.z); //Direction vector. For Directional Lights.
    if (lightColor != -1)
        glUniform3f(lightColor, color.r, color.g, color.b);     //RGB values
    if (specularity != -1)
        glUniform1f(specularity, spec);                         //How much specular reflection we have for our object

    //Values for Shadow computation
    if (lightSpace != -1)
        glUniformMatrix4fv(lightSpace, 1, false, glm::value_ptr(lightspacematrix));
}
