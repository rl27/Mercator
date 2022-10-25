#pragma once

#include "hyper.h"
#include <vector>
#include <iostream>

class Edge;
class Tile;

class Vertex
{
public:
    int k; // Number of edges per vertex.
    std::vector<Edge*> edges;
    glm::dvec3 pos;
    bool initialized;

    Vertex(int k);
    Vertex(int k, glm::dvec3 loc);
    void setPos(glm::dvec3 loc);
    glm::dvec3 getPos();
    void addEdge(Edge* e);
    Edge* next(Edge* e); // Get next edge in counter clockwise order.
    Edge* prev(Edge* e); // Get prev edge in ccw order (i.e. next edge in clockwise order).
    void clamp(glm::dvec3 loc);
    void replaceEdge(Edge* oldEdge, Edge* newEdge);
    int seekEdge(Edge* e); // Find index of given edge in the vector of edges.
    Edge* seekVertex(Vertex* v); // Find edge connecting this vertex to the given one.
};

class Edge
{
public:
    std::vector<Tile*> tiles;
    Vertex* vertex1;
    Vertex* vertex2;

    Edge(Vertex* v1, Vertex* v2);
    void addTile(Tile* t);
    std::vector<Vertex*> verts();
    std::vector<Vertex*> verts(glm::dvec3 center); // Get vertices in ccw order relative to given center
    bool hasDangling();
    void merge(Edge* e);
};
