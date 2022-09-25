#if 0

#include "Edge.h"

Edge::Edge(Vertex* v1, Vertex* v2) : vertex1(v1), vertex2(v2)
{
	tile1 = NULL;
	tile2 = NULL;
}

std::vector<Vertex*> Edge::verts()
{
	std::vector<Vertex*> v{ vertex1, vertex2 };
	return v;
}

std::vector<Vertex*> Edge::verts(glm::vec3 center)
{
	std::vector<Vertex*> verts;

	glm::vec3 v1 = getPoincare(vertex1->getPos()) - getPoincare(center);
	glm::vec3 v2 = getPoincare(vertex2->getPos()) - getPoincare(center);
	float rad1 = atan2(v1[2], v1[0]);
	float rad2 = atan2(v2[2], v2[0]);

	float angle = fmod(rad2 - rad1 + 2 * M_PI, 2 * M_PI);

	if (angle > M_PI)
	{
		std::vector<Vertex*> v{ vertex2, vertex1 };
		return v;
	}
	else
	{
		std::vector<Vertex*> v{ vertex1, vertex2 };
		return v;
	}
}

#endif