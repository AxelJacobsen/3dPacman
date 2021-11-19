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
 *  @see      Character:: getRandomAIdir();
 *  @see      Character:: getLegalDir(int dir);
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
 *  Handles LERP updates
 *
 *  @see      Character:: changeDir();
 *  @see      Character:: checkPellet();
 *  @see      Character:: AIupdateVertice();
 */
void Ghost::updateLerp() {
    if (lerpProg >= 1.0f || lerpProg <= 0.0f) { changeDir(); }
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
    textureSheet = load_opengl_texture("assets/pacman.png", 1);
}

/**
 *  Handles LERP updates
 *
 *  @see      Character:: changeDir();
 *  @see      Character:: checkPellet();
 *  @see      Character:: AIupdateVertice();
 */
void Ghost::compileGhostShadowShader() {
    std::tie(shadowmapFrameBuffer, depthMap) = createShadowmap(SHADOW_WIDTH, SHADOW_HEIGHT);
    //modelShadowShader = CompileShader(  VertexShaderSrc,
    //                                    shadowFragmentShaderSrc);
}

/**
 *  Handles LERP updates
 *
 *  @see      Character:: changeDir();
 *  @see      Character:: checkPellet();
 *  @see      Character:: AIupdateVertice();
 */
void Ghost::compileGhostModelShader() {
    shaderProgram = CompileShader(  VertexShaderSrc,
                                    directionalLightFragmentShaderSrc);
}

/**
 *  Returns map value
 *
 *  @return returns if coord is wall or not
 */
void Ghost::deleteGhostSpriteSheet() {
    glDeleteTextures(1, &textureSheet);
}


/**
 *  Checks if pacman is crashing with ghost
 *
 *  @see      Character:: AIgetXY();
 *  @see      Character:: AIgetLerpPog();
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
    glBindVertexArray(characterVAO);
    glUniform1i(ghostTextureLocation, 1);

    CamHolder->applycamera(shaderProgram, WidthHeight.second, WidthHeight.first);

    for (int g = 0; g < (ghostCount * 24); g += 24) {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)g);
    }
}

/**
 *  Draws Ghost
 *
 *  @param shader - shaderprogram to use for drawing
 *
 *  @see compileVertices(std::vector<Character*> itObj)
 *  @see CleanVAO(GLuint& vao)
 */
void Ghost::drawGhostsAsModels(float currentTime, const GLuint shadProg, std::pair<int,int> WH, const GLuint vao, const int size) {

    glUseProgram(shadProg);

    auto vertexColorLocation = glGetUniformLocation(shadProg, "u_Color");
    glUniform4f(vertexColorLocation, 0.8f, 0.2f, 0.2f, 1.0f);

    CamHolder->applycamera(shadProg, WH.second, WH.first);
    transformGhost(shadProg, currentTime);
    Light(shadProg);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 6, size);
}

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

