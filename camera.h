/**
 *   Header to the Camera class.
 *
 *   @file     camera.h
 *   @author   Axel Jacobsen
 */

#ifndef __CAMERA_H
#define __CAMERA_H

#include "include.h"

 // -----------------------------------------------------------------------------
 // Camera Class header
 // -----------------------------------------------------------------------------
class Camera {  
private:
    glm::vec3 cameraPos = glm::vec3(-1.0f, 0.0f, 0.05f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 0.5f);
    std::vector<std::vector<int>> mapHolder;

    bool firstMouse = true;
    float yaw   = 180.0f;
    float pitch = 0.0f;
    float lastX = 0;
    float lastY = 0;
    float fov   = 90.0f;
    int   pacCard = 0;
    int screenWidth = 1000,
        screenHeight = 1000;

    bool  keyCalled = false;
    int   pacDesDir = 2;
public:
    Camera() {};
    glm::vec3 getCamPos()   { return cameraPos; };
    glm::vec3 getCamFront() { return cameraFront; };
    glm::vec3 getCamUp()    { return cameraUp; };
    void setCamPos(glm::vec3    newCameraPos)   { cameraPos     = newCameraPos;     };
    void setCamFront(glm::vec3  newCameraFront) { cameraFront   = newCameraFront;   };
    void setCamUp(glm::vec3     newCameraUp)    { cameraUp      = newCameraUp;      };

    bool    getFirstMouse()       { return firstMouse;    };
    void    disableFirstMouse()   { firstMouse = false;   };
    int     getCard()             { return pacCard;       };
    void    setCard(int newCard)  { pacCard = newCard;    }
    int     getCamMapVal(int x, int y) {  return mapHolder[y][x]; };
    int     getNewDesDir() { if (keyCalled) { keyCalled = false; return pacDesDir; } else return -10; };
    void    setNewDesDir(int newDir) { pacDesDir = newDir; keyCalled = true; }
    int     checkCardinal(const float xRot, const float yRot);
    GLfloat getCoordsWithInt(int y, int x, int loop, float layer, std::pair<float, float> shift);
    void    applycamera(const GLuint shader, const float width, const float height);
    void    recieveMap(std::vector<std::vector<int>> valueMap) { mapHolder = valueMap; }
    void    mouseMoveCamera(const double xpos, const double ypos);
    std::pair<int, int> getScreenSize() { std::pair<int, int> wh = { screenWidth, screenHeight }; return wh; }
};

#endif
