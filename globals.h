/**
 *
 *   @file     globals.h
 *   @author   Axel Jacobsen
 */

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "include.h"
#include "pellet.h"
#include "character.h"
#include "map.h"

 // -----------------------------------------------------------------------------
 // Globals
 // -----------------------------------------------------------------------------
const int spriteSize = 64;

int      width, height;                     ///< Width and Height kept global due to frequent use in various places
int      ghostCount = 0;                    ///< Ammount of ghosts, soft cap at 20 due to processing power
float    Xshift, Yshift;                    ///< Width and Height of one "square"
bool     permittPelletUpdate = false,       ///< Reloads Pellet VAO
run = true;									///< End condition

//Mouse implimentation
// camera
glm::vec3 cameraPos = glm::vec3(-1.0f, 0.0f, 0.1f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 0.5f);

bool firstMouse = true;
float yaw = 0.0f;
float pitch = 0.0f;
float lastX = 0;
float lastY = 0;
float fov = 120.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

#endif