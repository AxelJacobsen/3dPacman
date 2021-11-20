/**
 *   Header til map klassen.
 *
 *   @file     globfunc.h
 *   @author   Axel Jacobsen
 */

#ifndef __GLOBFUNC_H
#define __GLOBFUNC_H

//Inclusions
#include "include.h"

// -----------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// -----------------------------------------------------------------------------

void    CleanVAO(GLuint& vao);
GLuint  CompileShader(const std::string& vertexShader, const std::string& fragmentShader);
GLuint  CompileShader(const std::string& vertexShader, const std::string& fragmentShader,
                                                       const std::string& geometryShaderSrc);
GLuint  CreateObject(GLfloat *object, int size, const int stride);
GLuint  CreateObject(GLfloat* object, int size, const int stride, bool noEbo);
GLuint  getIndices(      int out, int mid, int in);
GLuint  load_opengl_texture(const std::string& filepath, GLuint slot);
void    TransformMap(const GLuint);

#endif