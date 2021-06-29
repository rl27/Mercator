#ifndef TILE_H
#define TILE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "hyper.h"
#include <string>

class Tile
{
public:
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

	Tile(std::string n);

	// Set starting tile based on relative position to its center
	void setStart(glm::vec3 rel);

	// Set neighboring tiles
	void setRight(Tile* R);
	void setLeft(Tile* L);
	void setUp(Tile* U);
	void setDown(Tile* D);
};

#endif