#include "Vertex.h"

Vertex::Vertex(int k) : k(k) {
	initialized = false;
	pos = glm::vec3(0);
}

Vertex::Vertex(int k, glm::vec3 loc): k(k) {
	initialized = false;
	clamp(loc);
}

void Vertex::setPos(glm::vec3 loc) {
	pos = loc;
}

glm::vec3 Vertex::getPos() {
	return pos;
}

void Vertex::addEdge(Edge* e) {
	edges.push_back(e);
}

Edge* Vertex::next(Edge* e) {
	int idx = seekEdge(e);
	return edges.at((idx + 1) % k);
}

Edge* Vertex::prev(Edge* e) {
	int idx = seekEdge(e);
	return edges.at((idx - 1 + k) % k);
}

void Vertex::clamp(glm::vec3 loc) {
	assert(!initialized);
	initialized = true;
	pos = loc;
	int start = edges.size();
	for (int i = start; i < k; i++) {
		Vertex* loose_vert = new Vertex(k);
		Edge* loose_edge = new Edge(this, loose_vert);
	}
}

void Vertex::replaceEdge(Edge* oldEdge, Edge* newEdge) {
	int idx = seekEdge(oldEdge);
	edges.at(idx) = newEdge;
	delete oldEdge;
}

int Vertex::seekEdge(Edge* e) {
	auto it = std::find(edges.begin(), edges.end(), e);
	assert(it != edges.end());
	return it - edges.begin();
}

Edge* Vertex::seekVertex(Vertex* v) {
	for (Edge* e : edges) {
		if (e->vertex1 == v || e->vertex2 == v)
			return e;
	}
	assert(false);
}

/*********************************************************************/

Edge::Edge(Vertex* v1, Vertex* v2) : vertex1(v1), vertex2(v2) {
	v1->addEdge(this);
	v2->addEdge(this);
}

std::vector<Vertex*> Edge::verts() {
	std::vector<Vertex*> v{ vertex1, vertex2 };
	return v;
}

void Edge::addTile(Tile* t) {
	if (tiles.size() == 2)
		std::cout << "2 TILES!" << std::endl;
	if (std::find(tiles.begin(), tiles.end(), t) == tiles.end())
		tiles.push_back(t);
	else
		std::cout << "DUPLICATE TILE" << std::endl;
}

std::vector<Vertex*> Edge::verts(glm::vec3 center) {
	std::vector<Vertex*> verts;

	glm::vec3 v1 = getPoincare(vertex1->getPos()) - getPoincare(center);
	glm::vec3 v2 = getPoincare(vertex2->getPos()) - getPoincare(center);
	float rad1 = atan2(v1[2], v1[0]);
	float rad2 = atan2(v2[2], v2[0]);

	float angle = fmod(rad2 - rad1 + 2 * M_PI, 2 * M_PI);

	if (angle > M_PI) {
		std::vector<Vertex*> v{ vertex2, vertex1 };
		return v;
	} else {
		std::vector<Vertex*> v{ vertex1, vertex2 };
		return v;
	}
}

bool Edge::hasDangling() {
	return (!vertex1->initialized) || (!vertex2->initialized);
}

void Edge::merge(Edge* e) {
	assert(this->hasDangling() && e->hasDangling());
	Vertex* old_dangling;
	Vertex* other_dangling;
	if (!vertex1->initialized) {
		old_dangling = vertex1;
		if (!e->vertex1->initialized) {
			vertex1 = e->vertex2;
			other_dangling = e->vertex1;
		} else {
			vertex1 = e->vertex1;
			other_dangling = e->vertex2;
		}
		vertex1->replaceEdge(e, this);
	}
	else {
		old_dangling = vertex2;
		if (!e->vertex1->initialized) {
			vertex2 = e->vertex2;
			other_dangling = e->vertex1;
		} else {
			vertex2 = e->vertex1;
			other_dangling = e->vertex2;
		}
		vertex2->replaceEdge(e, this);
	}
	delete old_dangling;
	delete other_dangling;
}