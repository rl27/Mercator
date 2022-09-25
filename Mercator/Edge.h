#if 0

#include "hyper.h"
class Vertex;
class Tile;

#include <vector>

class Edge
{
public:
    Tile* tile1;
    Tile* tile2;
    Vertex* vertex1;
    Vertex* vertex2;

    Edge(Vertex* v1, Vertex* v2);
    std::vector<Vertex*> verts();
    std::vector<Vertex*> verts(glm::vec3 center); // Get vertices in ccw order relative to given center
};

#endif