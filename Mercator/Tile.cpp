#include "Tile.h"

// For origin tile
Tile::Tile(int n, int k) : name("O"), n(n), k(k) {
    float r = ((float)rand() / (RAND_MAX));
    float g = ((float)rand() / (RAND_MAX));
    float b = ((float)rand() / (RAND_MAX));
    color = glm::vec4(r, g, b, 1.0f);

    center = glm::dvec3(0, 1, 0);

    //Vertex* first_vert = new Vertex(k, hypNormalize(reversePoincare(circleRadius(n, k), 0)));
    Vertex* first_vert = new Vertex(k, reversePoincare(circleRadius(n, k), 0));
    vertices.push_back(first_vert);
    Edge* first_edge = first_vert->edges.at(0);
    
    Edge* edge = first_edge;
    edge->addTile(this);

    for (int i = 1; i < n; i++) {
        Vertex* next_vert = edge->vertex2;
        glm::dvec3 next_loc = rotate(vertices.at(i - 1)->getPos(), 2 * M_PI / n);
        next_vert->clamp(next_loc);
        vertices.push_back(next_vert);
        
        edge = next_vert->prev(edge);
        edge->addTile(this);
    }
    
    Edge* merge_edge = first_vert->next(first_edge);
    edge->merge(merge_edge);

    populateEdges();

    texture = -1;
    angle = 0;
    queueNum = -1;
}

// For non-origin tiles
Tile::Tile(Tile* ref, Edge* e, int n, int k) : name("N"), n(n), k(k) {
    float r = ((float)rand() / (RAND_MAX));
    float g = ((float)rand() / (RAND_MAX));
    float b = ((float)rand() / (RAND_MAX));
    color = glm::vec4(r, g, b, 1.0f);

    e->addTile(this);

    center = extend(ref->center, midpoint(e->vertex1->getPos(), e->vertex2->getPos()));

    std::vector<Vertex*> verts = e->verts(center);
    vertices.push_back(verts.at(1));

    Vertex* back_vert = verts.at(0);
    Vertex* front_vert = verts.at(1);

    vertices.insert(vertices.begin(), back_vert);

    Edge* back_edge = back_vert->next(e);
    while (back_vert != front_vert && !back_edge->hasDangling()) {
        back_edge->addTile(this);
        back_vert = (back_vert == back_edge->vertex1) ? back_edge->vertex2 : back_edge->vertex1; //back_vert = back_edge->verts(center).at(0);
        vertices.insert(vertices.begin(), back_vert);
        back_edge = back_vert->next(back_edge);
    }

    // Made a loop; all vertices accounted for
    if (back_vert == front_vert)
        vertices.erase(vertices.begin());
    else { // Need to complete the vertices
        
        Vertex* vertex = front_vert;
        Edge* edge = vertex->prev(e);
        edge->addTile(this);

        Vertex* reflecting_vertex = e->verts(ref->center).at(1);
        Edge* ref_edge = reflecting_vertex->prev(e);

        glm::dvec3 midpt = midpoint(e->vertex1->getPos(), e->vertex2->getPos());

        int size = vertices.size();
        for (int i = size; i < n; i++) {
            reflecting_vertex = (reflecting_vertex == ref_edge->vertex1) ? ref_edge->vertex2 : ref_edge->vertex1; //reflecting_vertex = ref_edge->verts(ref->center).at(1);

            glm::dvec3 next_loc = extend(reflecting_vertex->getPos(), midpt);
            if (!edge->vertex2->initialized) {
                vertex = edge->vertex2;
                vertex->clamp(next_loc);
            } else {
                vertex = (vertex == edge->vertex1) ? edge->vertex2 : edge->vertex1; // vertex = edge->verts(center).at(1);
                vertex->setPos(next_loc);
            }

            vertices.push_back(vertex);

            edge = vertex->prev(edge);
            ref_edge = reflecting_vertex->prev(ref_edge);
            edge->addTile(this);
        }

        edge->merge(back_edge);
    }

    populateEdges();

    texture = -1;
    angle = 0;
    queueNum = -1;
}

void Tile::populateEdges() {
    for (int i = 0; i < n; i++) {
        Vertex* v1 = vertices.at(i);
        Vertex* v2 = vertices.at((i + 1) % n);
        edges.push_back(v1->seekVertex(v2));
    }

    /*Vertex* v1 = vertices.at(0);
    Vertex* v2 = vertices.at(1);
    Edge* sweep = v1->seekVertex(v2);
    edges.push_back(sweep);
    Vertex* front = sweep->verts(center).at(1);

    for (int i = 0; i < n - 1; i++) {
        sweep = front->prev(sweep);
        edges.push_back(sweep);
        front = sweep->verts(center).at(1);
    }*/
}

int Tile::findEdge(Edge* e) {
    auto it = std::find(edges.begin(), edges.end(), e);
    assert(it != edges.end());
    return it - edges.begin();
}

