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
#define TINYOBJLOADER_IMPLEMENTATION
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
int main(){
    //Container definition
    std::vector<Map*>		Maps;		///< Contains only map, permits adding more maps in the future
    std::vector<Pacman*>    Pacmans;    ///< Contains only pacman, done for ease of use
    std::vector<Ghost*>     Ghosts;     ///< Contains ghosts
    std::vector<Pellet*>    Pellets;    ///< Contains All pellets
    Camera* cameraAdress = new Camera();

    // Creates coordinates for map
    auto window = initializeWindow();
    recieveCamera(cameraAdress);
    if (window == nullptr) { return EXIT_FAILURE; }

    //Init map
    Maps.push_back(new Map("../../../../levels/level0"));
    std::pair<float, float>XYshift = Maps[0]->getXYshift();
    Maps[0]->getMapCameraPointer(cameraAdress);
    Maps[0]->compileMapShader();
    cameraAdress->recieveMap(Maps[0]->getIntMap());
    //printf("Map Loaded\n");

    //Init pacman
    Pacmans.push_back(new Pacman(Maps[0]->getPacSpawnPoint(), XYshift));
    Pacmans[0]->compilePacShader();
    Pacmans[0]->setWidthHeight(Maps[0]->getWidthHeight());
    Pacmans[0]->setXYshift(XYshift);
    Pacmans[0]->getCameraPointer(cameraAdress);
    Pacmans[0]->setVAO(Pacmans[0]->compilePacman());
    //printf("Pacman Loaded\n");

    //Init pellets
    std::pair<int, int> WidthHeight = Maps[0]->getWidthHeight();
    for (int y = 0; y < WidthHeight.second; y++) {
        for (int x = 0; x < WidthHeight.first; x++) {
            if (Maps[0]->getMapVal(x, y) == 0) { Pellets.push_back(new Pellet(x, y, XYshift)); }
        }
    }
    Maps[0]->setPelletAmount(Pellets.size());
    Pellets[0]->pelletSetWidthHeight(Maps[0]->getWidthHeight());
    Pellets[0]->getPelletCameraPointer(cameraAdress);

    int pelletStride = 3;
    std::vector<float> pelletContainer; //initial fill
    for (auto& it : Pellets) {
        for (int vert = 0; vert < Pellets[0]->getVertSize(); vert++) {
            pelletContainer.push_back(it->getVertCoord(vert));
        }
    }
    Pellets[0]->callCreatePelletVAO((&pelletContainer[0]), pelletContainer.size() * sizeof(pelletContainer[0]), pelletStride);
    Pellets[0]->callCompilePelletShader();

    //Pellet collision vector
    std::vector<std::vector<Pellet*>> pelletMap(WidthHeight.second, std::vector<Pellet*>(WidthHeight.first, nullptr));
    for (auto& pIT : Pellets) {
        std::pair<int, int> tempXY = pIT->getPelletXY();
        pelletMap[tempXY.second][tempXY.first] = pIT;
    }
    //printf("Pellet Loaded\n");

    //spawn ghosts
    int ghostAmount = 5;
    std::vector<int>ghostPos;
    if (0 < ghostAmount) { ghostPos = Maps[0]->spawnGhost(ghostAmount); 
    int count = 0,
        procs = 0;
        for (auto& it : Pellets) {
            for (int l = 0; l < ghostAmount; l++) {
                if (count == ghostPos[l]) {
                    Ghosts.push_back(new Ghost(it->checkCoords(0), it->checkCoords(1), true, WidthHeight, XYshift, cameraAdress));
                    procs++;
                }
            }
            if (procs == ghostAmount) { break; }
            count++;
        }
        Ghosts[0]->callLoadModel();
        Ghosts[0]->compileGhostModelShader();
        Ghosts[0]->loadGhostSpriteSheet();
        int insurance = 0;
        for (auto& initializeAllGhosts : Ghosts) {
            if (insurance != 0) {
                initializeAllGhosts->setShader(Ghosts[0]->getShader());
                initializeAllGhosts->setVAO(Ghosts[0]->getVAO());
                initializeAllGhosts->setModelSize(Ghosts[0]->getModelSize());
                //printf("Ghost Loaded: %i\n", insurance);
            }
            insurance++;
        }

    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_MULTISAMPLE);

    double currentTime = 0.0;
    glfwSetTime(0.0);
    float frequency = currentTime;
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;
    float delay     = 0.015f;
    bool fullscreen = false;

    std::pair<int, int> wihi = cameraAdress->getScreenSize();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //update Time
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
        
        if (Pacmans[0]->getRun()) {
            Pacmans[0]->drawPacman();

            //Pellet Collision
            float pacLerpProg = Pacmans[0]->getLerpProg();
            if (0.5f <= pacLerpProg && pacLerpProg <= 0.6) {
                std::pair<int, int> pacXY = Pacmans[0]->getXY();
                if (pelletMap[pacXY.second][pacXY.first] != nullptr){
                    if (pelletMap[pacXY.second][pacXY.first]->getPelletXY() == pacXY) {
                        if (pelletMap[pacXY.second][pacXY.first]->removePellet()) {
                            Pacmans[0]->updatePelletState(true);
                            Pacmans[0]->pickupPellet();
                        }
                    }
                }
            }

            //If pellets has been eaten, update
            if (Pacmans[0]->updatePelletState(false)) {
                pelletContainer.clear();
                for (auto & it: Pellets){
                    for (int vert = 0; vert < Pellets[0]->getVertSize(); vert++) {
                        pelletContainer.push_back(it->getVertCoord(vert));
                    }
                }
                Pellets[0]->cleanPelletVAO();
                Pellets[0]->callCreatePelletVAO((&pelletContainer[0]), pelletContainer.size() * sizeof(pelletContainer[0]), pelletStride);
                Pacmans[0]->updatePelletState(true);
                if (Pellets.size() <= Pacmans[0]->getPellets()) {
                    printf("All Pellets Collected\n");
                    Pacmans[0]->setRun(false);
                }
            }

            //Draws Ghosts
            if (0 < ghostAmount) {
                int mengde = 0;
                for (auto& drawGhostIt : Ghosts) {
                    drawGhostIt->drawGhostsAsModels(currentTime, WidthHeight);
                }
            }
            Maps[0]->drawMap();
        }
        
        //LERP Update
        if (currentTime > (frequency + delay) && Pacmans[0]->getRun()) {
            Pellets[0]->drawPellets(Pellets.size());
            frequency = currentTime;
            bool animate = false;

            if (Pacmans[0]->getAnimDel() == 0) { animate = true; Pacmans[0]->updateAnimDel(3, true);
            }  //the effective speed of animation
            else { Pacmans[0]->updateAnimDel(-1, false); }
            Pacmans[0]->checkForKeyUpdate();
            Pacmans[0]->updateLerp();

            if (animate) Pacmans[0]->pacAnimate();

            if (0 < ghostAmount){
                std::pair<int, int>pacPos = Pacmans[0]->getXY();
                for (auto& ghostIt : Ghosts) {
                    ghostIt->updateLerp();
                    if (animate) ghostIt->ghostAnimate();
                    if (ghostIt->checkGhostCollision(pacPos.first, pacPos.second, Maps[0]->getXYshift()))
                    {
                        printf("Ghost Collision\n"); Pacmans[0]->setRun(false);
                    }
                }
            }
            glfwSwapBuffers(window);
        }
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            if (!fullscreen) {
                fullscreen = true;
                glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), NULL, NULL, wihi.first, wihi.second, 30);
            }
            else {
                
                fullscreen = false;
                glfwSetWindowMonitor(window, NULL, NULL, NULL, wihi.first, wihi.second, 30);
            }
        }
    }

    glUseProgram(0);
    Maps[0]->cleanMap();
    Pacmans[0]->cleanCharacter();
    if (0 < ghostAmount) Ghosts[0]->cleanCharacter();
    Pellets[0]->cleanPellets();

    glfwTerminate();

    return EXIT_SUCCESS;
    
    return 1;
}

