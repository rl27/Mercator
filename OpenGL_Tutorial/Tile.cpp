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
	float side = 1.191475;

	glm::vec3 og(0, 1, 0);
	glm::vec3 og_TR = translateX(translateZ(og, 0.626884), 0.530646);
	glm::vec3 og_TL = translateX(translateZ(og, 0.626884), -0.530646);
	glm::vec3 og_BR = translateX(translateZ(og, -0.626884), 0.530646);
	glm::vec3 og_BL = translateX(translateZ(og, -0.626884), -0.530646);

	this->center = translateX(translateZ(og, relPos.z), relPos.x);
	this->TL = translateX(translateZ(og_TL, relPos.z), relPos.x);;
	this->TR = translateX(translateZ(og_TR, relPos.z), relPos.x);;
	this->BL = translateX(translateZ(og_BL, relPos.z), relPos.x);;
	this->BR = translateX(translateZ(og_BR, relPos.z), relPos.x);;
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