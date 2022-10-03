#include "Tile.h"

// For origin tile
Tile::Tile(int n, int k) : name("O"), n(n), k(k) {
    float r = ((float)rand() / (RAND_MAX));
    float g = ((float)rand() / (RAND_MAX));
    float b = ((float)rand() / (RAND_MAX));
    color = glm::vec4(r, g, b, 1.0f);

    center = glm::vec3(0, 1, 0);

    Vertex* first_vert = new Vertex(k, reversePoincare(circleRadius(n, k), 0));
    vertices.push_back(first_vert);
    Edge* first_edge = first_vert->edges.at(0);
    
    Edge* edge = first_edge;
    edge->addTile(this);

    std::cout << "ORIGIN TILE" << std::endl;
    for (int i = 1; i < n; i++) {
        Vertex* next_vert = edge->vertex2;
        glm::vec3 next_loc = rotate(vertices.at(i - 1)->getPos(), 2 * M_PI / n);
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

    std::cout << "NON ORIGIN TILE" << std::endl;

    Edge* back_edge = back_vert->next(e);
    while (back_vert != front_vert && !back_edge->hasDangling()) {
        back_edge->addTile(this);
        back_vert = back_edge->verts(center).at(0);
        vertices.insert(vertices.begin(), back_vert);
        back_edge = back_vert->next(back_edge);
    }

    // Made a loop; all vertices accounted for
    if (back_vert == front_vert)
        vertices.erase(vertices.begin());
    else { // Need to complete the vertices
        Edge* edge = front_vert->prev(e);

        Edge* ref_edge = e->verts(ref->center).at(1)->prev(e);
        edge->addTile(this);

        Vertex* vertex;
        Vertex* reflecting_vertex;
        glm::vec3 midpt = midpoint(e->vertex1->getPos(), e->vertex2->getPos());

        int size = vertices.size();
        for (int i = size; i < n; i++) {
            reflecting_vertex = ref_edge->verts(ref->center).at(1);

            glm::vec3 next_loc = extend(reflecting_vertex->getPos(), midpt);
            if (edge->hasDangling()) {
                vertex = edge->vertex2;
                vertex->clamp(next_loc);
            } else {
                vertex = edge->verts(center).at(1);
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
    Vertex* v1 = vertices.at(0);
    Vertex* v2 = vertices.at(1);
    Edge* sweep = v1->seekVertex(v2);
    edges.push_back(sweep);
    Vertex* front = sweep->verts(center).at(1);

    for (int i = 0; i < n - 1; i++) {
        sweep = front->prev(sweep);
        edges.push_back(sweep);
        front = sweep->verts(center).at(1);
    }
}

int Tile::findEdge(Edge* e) {
    auto it = std::find(edges.begin(), edges.end(), e);
    assert(it != edges.end());
    return it - edges.begin();
}

void Tile::setVertexLocs(Tile* ref, Edge* e) {
    center = extend(ref->center, midpoint(e->vertex1->getPos(), e->vertex2->getPos()));

    Vertex* vertex = e->verts(center).at(1);
    Edge* edge = vertex->prev(e);

    Edge* ref_edge = e->verts(ref->center).at(1)->prev(e);
    Vertex* reflecting_vertex;

    glm::vec3 midpt = midpoint(e->vertex1->getPos(), e->vertex2->getPos());

    for (int i = 0; i < n-2; i++) {
        // ccw vertex ordering breaks down when the vertex locations are inaccurate.
        // Need to use prev_vertex and rely on comparing vertex1 and vertex2 instead of using verts().
        vertex = (vertex == edge->vertex1) ? edge->vertex2 : edge->vertex1;
        reflecting_vertex = ref_edge->verts(ref->center).at(1);
        
        glm::vec3 next_loc = extend(reflecting_vertex->getPos(), midpt);
        vertex->setPos(next_loc);

        edge = vertex->prev(edge);
        ref_edge = reflecting_vertex->prev(ref_edge);
    }
}

void Tile::expand(bool create) {
    for (Edge* e : edges) {
        Tile* other_tile = NULL;
        if (e->tiles.size() < 2) {
            if (create) {
                other_tile = new Tile(this, e, n, k);
                all.push_back(other_tile);
            }
        } else {
            assert(e->tiles.size() == 2);
            other_tile = (this == e->tiles.at(0)) ? e->tiles.at(1) : e->tiles.at(0);
        }
        if (other_tile && !other_tile->isVisible()) {
            next.push_back(other_tile);
            visible.push_back(other_tile);
            other_tile->setVertexLocs(this, e);
        }
    }
}

void Tile::setStart(glm::vec3 relPos) {
    // cosh(dist. between two centers) = golden ratio
    // dist = 2 * 0.5306375309525178260165094581067867429033927494693168481986051407
    // sinh(0.5306) * cosh(z) = sinh(z)
    // z = 0.6268696629061778141444633762119364014776097856510327417726257885

    // Order 6: log(2 + sqrt(3)) = 1.3169578969248167086250463473079684440269819714675164797684722569
    // cosh(dist. between two centers) = sqrt(1.5)
    //   Somehow when I looked up 1.22474487139 (which is approx. sqrt(1.5)),
    //   most of the search results were for a game called Nier Replicant.

    // Order 7: 1.4490747226775863350321731432577267824696315854162935178461711615122500380225563
    // cosh(dist. between two centers): https://qr.ae/pGuGKB

    // RULDR on (0,1,0) gives (sc^2 - sc^3 - 2cs^3 + sc^4,   cs^2 - 3s^2*c^2 + c^5,   sc^2 + s^3 - sc^3)

    // TODO: Figure out derivation of circleRadius (circ_rad)
    
    vertices.at(0)->setPos(rotate(reversePoincare(circleRadius(n, k), 0), angle));
    for (int i = 1; i < n; i++)
        vertices.at(i)->setPos(rotate(vertices.at(i - 1)->getPos(), 2 * M_PI / n));

    for (int i = 0; i < n; i++)
        vertices.at(i)->setPos(translateXZ(vertices.at(i)->getPos(), relPos.x, relPos.z));

    center = translateXZ(glm::vec3(0, 1, 0), relPos.x, relPos.z);
    /*
    TR = vertices.at(0)->getPos();
    TL = vertices.at(1)->getPos();
    BL = vertices.at(2)->getPos();
    BR = vertices.at(3)->getPos();
    */

    std::vector<Tile*> copy;

    next.clear();
    next.push_back(this);

    visible.clear();
    visible.push_back(this);

    int depth = 2;
    for (int i = 0; i <= depth; i++) {
        copy.clear();
        for (Tile* t : next)
            copy.push_back(t);
        next.clear();

        bool create = (i <= depth-1);
        for (Tile* t : copy)
            t->expand(create);
    }

    /* // This is for marking tiles to be generated
    if (!parent) {
        parents.push(this);
        parent = this;
        for (Tile* t : getNeighbors()) {
            if (!t->parent)
                t->parent = this;
        }
    }
    */
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