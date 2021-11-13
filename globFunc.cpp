#include "globFunc.h"

// ---------------------------------------------------------------------------- -
// COMPILE SHADER
// -----------------------------------------------------------------------------
GLuint CompileShader(const std::string & vertexShaderSrc,
    const std::string & fragmentShaderSrc)
{
    auto vertexSrc = vertexShaderSrc.c_str();
    auto fragmentSrc = fragmentShaderSrc.c_str();

    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);

    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);

    auto shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glBindFragDataLocation(shaderProgram, 0, "color");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint load_opengl_texture(const std::string& filepath, GLuint slot)
{
    /** Image width, height, bit depth */
    int w, h, bpp;
    auto pixels = stbi_load(filepath.c_str(), &w, &h, &bpp, STBI_rgb_alpha);

    /*Generate a texture object and upload the loaded image to it.*/
    GLuint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0 + slot); //Texture Unit
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    /** Set parameters for the texture */
    //Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /** Free memory */
    if (pixels)
        stbi_image_free(pixels);

    return tex;
};

// -----------------------------------------------------------------------------
// DRAW FUNCTIONS
// -----------------------------------------------------------------------------
/**
 *  Draws map
 *
 *  @param shader - shaderprogram to use for drawing
 *  @param vao    - vao of object
 */
void drawMap(const GLuint shader, const GLuint vao, const int mapSize) {
    int numElements = mapSize;
    auto mapTextureLocation = glGetUniformLocation(shader, "u_mapTexture");

    glUseProgram(shader);
    glUniform1i(mapTextureLocation, 2);
    applycamera(shader);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, (const void*)0);
}
/**
 *  Draws Pellets
 *
 *  @param shader - shaderprogram to use for drawing
 *  @param vao    - vao of object
 */
void drawPellets(const GLuint shader, const GLuint vao) {
    auto pelletVertexColorLocation = glGetUniformLocation(shader, "u_Color");
    glUseProgram(shader);
    applycamera(shader);
    glBindVertexArray(vao);
    glUniform4f(pelletVertexColorLocation, 0.8f, 0.8f, 0.0f, 1.0f);
    glDrawElements(GL_TRIANGLES, int(6 * Pellets.size()), GL_UNSIGNED_INT, (const void*)0);
}
/**
 *  Draws Ghost
 *
 *  @param shader - shaderprogram to use for drawing
 *
 *  @see compileVertices(std::vector<Character*> itObj)
 *  @see CleanVAO(GLuint& vao)
 */
void drawGhosts(const GLuint shader) {

    auto ghostVAO = compileVertices(Ghosts);
    GLuint gtexAttrib = glGetAttribLocation(shader, "gTexcoord");
    glEnableVertexAttribArray(gtexAttrib);
    glVertexAttribPointer(gtexAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    auto ghostTextureLocation = glGetUniformLocation(shader, "g_GhostTexture");
    glUseProgram(shader);
    applycamera(shader);
    glBindVertexArray(ghostVAO);
    glUniform1i(ghostTextureLocation, 1);

    for (int g = 0; g < (ghostCount * 24); g += 24) {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)g);
    }
    CleanVAO(ghostVAO);
}
/**
 *  Draws Ghost
 *
 *  @param shader - shaderprogram to use for drawing
 *
 *  @see compileVertices(std::vector<Character*> itObj)
 *  @see Character::Transform(const GLuint ShaderProgram);
 *  @see CleanVAO(GLuint& vao)
 */
void drawPacman(const GLuint shader) {
    auto playerVAO = compileVertices(Pacman);

    GLuint ptexAttrib = glGetAttribLocation(shader, "pTexcoord");
    glEnableVertexAttribArray(ptexAttrib);
    glVertexAttribPointer(ptexAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    auto playerTextureLocation = glGetUniformLocation(shader, "u_PlayerTexture");

    glUseProgram(shader);

    glBindVertexArray(playerVAO);
    glUniform1i(playerTextureLocation, 0);
    Pacman[0]->Transform(shader);
    applycamera(shader);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);
    CleanVAO(playerVAO);
}

/**
 *  Applies camera tranformation
 */
void applycamera(const GLuint shader) {

    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)(width * spriteSize / 3) / (float)(height * spriteSize / 3), 0.001f, 2.0f);
    GLuint projMat = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projMat, 1, false, glm::value_ptr(projection));

    // camera/view transformation
    glm::mat4 view = glm::lookAt(cameraPos, (cameraPos + cameraFront), cameraUp);
    GLuint viewMat = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(viewMat, 1, false, glm::value_ptr(view));
}

