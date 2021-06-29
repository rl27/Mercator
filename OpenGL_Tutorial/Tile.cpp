#include "Tile.h"

Tile::Tile(std::string n)
{
	name = n;
}

void Tile::setStart(glm::vec3 rel)
{
	float y = cosh(sqrt(2) / 2);
	float r = sqrt(pow(y, 2) - 1);
	float theta = M_PI / 4;
	float x = r * cos(theta);
	float z = r * sin(theta);
	this->center = glm::vec3(0, 1, 0);
	this->TL = glm::vec3(-x, y, z);
	this->TR = glm::vec3(x, y, z);
	this->BL = glm::vec3(-x, y, -z);
	this->BR = glm::vec3(x, y, -z);
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