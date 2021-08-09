#ifndef TILE_H
#define TILE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "hyper.h"
#include <string>
#include <random>
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>

/* Tile class for square tiles.
* Order-5 square tiling (5 squares at each corner) is achieved by having a hyperbolic distance
* of phi (golden ratio) between the centers of neighboring tiles. */

class Tile
{
public:
    static std::vector<Tile*> tiles;
    static std::vector<Tile*> next;
    static std::vector<Tile*> created;
    static std::queue<Tile*> all;

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
    int texture;
    float angle;
    int queueNum;

    Tile(std::string n);

    // Expand in all four directions, creating new tiles if necessary
    void expand(bool create);

    // Set starting tile position based on relative position to its center
    // Calls expand() repeatedly, breadth-first
    void setStart(glm::vec3 relPos);

    // Update center and corners of neighboring tiles
    void setRight(Tile* R);
    void setLeft(Tile* L);
    void setUp(Tile* U);
    void setDown(Tile* D);

    // Get position of center of possible neighbors
    glm::vec3 getRight();
    glm::vec3 getLeft();
    glm::vec3 getUp();
    glm::vec3 getDown();

    // Check if tile is in vector of all currently updated tiles
    bool inTiles();

    // Checks for unconnected neighboring tiles and connects them
    void connectInTiles();
};

#endif