// -----------------------------------------------------------------------------
//  INITIALIZE OBJECT
// -----------------------------------------------------------------------------
/**
 *  Creates object
 *
 *  @param object    - pointer to object to be created
 *  @param size      - size of object
 *  @param stride    - stride used in object
 *
 *  @return     returns vao
 */
GLuint CreateObject(GLfloat* object, int size, const int stride)
{
    std::vector<GLuint> object_indices;

    for (int i = 0; i < size; i += 4) {
        for (int o = 0; o < 2; o++) {
            for (int p = i; p < (i + 3); p++) {
                object_indices.push_back(getIndices(i, o, p));
            }
        }
    };

    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        size,
        (&object[0]),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (sizeof(GLfloat) * stride), (const void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object_indices.size() * sizeof(object_indices)[0], (&object_indices[0]), GL_STATIC_DRAW);

    return vao;
};


// -----------------------------------------------------------------------------
//  CREATE MAP
// -----------------------------------------------------------------------------
/**
 *  Creates map
 *
 *  @param *map       - pointer to map vector
 *  @param *mapObje   - pointer to first item in map vector, easier to work with this
 *
 *  @see GLuint getIndices(int out, int mid, int in)
 *
 *  @return returns vao of map
 */
GLuint CreateMap(std::vector<GLfloat>* map, GLfloat* mapObj) {
    std::vector<GLuint> mapIndices;
    for (int o = 0; o < ((*map).size() / 3); o += 4) {
        for (int m = 0; m < 2; m++) {
            for (int i = o; i < (o + 3); i++) {
                int hold = getIndices(o, m, i);
                if ((i - o) == 2 && m != 1) { hold++; }
                mapIndices.push_back(hold);
            }
        }
    };

    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        ((*map).size() * sizeof((*map)[0])),
        (&mapObj)[0],
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (const void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mapIndices.size() * sizeof(mapIndices[0]), (&mapIndices[0]), GL_STATIC_DRAW);

    return vao;
}
// -----------------------------------------------------------------------------
// SHADER TRANSFORMATIONS
// -----------------------------------------------------------------------------
/**
 *  Performs shader transformation for Pacman
 *
 *  @param shaderprogram - pacmans shaderprogram
 *  @param lerpProg      - current pacman lerpprog
 *  @param lerpStart     - pacman lerpstartXY
 *  @param lerpStop      - pacman lerpstopXY
 */
void TransformPlayer(const GLuint shaderprogram, float lerpProg, float lerpStart[], float lerpStop[])
{

    float newX = (((1 - lerpProg) * lerpStart[0]) + (lerpProg * lerpStop[0]));
    float newY = (((1 - lerpProg) * lerpStart[1]) + (lerpProg * lerpStop[1]));

    //LERP performed in the shader for the pacman object
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(newX, newY, 0.2f));
    moveCamera(newX - 1, newY);
    GLuint transformationmat = glGetUniformLocation(shaderprogram, "u_TransformationMat");

    glUniformMatrix4fv(transformationmat, 1, false, glm::value_ptr(translation));
}

// -----------------------------------------------------------------------------
// GLOBAL FUNCTIONS
// -----------------------------------------------------------------------------

/**
 *  Creates map coordinates and initializes all objects
 *
 *  @param *map       - pointer to map vector
 *  @param *mapObje   - pointer to first item in map vector, easier to work with this
 *
 *  @see GLfloat getCoordsWithInt(int y, int x, int loop);
 *  @see Pellet::checkCoords(int XY);
 */
void callMapCoordinateCreation(std::vector<std::vector<int>> levelVect, std::vector<float>* map) {
    int hallCount = 0;
    for (int i = 0; i < height; i++) {    // creates map
        for (int j = 0; j < width; j++) {
            if (levelVect[i][j] == 1) {
                int loop = 0;
                int wallType = findWhatWalls(levelVect, j, i);
                std::vector<int> loopO = loopOrder(wallType);
                int counter = 0;
                for (int l = 0; l < howManyWalls(wallType); l++) {
                    float height = 0.0f;
                    for (int corners = 0; corners < 4; corners++) {
                        loop = loopO[counter];
                        counter++;
                        if (2 == corners) { height = 0.15f; };
                        for (int point = 0; point < 3; point++) {
                            (*map).push_back(getCoordsWithInt(i, j, loop, height));
                            loop++;
                        }
                        std::pair<int, int> pholder = handleMapTextCoord(corners);
                        (*map).push_back(pholder.first);
                        (*map).push_back(pholder.second);
                    }
                }
            }
            else if (levelVect[i][j] == 2) {
                Pacman.push_back(new Character(j, i));
                Pacman[0]->recieveMapInt(levelVect);
            }
            else { hallCount++;  Pellets.push_back(new Pellet(j, i)); }
        }
    }
    spawnGhost(hallCount);
}

