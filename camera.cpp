#include "camera.h"

// -----------------------------------------------------------------------------
// Camera Class function definition
// -----------------------------------------------------------------------------

/**
 *  Handles checking what cardinal Direction the camera is facing relative to the map
 *
 *  @param xRot - xRot x of front of camera
 *  @param yRot - yRot y of front of camera
 *
 *  @return a number representing what direction the camera is facing
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
 *  @param y     - y coordinate for coords
 *  @param x     - x coordinate for coords
 *  @param loop  - current repitition / desired vertice
 *  @param layer - height of coords
 *  @param shift - width and height of a square, sent in a pair
 *
 *  @return returns float for correct coord
 */
GLfloat Camera::getCoordsWithInt(int y, int x, int loop, float layer, std::pair<float, float> shift) {
    float Xshift = shift.first;
    float Yshift = shift.second;
    GLfloat tempXs, tempYs;
    if (x == 0 && y == 0) { tempXs = 0, tempYs = 0; }
    else { tempXs = (Xshift * float(x)), tempYs = (Yshift * float(y)); }

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
 *  Applies camera transformation to supplied shader
 *
 *  @param shader - shader to apply transformation to
 *  @param width  - width of screeen
 *  @param height - height of screen
 *
 */
void Camera::applycamera(const GLuint shader, const float width, const float height) {
    const int spriteSize = 64;
    
    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)(width) / (float)(height), 0.001f, 2.0f);
    GLuint projMat = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projMat, 1, false, glm::value_ptr(projection));
    
    // camera/view transformation
    glm::mat4 view = glm::lookAt(cameraPos, (cameraPos + cameraFront), cameraUp);
    GLuint viewMat = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(viewMat, 1, false, glm::value_ptr(view));
}

/**
 *  Updates camera values and front vector
 *
 *  @param xpos - updated xPos of mouse
 *  @param ypos - updated yPos of mouse
 *
 */
void Camera::mouseMoveCamera(const double xpos, const double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top     
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.z = sin(glm::radians(pitch));
    front.x = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    int temp = checkCardinal(front.x, front.y);
    if (temp != -1) { setCard(temp); }
    cameraFront = glm::normalize(front);
}