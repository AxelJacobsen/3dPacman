/**
 *   Header til map klassen.
 *
 *   @file     map.h
 *   @author   Axel Jacobsen
 */

#include "map.h"
#include "globFunc.h"

/**
*  Recieves lvlVect in to Pacman[0]
* 
*   @param filePath - map file filepath
* 
*   @see Map::mapFloatCreate()
*   @see Map::loadMapSpriteSheet()
*/
Map::Map(std::string filePath) {
    // Read from level file;
    std::ifstream inn(filePath);
    if (inn) {
        inn >> width; inn.ignore(1); inn >> height;
        XYshift.first = 2.0f / float(width);
        XYshift.second = 2.0f / float(height);
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
        mapI = tempMapVect;
        inn.close();
        mapFloatCreate();
        loadMapSpriteSheet();
    }
    else { printf("\n\nERROR: Couldnt find level file, check that it is in the right place.\n\n"); exit(EXIT_FAILURE); }
}

/**
 *  Creates map coordinates
 *
 *  @see GLfloat getCoordsWithInt(int y, int x, int loop);
 *  @see Map::findWhatWalls(const int x, const int y)
 *  @see Map::loopOrder(int num)
 *  @see Map::handleMapTexCoords(int rep)
 */
void Map::mapFloatCreate() {
    for (int i = 0; i < height; i++) { // creates map
        for (int j = 0; j < width; j++) {
            if (mapI[i][j] == 1) {
                int loop = 0;
                int wallType = findWhatWalls(j, i);
                std::vector<int> loopO = loopOrder(wallType);
                int counter = 0;
                for (int l = 0; l < howManyWalls(wallType); l++) {
                    float height = 0.0f;
                    for (int corners = 0; corners < 4; corners++) {
                        loop = loopO[counter];
                        counter++;
                        if (2 == corners) { height = 0.1f; };
                        for (int point = 0; point < 3; point++) {
                            mapF.push_back(mCamHolder->getCoordsWithInt(i, j, loop, height, XYshift));
                            loop++;
                        }
                        handleMapTexCoords(corners);
                    }
                }
            }
            else if (mapI[i][j] == 2) {
                pacSpawn.first = j;
                pacSpawn.second = i;
            }
            else { pelletAmount++; }
        }
    }
}

/**
 *  Handles Map Texture coords
 *
 *  @param rep - tells us what corner needs coords
 *
 */
void Map::handleMapTexCoords(int rep) {
    std::pair <int, int> temp = { 0,0 };
    switch (rep) {
    case 3:                     break;
    case 2:  temp.first = 1;    break;
    case 1:  temp.second = 1;   break;
    case 0:  temp = { 1, 1 };   break;
    default: temp = { -1,-1 };  break;
    }
    mapF.push_back(temp.first);
    mapF.push_back(temp.second);
}

/**
 *  Finds what wall tile draws a wall
 *
 *  @param x - tile x mapcoord
 *  @param y - tile y mapcoord
 *
 *  @return returns number from 1-15 deciding what type of wall is needed
 */
int Map::findWhatWalls(const int x, const int y) {
    int wallType = 0;
    for (int i = 0; i < 4; i++) {
        switch (i) {
        case 0: if ((y + 1) < height)   { if ((mapI[(y + 1)][x] == 0) || mapI[(y + 1)][x] == 2) { wallType += 1; } } break;
        case 1: if (0 < (x - 1))        { if ((mapI[y][(x - 1)] == 0) || mapI[y][(x - 1)] == 2) { wallType += 2; } } break;
        case 2: if ((x + 1) < width)    { if ((mapI[y][(x + 1)] == 0) || mapI[y][(x + 1)] == 2) { wallType += 4; } } break;
        case 3: if (0 < (y - 1))        { if ((mapI[(y - 1)][x] == 0) || mapI[(y - 1)][x] == 2) { wallType += 8; } } break;
        }
    }
    return wallType;
};

/**
 *  Gets number from finwhatwalls and returns ammount of walls that nubmer corresponds to
 *
 *  @param num - wallType
 *
 *  @return returns how many walls are to be drawn
 */
int Map::howManyWalls(int num) {
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
};

/**
 *  Finds what wall tile draws a wall
 *
 *  @param num - what type of wall
 *
 *  @return returns float order of walls
 * 
 *  not gonna lie, this took time
 */
