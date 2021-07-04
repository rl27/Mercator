#ifndef TILE_H
#define TILE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "hyper.h"
#include <string>
#include <random>
#include <vector>
#include <algorithm>
#include <iostream>

/* Tile class for square tiles.
* Order-5 square tiling is achieved by having a distance of phi (golden ratio) between the centers of neighboring tiles.
*/
class Tile
{
public:
    static std::vector<Tile*> tiles;

    glm::vec3 center;
    glm::vec3 TL;
    glm::vec3 TR;
    glm::vec3 BL;
    glm::vec3 BR;
    Tile *Up;
    Tile *Left;
    Tile *Right;
    Tile *Down;
    std::string name;
    glm::vec4 color;
    int num;

    Tile(std::string n);

    // Recursively expand in each direction
    void expand();

    // Set starting tile position based on relative position to its center
    void setStart(glm::vec3 relPos);

    // Set neighboring tiles
    void setRight(Tile* R);
    void setLeft(Tile* L);
    void setUp(Tile* U);
    void setDown(Tile* D);

    // Check if in vector of tiles
    bool inTiles();
};

#endif