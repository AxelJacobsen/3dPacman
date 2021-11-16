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

#include "initialize.h"
#include "pacman.h"
#include "ghost.h"
#include "pellet.h"
#include "map.h"

// -----------------------------------------------------------------------------
// ENTRY POINT
// -----------------------------------------------------------------------------
/**
 *  main function
 */
int main()
{
  
    //Container definition
    std::vector<Map*>		Maps;		///< Contains only map, permits adding more maps in the future
    std::vector<Pacman*>    Pacmans;    ///< Contains only pacman, done for ease of use
    std::vector<Ghost*>     Ghosts;     ///< Contains ghosts
    std::vector<Pellet*>    Pellets;    ///< Contains All pellets
    Camera* cameraAdress = new Camera();

    // Creates coordinates for map
    auto window = initializeWindow();
    if (window == nullptr) { return EXIT_FAILURE; }

    //create map
    Maps.push_back(new Map("../../../../levels/level0"));
    std::pair<float, float>XYshift = Maps[0]->getXYshift();
    Maps[0]->getMapCameraPointer(cameraAdress);

    //spawn pacman
    Pacmans.push_back(new Pacman(Maps[0]->getPacSpawnPoint()));
    Pacmans[0]->setXYshift(XYshift);
    Pacmans[0]->getCameraPointer(cameraAdress);

    //spawn pellets
    std::pair<int, int> WidthHeight = Maps[0]->getWidthHeight();
    for (int x = 0; x < WidthHeight.first; x++) {
        for (int y = 0; y < WidthHeight.second; y++) {
            if (Maps[0]->getMapVal(x, y) == 0) { Pellets.push_back(new Pellet(x, y)); }
        }
    }
    Pellets[0]->setXYshift(XYshift);
    Pellets[0]->getPelletCameraPointer(cameraAdress);
    //spawn ghosts
    int ghostAmount = 4;
    std::vector<int>ghostPos = Maps[0]->spawnGhost(ghostAmount);
    int count = 0;
    for (auto& it : Pellets) {
        for (int l = 0; l < ghostAmount; l++) {
            if (count == ghostPos[l]) {
                Ghosts.push_back(new Ghost(it->checkCoords(0), it->checkCoords(1), true));
            }
        }
        count++;
    }
    Ghosts[0]->setXYshift(XYshift);
    Ghosts[0]->getCameraPointer(cameraAdress);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_MULTISAMPLE);

    double currentTime = 0.0;
    glfwSetTime(0.0);
    float frequency = currentTime;
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;
    float delay = 0.015f;

    std::vector<float> pelletContainer; //initial fill
    std::vector<float> ghostContainer; //initial fill
    for (auto& it : Pellets) {
        for (int vert = 0; vert < Pellets[0]->getVertSize(); vert++) {
            pelletContainer.push_back(it->getVertCoord(vert));
        }
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //update Time
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        //Refresh Ghosts
        for (auto& it : Ghosts) {
            for (int vert = 0; vert < Ghosts[0]->getVertSize(); vert++) {
                ghostContainer.push_back(it->getVertCoord(vert));
            }
        }

        if (Pacmans[0]->getRun()) {
            Pellets[0]->drawPellets(Pellets.size());
            Pacmans[0]->drawPacman();
            
            if (0 < Ghosts.size()) { 
                Ghosts[0]->callCreateCharacterVao((&ghostContainer[0]), Ghosts.size(), 5);
                Ghosts[0]->drawGhosts(ghostAmount); 
            }
            Maps[0]->drawMap();

            if (Pacmans[0]->updatePelletState(false)) {//fills Pelletcontainer
                for (auto & it: Pellets){
                    for (int vert = 0; vert < Pellets[0]->getVertSize(); vert++) {
                        pelletContainer.push_back(it->getVertCoord(vert));
                    }
                }
                Pellets[0]->cleanPelletVAO();
                Pellets[0]->callCreatePelletVAO((&pelletContainer[0]), Pellets.size(), 3);
                Pacmans[0]->updatePelletState(true);
                Pacmans[0]->pickupPellet();
                if (Pellets.size() <= Pacmans[0]->getPellets()) {
                    Pacmans[0]->setRun(false);
                }
            }
        }
        //LERP Update
        if (currentTime > (frequency + delay) && Pacmans[0]->getRun()) {
            frequency = currentTime;
            frequency = currentTime;
            bool animate = false;

            if (Pacmans[0]->getAnimDel() == 0) { animate = true; Pacmans[0]->updateAnimDel(3, true);
            }  //the effective speed of animation
            else { Pacmans[0]->updateAnimDel(-1, false); }

            Pacmans[0]->updateLerp();

            if (animate) Pacmans[0]->pacAnimate();
            for (auto& ghostIt : Ghosts) {
                ghostIt->updateLerp();
                if (animate) ghostIt->ghostAnimate();
            }
            //if (Pacmans[0]->checkGhostCollision()) { Pacmans[0]->setRun(false); }

            glfwSwapBuffers(window);
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
    }

    glUseProgram(0);
    Maps[0]->cleanMap();
    Pacmans[0]->cleanCharacter();
    Ghosts[0]->cleanCharacter();
    Pellets[0]->cleanPellets();


    glfwTerminate();

    return EXIT_SUCCESS;
    
    return 1;
}