// -----------------------------------------------------------------------------
// Draw the shadowmap
// -----------------------------------------------------------------------------
void Ghost::ShadowMapping(GLuint vertexarray, GLuint modelShaderprogram, GLuint depthmapfb, int size, GLuint SHADOW_WIDTH, GLuint SHADOW_HEIGHT)
{
    //Set up the scene for the shadowmap
    glViewport(-1, -1, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthmapfb);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(modelShaderprogram);

    //Add Camera
    //CamHolder->applycamera(shaderProgram, WidthHeight.second, WidthHeight.first);

    //Compute the transformation for the scene for the current frame as normal
    Transform(modelShaderprogram);

    //Compute the Light
    Light(modelShaderprogram);

    //Draw a square underneath the Cup for shadowing purposes
    GLuint squareVao = CreateSquare();
    glBindVertexArray(squareVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);

    //Draw the cup
    glBindVertexArray(vertexarray);
    glDrawArrays(GL_TRIANGLES, 6, size);

    //Reset all changed values back to normal once we are done here
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(-1.0f, -1.0f, SHADOW_WIDTH, SHADOW_HEIGHT);

    //Make sure to clean up after yourself, lest you face memoryleaks
    CleanVAO(squareVao);
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

// -----------------------------------------------------------------------------
// Set up the shadowmap
// -----------------------------------------------------------------------------
std::tuple<GLuint, GLuint> Ghost::createShadowmap(const GLuint SHADOW_WIDTH, const GLuint SHADOW_HEIGHT)
{
    //Create a new framebuffer
    GLuint FrameBuffer;
    glGenFramebuffers(1, &FrameBuffer);

    //Make a texture containing depth data
    GLuint depthmap;
    glGenTextures(1, &depthmap);
    glBindTexture(GL_TEXTURE_2D, depthmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


    //This helps remove issues with areas that are not supposed to be shadowed being rendered as dark if outside of the maps area(SHADOW_WIDHT,SHADOW_HEIGHT).
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //Bind the depthmap to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmap, 0);

    //We aren't going to use color drawing in the framebuffer, so we can deactivate these
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    //Make sure to return to the regular Framebuffer once you are done here to make sure you don't accidentally draw to the wrong buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Utilizing the power of C++ 11+, we can send more than one return value, so long as we specify that the function returns a std::tuple,
    //as well as what the value types contained are.
    return { FrameBuffer,depthmap };
}

// -----------------------------------------------------------------------------
//  Create a quad.
// -----------------------------------------------------------------------------
//This has been updated in order to help draw a plane beneath the cup so we can have something to cast shadows onto
GLuint Ghost::CreateSquare()
{

    GLfloat square[4 * 8] =
    {
        //x     y     z          normals
        -1.0f, -1.0f, -0.1f,   0.0f,1.0f,0.0f,
        -1.0f,  1.0f, -0.1f,   0.0f,1.0f,0.0f,
         1.0f,  1.0f, -0.1f,   0.0f,1.0f,0.0f,
         1.0f, -1.0f, -0.1f,   0.0f,1.0f,0.0f,
    };

    GLuint square_indices[6] = { 0,1,2,0,2,3 };

    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (const void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(float) * 3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

    return vao;
}

// -----------------------------------------------------------------------------
// Code handling Transformations
// -----------------------------------------------------------------------------
void Ghost::Transform(
    const GLuint shaderprogram,
    const glm::vec3& translation,
    const float& radians,
    const glm::vec3& rotation_axis,
    const glm::vec3& scale
)
{

    //Presentation below purely for ease of viewing individual components of calculation, and not at all necessary.

    //LERP performed in the shader for the pacman object
    glm::mat4 transMat = glm::translate(glm::mat4(1), glm::vec3(vertices[0] + (XYshift.first / 2.0f), vertices[1] + (XYshift.second / 2.0f), 0.0f));

    //Rotate the object            base matrix      degrees to rotate   axis to rotate around
    glm::mat4 rotate = glm::rotate(glm::mat4(1), glm::radians(1.5708f), glm::vec3(1.0f, 0.0f, 0.0f));

    //Scale the object             base matrix      vector containing how much to scale along each axis (here the same for all axis)
    glm::mat4 scaling = glm::scale(glm::mat4(1), glm::vec3(0.5f, 0.5f, 0.5f));

    //Create transformation matrix      These must be multiplied in this order, or the results will be incorrect
    glm::mat4 transformation = transMat * rotate * scaling;


    //Get uniform to place transformation matrix in
    //Must be called after calling glUseProgram     shader program in use   Name of Uniform
    GLuint transformationmat = glGetUniformLocation(shaderprogram, "u_TransformationMat");

    //Send data from matrices to uniform
    //We also add a check to make sure that we found the location of the matrix before trying to write to it
    if (transformationmat != -1)
        //                     Location of uniform  How many matrices we are sending    value_ptr to our transformation matrix
        glUniformMatrix4fv(transformationmat, 1, false, glm::value_ptr(transformation));
}