/**
 *  returns coordinates based on give x y and "loop"
 *
 *  @param y    - y coordinate for coords
 *  @param x    - x coordinate for coords
 *  @param loop - current repitition / desired vertice
 *
 *  @return returns float for correct coord
 */
GLfloat getCoordsWithInt(int y, int x, int loop, float layer) {
    Xshift = 2.0f / (float(width));
    Yshift = 2.0f / (float(height));
    GLfloat tempXs, tempYs;
    if (x == 0 && y == 0) { tempXs = 0, tempYs = 0; }
    else { tempXs = (Xshift * x), tempYs = (Yshift * y); }

    switch (loop) {
    case 0:   tempXs;             return (tempXs - 1.0f);  // Bot Left
    case 1:   tempYs;             return (tempYs - 1.0f);  // Bot Left

    case 3:   tempXs;             return (tempXs - 1.0f);  // Top Left
    case 4:   tempYs += Yshift;   return (tempYs - 1.0f);  // Top Left

    case 6:   tempXs += Xshift;   return (tempXs - 1.0f);  // Top Right
    case 7:   tempYs += Yshift;   return (tempYs - 1.0f);  // Top Right

    case 9:   tempXs += Xshift;   return (tempXs - 1.0f);  // Bot Right
    case 10:  tempYs;             return (tempYs - 1.0f);  // Bot Right
    default: return layer;
    }
};

/**
 *  Returns indice
 *
 *  @param out  -   outer  for loop value "o"
 *  @param mid  -   middle for loop value "m"
 *  @param in   -   inner for loop value "i"
 */
GLuint getIndices(int out, int mid, int in) {
    if (in == out) { return out; }
    else { return (mid + in); };
}

/**
 *  Reads inn map from file
 *
 *  @return returns map vector
 */
std::vector<std::vector<int>> loadFromFile() {
    // Read from level file;
    std::ifstream inn("../../../../levels/level0");
    if (inn) {
        inn >> width; inn.ignore(1); inn >> height;
        std::vector<std::vector<int>> tempMapVect(height, std::vector<int>(width, 0));
        int row = 0, column = 0;
        int temp;
        inn >> temp;
        while (column < height) {
            int Yvalue = (height - 1 - column);
            if (row < width) {
                tempMapVect[Yvalue][row] = temp;
                row++;
                inn >> temp;
            }
            else { row = 0; column++; }
        }
        inn.close();
        return tempMapVect;
    }
    else { printf("\n\nERROR: Couldnt find level file, check that it is in the right place.\n\n"); exit(EXIT_FAILURE); }
}

/**
 *  Compiles all Pellet verticie lists into a large vector and calls CreateObject
 *
 *  @param itObj - which type of object to iterate, here always pellet
 *
 *  @see Pellet::getVertCoord(int index);
 *  @see GLuint CreateObject(GLfloat* object, int size, const int stride);
 *
 *  @return returns VAO gotten from CreateObject func
 */
