/**
 *   @file     include.h
 *
 *	Includes all libraries for other files
 *
 *   @author   Axel Jacobsen
 */
#ifndef __INITIALIZE_H
#define __INITIALIZE_H

#include "include.h"
#include "camera.h"

GLFWwindow* initializeWindow();

//Callback functions
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void recieveCamera(Camera* camPoint);

void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam);

#endif