#include "globFunc.h"
#include "camera.h"
#include <stb_image.h>

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
// GLOBAL FUNCTIONS
// -----------------------------------------------------------------------------

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
 *  Compiles all verticie lists into a large vector and calls CreateObject
 *
 *  @param itObj - which type of object to iterate, here either Pacman or ghost
 *
 *  @see Character::getVertCoord(int index);
 *  @see GLuint CreateObject(GLfloat* object, int size, const int stride);
 *
 *  @return returns VAO gotten from CreateObject func
 */
/*
GLuint compileVertices(std::vector<Ghost*> itObj) {
    std::vector<GLfloat> veticieList;
    int stride = 5;
    for (auto& it : itObj) {
        for (int i = 0; i < 20; i++) {
            veticieList.push_back(it->getVertCoord(i));
        }
    }
    return CreateObject(&veticieList[0], veticieList.size() * sizeof(veticieList[0]), stride);
}
*/

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