GLuint compileVertices(std::vector<Pellet*> itObj) {
    std::vector<GLfloat> veticieList;
    int stride = 3;
    for (auto& it : itObj) {
        if (it->isEnabled()) {
            for (int i = 0; i < 12; i++) {
                veticieList.push_back(it->getVertCoord(i));
            }
        }
    }
    return CreateObject(&veticieList[0], veticieList.size() * sizeof(veticieList[0]), stride);
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
GLuint compileVertices(std::vector<Character*> itObj) {
    std::vector<GLfloat> veticieList;
    int stride = 5;
    for (auto& it : itObj) {
        for (int i = 0; i < 20; i++) {
            veticieList.push_back(it->getVertCoord(i));
        }
    }
    return CreateObject(&veticieList[0], veticieList.size() * sizeof(veticieList[0]), stride);
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
void moveCamera(float x, float y) {
    cameraPos[0] = (x + (Xshift / 2));
    cameraPos[1] = (y + (Yshift / 2));
}

/**
 *  Finds what wall tile draws a wall
 *
 *  @param itObj - which type of object to iterate, here either Pacman or ghost
 *
 *  @see Character::getVertCoord(int index);
 *  @see GLuint CreateObject(GLfloat* object, int size, const int stride);
 *
 *  @return returns VAO gotten from CreateObject func
 */
int findWhatWalls(std::vector<std::vector<int>> levelVect, const int x, const int y) {
    int wallType = 0;
    for (int i = 0; i < 4; i++) {
        switch (i) {
        case 0: if ((y + 1) < height) { if ((levelVect[(y + 1)][x] == 0) || levelVect[(y + 1)][x] == 2) { wallType += 1; } } break;
        case 1: if (0 < (x - 1)) { if ((levelVect[y][(x - 1)] == 0) || levelVect[y][(x - 1)] == 2) { wallType += 2; } } break;
        case 2: if ((x + 1) < width) { if ((levelVect[y][(x + 1)] == 0) || levelVect[y][(x + 1)] == 2) { wallType += 4; } } break;
        case 3: if (0 < (y - 1)) { if ((levelVect[(y - 1)][x] == 0) || levelVect[(y - 1)][x] == 2) { wallType += 8; } } break;
        }
    }
    return wallType;
}

/**
 *  Finds what wall tile draws a wall
 *
 *  @param itObj - which type of object to iterate, here either Pacman or ghost
 *
 *  @see Character::getVertCoord(int index);
 *  @see GLuint CreateObject(GLfloat* object, int size, const int stride);
 *
 *  @return returns how many walls are to be drawn
 */
int howManyWalls(int num) {
    switch (num) {
        // 1 wall
    case 1: case 2: case 4: case 8:                     return 1; break;
        // 2 walls
    case 3: case 5: case 6: case 9: case 10: case 12:   return 2; break;
        // 3 walls (dont think there is one but its for redundance)
    case 7: case 11: case 13: case 14:                  return 3; break;
        // 4 walls (doesnt exist and would be wierd)
    case 15:                                            return 4; break;
    }
    return -1;
}

/**
 *  Finds what wall tile draws a wall
 *
 *  @param itObj - which type of object to iterate, here either Pacman or ghost
 *
 *  @see Character::getVertCoord(int index);
 *  @see GLuint CreateObject(GLfloat* object, int size, const int stride);
 *
 *  @return returns how many walls are to be drawn
 */
std::vector<int> loopOrder(int num) {
    std::pair<int, int> up = { 3, 6 };
    std::pair<int, int> left = { 0, 3 };
    std::pair<int, int> right = { 0, 9 }; //
    std::pair<int, int> down = { 6, 9 }; //
    std::vector<int> loopy;
    int rep = 0;
    int push[8] = { 0 };
    switch (num) {
        // 1 wall
    case  1: rep = 1; push[0] = up.first;   push[1] = up.second;    break;
    case  2: rep = 1; push[0] = left.first; push[1] = left.second;  break;
    case  4: rep = 1; push[0] = down.first; push[1] = down.second;  break;
    case  8: rep = 1; push[0] = right.first; push[1] = right.second; break;
        // 2 walls
    case  3: rep = 2; push[0] = up.first;   push[1] = up.second;   push[2] = left.first;  push[3] = left.second;  break;
    case  5: rep = 2; push[0] = up.first;   push[1] = up.second;   push[2] = down.first;  push[3] = down.second;  break;
    case  6: rep = 2; push[0] = left.first; push[1] = left.second; push[2] = down.first;  push[3] = down.second;  break;
    case  9: rep = 2; push[0] = up.first;   push[1] = up.second;   push[2] = right.first; push[3] = right.second; break;
    case 10: rep = 2; push[0] = left.first; push[1] = left.second; push[2] = right.first; push[3] = right.second; break;
    case 12: rep = 2; push[0] = down.first; push[1] = down.second; push[2] = right.first; push[3] = right.second; break;
        // 3 walls (dont think there is one but its for redundance)
    case  7: rep = 3; push[0] = up.first;   push[1] = up.second;   push[2] = left.first;  push[3] = left.second;  push[4] = down.first;  push[5] = down.second;  break;
    case 11: rep = 3; push[0] = up.first;   push[1] = up.second;   push[2] = left.first;  push[3] = left.second;  push[4] = right.first; push[5] = right.second; break;
    case 13: rep = 3; push[0] = up.first;   push[1] = up.second;   push[2] = right.first; push[3] = right.second; push[4] = down.first;  push[5] = down.second;  break;
    case 14: rep = 3; push[0] = left.first; push[1] = left.second; push[2] = down.first;  push[3] = down.second;  push[4] = right.first; push[5] = right.second; break;
        // 4 walls (doesnt exist and would be wierd)
    case 15: rep = 4; push[0] = up.first;   push[1] = up.second;
        push[2] = left.first; push[3] = left.second;
        push[4] = down.first; push[5] = down.second;
        push[6] = right.first; push[7] = right.second; break;
    default: break;
    }

    for (int i = 0; i < rep * 2; i += 2) {
        for (int o = 0; o < 2; o++) {
            loopy.push_back(push[i]);
            loopy.push_back(push[i + 1]);
        }
    }

    return loopy;
}

/**
 *  Handles Ghost spawning
 *
 *  @param y    - y coordinate for coords
 *  @param x    - x coordinate for coords
 *  @param loop - current repitition / desired vertice
 *
 *  @return returns float for correct coord
 */
void spawnGhost(const int hallCount) {
    time_t t;
    srand((unsigned)time(&t));
    std::vector<int> formerPositions;
    bool noDouble = false;
    do {
        for (int g = 0; g < ghostCount; g++) {
            int randPos;

            randPos = (rand() % hallCount);
            formerPositions.push_back(randPos);
        }
        noDouble = true;
        for (int n = 0; n < ghostCount; n++) {
            for (int m = (n + 1); m < (ghostCount); m++) {
                if (formerPositions[n] == formerPositions[m]) {
                    noDouble = false;
                }
            }
        }
    } while (!noDouble);
    int count = 0;
    for (auto& it : Pellets) {
        for (int l = 0; l < ghostCount; l++) {
            if (count == formerPositions[l]) {
                Ghosts.push_back(new Character(it->checkCoords(0), it->checkCoords(1), true));
            }
        }
        count++;
    }
};

/**
 *  Handles Ghost spawning
 *
 *  @param y    - y coordinate for coords
 *  @param x    - x coordinate for coords
 *  @param loop - current repitition / desired vertice
 *
 *  @return returns float for correct coord
 */
std::pair<int, int> handleMapTextCoord(const int rep) {
    std::pair <int, int> temp = { 0,0 };
    switch (rep) {
    case 3:                    return temp; break;
    case 2:  temp.first = 1;  return temp; break;
    case 1:  temp.second = 1;  return temp; break;
    case 0:  temp = { 1, 1 };  return temp; break;
    default: temp = { -1,-1 };   return temp; break;
    }
};

/**
 *  Handles Ghost spawning
 *
 *  @param y    - y coordinate for coords
 *  @param x    - x coordinate for coords
 *  @param loop - current repitition / desired vertice
 *
 *  @return returns float for correct coord
 */
int checkCardinal(const float xRot, const float yRot) {
    if ((xRot < 0.5 && -0.5f < xRot) && (0.5f < yRot)) { return 0; } //North
    else if ((0.5f < xRot) && (yRot < 0.5 && -0.5f < yRot)) { return 1; } //East
    else if ((xRot < 0.5 && -0.5f < xRot) && (yRot < -yRot)) { return 2; } //South
    else if ((xRot < -0.5f) && (yRot < 0.5 && -0.5f < yRot)) { return 3; } //East
    else return -1;
};

// -----------------------------------------------------------------------------
// Clean VAO
// -----------------------------------------------------------------------------
void CleanVAO(GLuint& vao)
{
    GLint nAttr = 0;
    std::set<GLuint> vbos;

    GLint eboId;
    glGetVertexArrayiv(vao, GL_ELEMENT_ARRAY_BUFFER_BINDING, &eboId);
    glDeleteBuffers(1, (GLuint*)&eboId);

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nAttr);
    glBindVertexArray(vao);

    for (int iAttr = 0; iAttr < nAttr; ++iAttr)
    {
        GLint vboId = 0;
        glGetVertexAttribiv(iAttr, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vboId);
        if (vboId > 0)
        {
            vbos.insert(vboId);
        }

        glDisableVertexAttribArray(iAttr);
    }

    for (auto vbo : vbos)
    {
        glDeleteBuffers(1, &vbo);
    }

    glDeleteVertexArrays(1, &vao);
}
