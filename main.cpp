/**
 *   OpenGL Pacman Group 34
 *
 *   The Program:
 *     - Draws necessary objects
 *     - Runs wall collision for pacman and ghosts
 *     - Permits consuming pellets
 *     - Utilizes sprites to draw characters
 *     - Has victory conditions with both ghosts and pellets
 *
 *   @file     main.cpp
 *   @author   Axel E.W. Jacobsen, Rafael P. Avalos, Mekides A. Abebe
 */

//File Inclusion
#include "map.h"
#include "character.h"
#include "pellet.h"


//Callback functions
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

std::vector<std::vector<int>>   loadFromFile();
std::vector<int>                loopOrder(int num);
std::pair<int, int>             handleMapTextCoord(const int rep);


void GLAPIENTRY MessageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam);




//Container definition
std::vector<Character*> Pacman;     ///< Contains only pacman, done for ease of use
std::vector<Character*> Ghosts;     ///< Contains ghosts
std::vector<Pellet*>    Pellets;    ///< Contains All pellets

//Container Functions
GLuint compileVertices(std::vector<Pellet*> itObj);
GLuint compileVertices(std::vector<Character*> itObj);


// -----------------------------------------------------------------------------
// ENTRY POINT
// -----------------------------------------------------------------------------
/**
 *  main function
 */
int main()
{
    int collected = 0,  //collected pellet counter
        resize    = 3;  //resizes window initially
    std::vector<std::vector<int>> levelVect = loadFromFile();
    std::vector<GLfloat> map;

    // Creates coordinates for map
    callMapCoordinateCreation(levelVect, &map);


    // Initialization of GLFW
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed." << '\n';
        std::cin.get();

        return EXIT_FAILURE;
    }

    // Setting window hints
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

   // auto window = glfwCreateWindow(width * spriteSize / resize, height * spriteSize / resize, "Pacman", nullptr, nullptr);
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

        return EXIT_FAILURE;
    }

    // Setting the OpenGL context.
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    auto playerShaderProgram = CompileShader(  playerVertexShaderSrc,
                                               playerFragmentShaderSrc);

    GLint pposAttrib = glGetAttribLocation(playerShaderProgram, "pPosition");
    glEnableVertexAttribArray(pposAttrib);
    glVertexAttribPointer(pposAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    auto pelletVAO           = compileVertices(Pellets);
    auto pelletShaderProgram = CompileShader(  pelletVertexShaderSrc,
                                               pelletFragmentShaderSrc);
    
    auto mapShaderProgram    = CompileShader(  mapVertexShaderSrc,
                                               mapFragmentShaderSrc);

    GLint mPosAttrib = glGetAttribLocation(mapShaderProgram, "mPosition");
    glEnableVertexAttribArray(mPosAttrib);
    glVertexAttribPointer(mPosAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    int mapSize = map.size();
    auto mapVAO = CreateMap(&map, (&map[0]));

    GLuint mtexAttrib = glGetAttribLocation(mapShaderProgram, "mTexcoord");
    glEnableVertexAttribArray(mtexAttrib);
    glVertexAttribPointer(mtexAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    auto ghostShaderProgram  = CompileShader(  ghostVertexShaderSrc,
                                               ghostFragmentShaderSrc);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_MULTISAMPLE);

    //Texture loading
    auto spriteSheetP = load_opengl_texture("assets/pacman.png", 0);
    auto spriteSheetG = load_opengl_texture("assets/pacman.png", 1);
    auto spriteSheetM = load_opengl_texture("assets/wallTexture.png", 2);

    double currentTime = 0.0;
    glfwSetTime(0.0);
    float frequency = currentTime;
    float delay = 0.015f;
    int animDelay = 10;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        currentTime = glfwGetTime();

        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
        if (run){
            //Draw calls
            drawPellets(pelletShaderProgram, pelletVAO);
            drawPacman( playerShaderProgram);
            if (ghostCount != 0) { drawGhosts(ghostShaderProgram); }
            drawMap(mapShaderProgram, mapVAO, mapSize);

            // Reloads pellets after consumption
            if (permittPelletUpdate) {
                CleanVAO(pelletVAO);
                pelletVAO = compileVertices(Pellets);
                permittPelletUpdate = false;
                collected++;
                if (collected == Pellets.size()) {
                    run = false;
                }
            }
        }
        //Update all Lerps

        if (currentTime > (frequency+delay) && run) {
            frequency = currentTime;
            bool animate = false;

            if (animDelay == 0) { animate = true; animDelay = 3; }  //the effective speed of animation
            else { animDelay--; }

            Pacman[0]->updateLerp();

            if (animate) Pacman[0]->pacAnimate();
            for (auto& ghostIt : Ghosts) {
                ghostIt->updateLerp();
                if (animate) ghostIt->AIanimate();
            }
            if (Pacman[0]->checkGhostCollision()) { run = false; }

            glfwSwapBuffers(window);
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
    }

    glUseProgram(0);
    glDeleteProgram(playerShaderProgram);
    glDeleteProgram(mapShaderProgram);
    glDeleteProgram(pelletShaderProgram);
    glDeleteProgram(ghostShaderProgram);

    glDeleteTextures(1, &spriteSheetP);
    glDeleteTextures(1, &spriteSheetG);
    glDeleteTextures(1, &spriteSheetM);

    CleanVAO(mapVAO);
    CleanVAO(pelletVAO);

    glfwTerminate();

    return EXIT_SUCCESS;
}

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
    if (temp != -1) { Pacman[0]->updateCard(temp);}
    cameraFront = glm::normalize(front);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    int currentCardDir = Pacman[0]->getCard();
    int desdir[4] = {2,9,4,3};

    switch (currentCardDir) {
        case 0: desdir[0] = 2; desdir[1] = 4; desdir[2] = 3; desdir[3] = 9; break;  //North
        case 1: desdir[0] = 9; desdir[1] = 3; desdir[2] = 2; desdir[3] = 4; break;  //East
        case 2: desdir[0] = 4; desdir[1] = 2; desdir[2] = 9; desdir[3] = 3; break;  //South
        case 3: desdir[0] = 3; desdir[1] = 9; desdir[2] = 4; desdir[3] = 2; break;  //West
    }
    if (action == GLFW_PRESS) {
        float cameraSpeed = 2.5 * deltaTime;
        switch (key) {
        case GLFW_KEY_W:        if (Pacman[0]->getLegalDir(desdir[0])) {
            Pacman[0]->updateDir(desdir[0]); Pacman[0]->changeDir();
        };  break;
        case GLFW_KEY_S:        if (Pacman[0]->getLegalDir(desdir[1])) {
            Pacman[0]->updateDir(desdir[1]); Pacman[0]->changeDir();
        };  break;
        case GLFW_KEY_A:        if (Pacman[0]->getLegalDir(desdir[2])) {
            Pacman[0]->updateDir(desdir[2]); Pacman[0]->changeDir();
        };  break;
        case GLFW_KEY_D:        if (Pacman[0]->getLegalDir(desdir[3])) {
            Pacman[0]->updateDir(desdir[3]); Pacman[0]->changeDir();
        };  break;
        }
    }
}
