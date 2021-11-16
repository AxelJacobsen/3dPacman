/**
 *   @file     include.h
 *
 *	Includes all libraries for other files
 *
 *   @author   Axel Jacobsen
 */

#include "initialize.h"

GLFWwindow* initializeWindow() {

    // Initialization of GLFW
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed." << '\n';
        std::cin.get();
    }

    // Setting window hints
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    auto window = glfwCreateWindow(1000, 1000, "Pacman", nullptr, nullptr);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (window == nullptr)
    {
        std::cerr << "GLFW failed on window creation." << '\n';
        std::cin.get();

        glfwTerminate();

    }

    // Setting the OpenGL context.
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    return window;
}

Camera* cameraHolder;

// -----------------------------------------------------------------------------
// MessageCallback (for debugging purposes)
// -----------------------------------------------------------------------------
void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    std::cerr << "GL CALLBACK:" << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") <<
        "type = 0x" << type <<
        ", severity = 0x" << severity <<
        ", message =" << message << "\n";
}

// -----------------------------------------------------------------------------
// Callback Functions
// -----------------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    glm::vec3 tempCameraFront = cameraHolder->getCamFront();
    float tempYaw = cameraHolder->getYaw();
    float tempPitch = cameraHolder->getpitch();
    float tempLastX = cameraHolder->getlastX();
    float tempLastY = cameraHolder->getlastY();
    ;
    if (cameraHolder->getFirstMouse())
    {
        tempLastX = xpos;
        tempLastY = ypos;
        cameraHolder->disableFirstMouse();
    }

    float xoffset = xpos - tempLastX;
    float yoffset = tempLastY - ypos; // reversed since y-coordinates go from bottom to top     
    tempLastX = xpos;
    tempLastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    tempYaw += xoffset;
    tempPitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (tempPitch > 89.0f)
        tempPitch = 89.0f;
    if (tempPitch < -89.0f)
        tempPitch = -89.0f;

    glm::vec3 front;
    front.z = sin(glm::radians(tempPitch));
    front.x = -cos(glm::radians(tempYaw)) * cos(glm::radians(tempPitch));
    front.y = sin(glm::radians(tempYaw)) * cos(glm::radians(tempPitch));
    int temp = cameraHolder->checkCardinal(front.x, front.y);
    if (temp != -1) { cameraHolder->setCard(temp); }
    cameraHolder->setCamFront(glm::normalize(front));
    cameraHolder->setYaw(tempYaw);
    cameraHolder->setpitch(tempPitch);
    cameraHolder->setlastX(tempLastX);
    cameraHolder->setlastY(tempLastY);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    int currentCardDir = cameraHolder->getCard();
    int desdir[5] = { 2,9,4,3 };

    switch (currentCardDir) {
    case 0: desdir[0] = 2; desdir[1] = 4; desdir[2] = 3; desdir[3] = 9; break;  //North
    case 1: desdir[0] = 9; desdir[1] = 3; desdir[2] = 2; desdir[3] = 4; break;  //East
    case 2: desdir[0] = 4; desdir[1] = 2; desdir[2] = 9; desdir[3] = 3; break;  //South
    case 3: desdir[0] = 3; desdir[1] = 9; desdir[2] = 4; desdir[3] = 2; break;  //West
    }
    if (action == GLFW_PRESS) {
        //float cameraSpeed = 2.5 * deltaTime;
        switch (key) {
        case GLFW_KEY_W: cameraHolder->setNewDesDir(desdir[0]);  break;
        case GLFW_KEY_S: cameraHolder->setNewDesDir(desdir[1]);  break;
        case GLFW_KEY_A: cameraHolder->setNewDesDir(desdir[2]); break;
        case GLFW_KEY_D: cameraHolder->setNewDesDir(desdir[3]);  break;
        }
    }
}

void recieveCamera(Camera* camPoint) {
    cameraHolder = camPoint;
}