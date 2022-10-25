#ifndef TILE_H
#define TILE_H

#include "hyper.h"
#include "Vertex.h"
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
    static std::vector<Tile*> visible;
    static std::vector<Tile*> next;
    static std::vector<Tile*> all;
    static std::queue<Tile*> parents;

    glm::vec3 center;
    std::string name;
    glm::vec4 color;
    int texture;
    float angle;
    int queueNum;
    Tile* parent;

    std::vector<Vertex*> vertices; // CCW order
    std::vector<Edge*> edges; // CCW order

    int n; // Number of vertices per tile
    int k; // Number of tiles per vertex

    Tile(int n, int k);
    Tile(Tile* ref, Edge* e, int n, int k);

    void populateEdges(); // Once all vertices are set, fill edges vector with edges
    int findEdge(Edge* e); // Find index of edge in edges vector
    void setVertexLocs(Tile* ref, Edge* e); // Set vertex locations for this tile, given a reference tile and edge
    std::vector<Tile*> getNeighbors(); // Get tile neighbors

    // Expand in all four directions, creating new tiles if necessary
    void expand(bool create);

    // Set starting tile position based on relative position to its center
    // Calls expand() repeatedly, breadth-first
    void setStart(glm::vec3 relPos);

    // Check if tile is in vector of all currently updated/visible tiles
    bool isVisible();
};

#endif