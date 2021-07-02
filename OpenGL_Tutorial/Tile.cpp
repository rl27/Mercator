#include "Tile.h"

Tile::Tile(std::string n)
{
	name = n;
	float r = ((float)rand() / (RAND_MAX));
	float g = ((float)rand() / (RAND_MAX));
	float b = ((float)rand() / (RAND_MAX));
	color = glm::vec4(r, g, b, 1.0f);
}

void Tile::setStart(glm::vec3 relPos)
{
	/* NOT WORKING */
	/* NOT WORKING */
	/* NOT WORKING */

	float side = 1.191475;

	this->center = fromOrigin(relPos.x, relPos.z);
	this->TL = fromOrigin(relPos.x - side / 2, relPos.z + side / 2);
	this->TR = fromOrigin(relPos.x + side / 2, relPos.z + side / 2);
	this->BL = fromOrigin(relPos.x - side / 2, relPos.z - side / 2);
	this->BR = fromOrigin(relPos.x + side / 2, relPos.z - side / 2);
}

void Tile::setRight(Tile* R)
{
	Right = R;
	R->Left = this;

	R->center = extend(this->center, midpoint(this->TR, this->BR));
	R->TL = this->TR;
	R->BL = this->BR;
	R->TR = extend(R->BL, R->center);
	R->BR = extend(R->TL, R->center);
}

void Tile::setLeft(Tile* L)
{
	Left = L;
	L->Right = this;

	L->center = extend(this->center, midpoint(this->TL, this->BL));
	L->TR = this->TL;
	L->BR = this->BL;
	L->TL = extend(L->BR, L->center);
	L->BL = extend(L->TR, L->center);
}

void Tile::setUp(Tile* U)
{
	Up = U;
	U->Down = this;

	U->center = extend(this->center, midpoint(this->TL, this->TR));
	U->BL = this->TL;
	U->BR = this->TR;
	U->TL = extend(U->BR, U->center);
	U->TR = extend(U->BL, U->center);
}

void Tile::setDown(Tile* D)
{
	Down = D;
	D->Up = this;

	D->center = extend(this->center, midpoint(this->BL, this->BR));
	D->TL = this->BL;
	D->TR = this->BR;
	D->BL = extend(D->TR, D->center);
	D->BR = extend(D->TL, D->center);
}