void Tile::setVertexLocs(Tile* ref, Edge* e) {
    glm::dvec3 midpt = midpoint(e->vertex1->getPos(), e->vertex2->getPos());
    center = extend(ref->center, midpt);

    Vertex* vertex = e->verts(center).at(1);
    Edge* edge = vertex->prev(e);

    Vertex* reflecting_vertex = e->verts(ref->center).at(1);
    Edge* ref_edge = reflecting_vertex->prev(e);

    for (int i = 0; i < n-2; i++) {
        // ccw vertex ordering breaks down when the vertex locations are inaccurate.
        // Need to rely on comparing vertex1 and vertex2 instead of using verts().
        vertex = (vertex == edge->vertex1) ? edge->vertex2 : edge->vertex1;
        reflecting_vertex = (reflecting_vertex == ref_edge->vertex1) ? ref_edge->vertex2 : ref_edge->vertex1; //reflecting_vertex = ref_edge->verts(ref->center).at(1);
        
        glm::dvec3 next_loc = extend(reflecting_vertex->getPos(), midpt);
        vertex->setPos(next_loc);

        edge = vertex->prev(edge);
        ref_edge = reflecting_vertex->prev(ref_edge);
    }
}

void Tile::setVertexLocs2(Tile* ref, Edge* e) {
    glm::dvec3 midpt = midpoint(e->vertex1->getPos(), e->vertex2->getPos());
    center = extend(ref->center, midpt);

    Vertex* vertex = e->verts(center).at(1);
    Edge* edge = vertex->prev(e);

    Vertex* reflecting_vertex = vertex;
    Edge* ref_edge = reflecting_vertex->next(e);

    for (int i = 0; i < n - 2; i++) {
        // ccw vertex ordering breaks down when the vertex locations are inaccurate.
        // Need to rely on comparing vertex1 and vertex2 instead of using verts().
        vertex = (vertex == edge->vertex1) ? edge->vertex2 : edge->vertex1;
        reflecting_vertex = (reflecting_vertex == ref_edge->vertex1) ? ref_edge->vertex2 : ref_edge->vertex1;

        glm::dvec3 next_loc = symmetry(reflecting_vertex->getPos(), ref->center, center);
        vertex->setPos(next_loc);

        edge = vertex->prev(edge);
        ref_edge = reflecting_vertex->next(ref_edge);
    }
}

void Tile::expand() {
    for (Edge* e : edges) {
        Tile* other_tile = NULL;
        if (e->tiles.size() < 2) {
            other_tile = new Tile(this, e, n, k);
            all.push_back(other_tile);
        }
        else {
            assert(e->tiles.size() == 2);
            other_tile = (this == e->tiles.at(0)) ? e->tiles.at(1) : e->tiles.at(0);
            // other_tile->setVertexLocs(this, e);
        }
        if (other_tile && !other_tile->isVisible()) {
            next.push_back(other_tile);
            visible.push_back(other_tile);
            other_tile->setVertexLocs2(this, e);
        }
    }
}

void Tile::setStart(glm::dvec3 relPos) {
    //vertices.at(0)->setPos(rotate(hypNormalize(reversePoincare(circleRadius(n, k), 0)), angle));
    vertices.at(0)->setPos(rotate(reversePoincare(circleRadius(n, k), 0), angle));
    for (int i = 1; i < n; i++)
        vertices.at(i)->setPos(rotate(vertices.at(i - 1)->getPos(), 2 * M_PI / n));

    for (int i = 0; i < n; i++)
        vertices.at(i)->setPos(translateXZ(vertices.at(i)->getPos(), relPos.x, relPos.z));

    center = translateXZ(glm::dvec3(0, 1, 0), relPos.x, relPos.z);

    std::vector<Tile*> copy;

    next.clear();
    next.push_back(this);

    visible.clear();
    visible.push_back(this);

    while (next.size() != 0) {
        Tile* t = next.back();
        next.pop_back();
        if (t->withinRadius(0.75))
            t->expand();
    }

    /*
    // This is for marking tiles to receive generated outputs; comment out to disable
    if (!parent) {
        parents.push(this);
        parent = this;
        for (Tile* t : getNeighbors()) {
            if (!t->parent)
                t->parent = this;
        }
    }*/
}

std::vector<Tile*> Tile::getNeighbors() {
    std::vector<Tile*> neighbors;
    for (Edge* e : edges) {
        if (e->tiles.size() == 2) {
            Tile* neighbor = (this == e->tiles.at(0)) ? e->tiles.at(1) : e->tiles.at(0);
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

bool Tile::isVisible() {
    if (std::find(visible.begin(), visible.end(), this) != visible.end())
        return true;
    return false;
}

bool Tile::withinRadius(double rad) {
    for (Vertex* v : vertices) {
        if (glm::distance(glm::dvec3(0), getPoincare(v->getPos())) < rad)
            return true;
    }
    return false;
}