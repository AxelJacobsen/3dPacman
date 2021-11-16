#include "camera.h"

// -----------------------------------------------------------------------------
// Class function definition
// -----------------------------------------------------------------------------

/**
 *  Handles Ghost spawning
 *
 *  @param y    - y coordinate for coords
 *  @param x    - x coordinate for coords
 *  @param loop - current repitition / desired vertice
 *
 *  @return returns float for correct coord
 */
int Camera::checkCardinal(const float xRot, const float yRot) {
    if ((xRot < 0.5 && -0.5f < xRot) && (0.5f < yRot)) { return 0; } //North
    else if ((0.5f < xRot) && (yRot < 0.5 && -0.5f < yRot)) { return 1; } //East
    else if ((xRot < 0.5 && -0.5f < xRot) && (yRot < -yRot)) { return 2; } //South
    else if ((xRot < -0.5f) && (yRot < 0.5 && -0.5f < yRot)) { return 3; } //East
    else return -1;
};

/**
 *  returns coordinates based on give x y and "loop"
 *
 *  @param y    - y coordinate for coords
 *  @param x    - x coordinate for coords
 *  @param loop - current repitition / desired vertice
 *
 *  @return returns float for correct coord
 */
GLfloat Camera::getCoordsWithInt(int y, int x, int loop, float layer, std::pair<float, float> shift) {
    float Xshift = shift.first;
    float Yshift = shift.second;

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
 *  Applies camera tranformation
 */
void Camera::applycamera(const GLuint shader, const float width, const float height) {
    int spriteSize = 64;
    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)(width * spriteSize / 3) / (float)(height * spriteSize / 3), 0.001f, 2.0f);
    GLuint projMat = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projMat, 1, false, glm::value_ptr(projection));

    // camera/view transformation
    glm::mat4 view = glm::lookAt(cameraPos, (cameraPos + cameraFront), cameraUp);
    GLuint viewMat = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(viewMat, 1, false, glm::value_ptr(view));
}