std::vector<int> Map::loopOrder(int num) {
    std::pair<int, int> up = { 3, 6 };
    std::pair<int, int> left = { 0, 3 };
    std::pair<int, int> right = { 0, 9 };   //
    std::pair<int, int> down = { 6, 9 };    //
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
};

/**
 *  calls CompileShader for maps shader, also sets map verticie values
 *
 *
 *  @see CompileShader(const std::string& vertexShaderSrc,
            const std::string& fragmentShaderSrc)
 *  @see Map::callCreateMapVao()
 *  
 */
void Map::compileMapShader(){
    mapShaderProgram = CompileShader(   mapVertexShaderSrc,
                                        mapFragmentShaderSrc);

    GLint mPosAttrib = glGetAttribLocation(mapShaderProgram, "mPosition");
    glEnableVertexAttribArray(mPosAttrib);
    glVertexAttribPointer(mPosAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    callCreateMapVao();

    GLuint mtexAttrib = glGetAttribLocation(mapShaderProgram, "mTexcoord");
    glEnableVertexAttribArray(mtexAttrib);
    glVertexAttribPointer(mtexAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
}

void Map::callCreateMapVao() {
    int mapSize = mapF.size();
    mapVAO = CreateMap(mapSize);
}

// -----------------------------------------------------------------------------
//  CREATE MAP
// -----------------------------------------------------------------------------
/**
 *  Creates map VAO
 *
 *  @param size - size of map to be drawn
 *
 *  @see GLuint getIndices(int out, int mid, int in)
 *
 *  @return returns vao of map
 */
GLuint Map::CreateMap(float size) {
    std::vector<GLuint> mapIndices;
    for (int o = 0; o < (size / 3); o += 4) {
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
        size * sizeof((mapF)[0]),
        (&mapF[0]),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (const void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mapIndices.size() * sizeof(mapIndices[0]), (&mapIndices[0]), GL_STATIC_DRAW);

    return vao;
}

/**
 *  Loads texture for map Walls
 *
 *  @see load_opengl_texture(const std::string& filepath, GLuint slot)
 */
void Map::loadMapSpriteSheet() {
    mapSpriteSheet = load_opengl_texture("assets/wallTexture.png", 2);
}

/**
 *  Cleans shader, texturesheet & mapVAO
 *
 *  @see CleanVAO(GLuint& vao)
 */
void Map::cleanMap() {
    glDeleteProgram(mapShaderProgram);
    glDeleteTextures(1, &mapSpriteSheet);
    CleanVAO(mapVAO);
}

/**
 *  Handles Ghost spawning
 *
 *  @param ghostCount - amount of ghost points to be spawned
 *
 *  @return returns floats for pellet index for ghosts
 */
std::vector<int> Map::spawnGhost(const int ghostCount) {
    time_t t;
    srand((unsigned)time(&t));
    std::vector<int> formerPositions;
    bool noDouble = false;
    do {
        for (int g = 0; g < ghostCount; g++) {
            int randPos = 0;
            randPos = rand() % pelletAmount;
            formerPositions.push_back(randPos);
        }
        noDouble = true;
        if (1 < ghostCount) {
            for (int n = 0; n < ghostCount; n++) {
                for (int m = (n + 1); m < (ghostCount); m++) {
                    if (formerPositions[n] == formerPositions[m]) {
                        noDouble = false;
                        formerPositions.clear();
                    }
                }
            }
        }
    } while (!noDouble);
    return formerPositions;
};

/**
 *  Draws map
 *
 *  @param shader - shaderprogram to use for drawing
 *  @param vao    - vao of object
 * 
 *  @see Camera::applycamera(const GLuint shader, const float width, const float height)
 */
void Map::drawMap() {
    auto mapTextureLocation = glGetUniformLocation(mapShaderProgram, "u_mapTexture");
    glUseProgram(mapShaderProgram);
    glUniform1i(mapTextureLocation, 2);
    mCamHolder->applycamera(mapShaderProgram, XYshift.first, XYshift.second);
    glBindVertexArray(mapVAO);
    glDrawElements(GL_TRIANGLES, mapF.size(), GL_UNSIGNED_INT, (const void*)0);
}