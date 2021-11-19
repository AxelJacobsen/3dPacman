/**
 *   Header til map klassen.
 *
 *   @file     map.h
 *   @author   Axel Jacobsen
 */

#ifndef __MAP_H
#define __MAP_H

#include "include.h"
#include "camera.h"
 /**
  *  Map
  */
class Map {
private:
    std::vector<std::vector<int>> mapI;     //Holds the level0 map in Pacman[0]
    std::vector<float> mapF;                //Holds the level0 map coordinates in Pacman[0]
    GLuint mapShaderProgram;
    GLuint mapVAO;
    GLuint mapSpriteSheet;
    std::pair<int, int> pacSpawn = {0,0};
    std::pair<float, float> XYshift{0,0};
    int pelletAmount = 0,
        width, height;

    Camera* mCamHolder;
public:
    Map() {};
    Map(std::string filePath);
    void   mapFloatCreate();
    void   handleMapTexCoords(int rep);
    int    findWhatWalls(const int x, const int y);
    int    howManyWalls(int num);
    std::vector<int> loopOrder(int num);
    void   compileMapShader();
    void   callCreateMapVao();
    GLuint CreateMap(float size);
    void   loadMapSpriteSheet();
    void   cleanMap();
    std::vector<int> spawnGhost(const int ghostCount);
    //Getters
    GLuint getMapShader()   { return mapShaderProgram; };
    GLuint getMapVAO()      { return mapVAO; };
    GLuint getMapSprite()   { return mapSpriteSheet; };
    int    getMapSize()     { return mapF.size(); };
    int    getMapVal(int x, int y) { return mapI[y][x]; };
    void   setPelletAmount(int size) { pelletAmount = size; }
    int    getPelletAmount() { return pelletAmount; };

    std::pair<float, float> getXYshift() { return XYshift; }
    std::pair<int, int> getWidthHeight() {
                        std::pair<int, int> size = { width, height };
                                            return size; }
    std::pair<int,int> getPacSpawnPoint() { return pacSpawn; };
    std::vector<std::vector<int>> getIntMap() { return mapI; }
    void drawMap();
    void getMapCameraPointer(Camera* newCamera) { mCamHolder = newCamera; };
};  

#endif
