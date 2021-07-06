#include "Tile.h"

Tile::Tile(std::string n)
{
    name = n;

    float r = ((float)rand() / (RAND_MAX));
    float g = ((float)rand() / (RAND_MAX));
    float b = ((float)rand() / (RAND_MAX));
    color = glm::vec4(r, g, b, 1.0f);

    Up = NULL;
    Right = NULL;
    Left = NULL;
    Down = NULL;

    center = glm::vec3(0);
    TL = glm::vec3(0);
    TR = glm::vec3(0);
    BL = glm::vec3(0);
    BR = glm::vec3(0);

    num = 0;
}

void Tile::expand()
{
    /*
    * DOESN'T CHECK FOR DUPLICATES YET
    */

    // Add to list of seen tiles
    tiles.push_back(this);

    // Return if tile is too far away from origin
    if (num > 2)
        return;

    if (Up == NULL)
    {
        Up = new Tile(name + 'U');
        Up->Down = this;
    }
    if (Right == NULL)
    {
        Right = new Tile(name + 'R');
        Right->Left = this;
    }
    if (Down == NULL)
    {
        Down = new Tile(name + 'D');
        Down->Up = this;
    }
    if (Left == NULL)
    {
        Left = new Tile(name + 'L');
        Left->Right = this;
    }

    if (!Up->inTiles())
    {
        setUp(Up);
        Up->expand();
    }
    if (!Right->inTiles())
    {
        setRight(Right);
        Right->expand();
    }
    if (!Down->inTiles())
    {
        setDown(Down);
        Down->expand();
    }
    if (!Left->inTiles())
    {
        setLeft(Left);
        Left->expand();
    }
}

void Tile::setStart(glm::vec3 relPos)
{
    num = 0;

    // cosh(dist. between two centers) = golden ratio
    // dist = 2 * 0.5306375309525178260165094581067867429033927494693168481986051407
    // sinh(0.5306) * cosh(z) = sinh(z)
    // z = 0.6268696629061778141444633762119364014776097856510327417726257885

    glm::vec3 og(0, 1, 0);
    glm::vec3 og_TR = translateX(translateZ(og, 0.6268697), 0.5306375);
    glm::vec3 og_TL = translateX(translateZ(og, 0.6268697), -0.5306375);
    glm::vec3 og_BR = translateX(translateZ(og, -0.6268697), 0.5306375);
    glm::vec3 og_BL = translateX(translateZ(og, -0.6268697), -0.5306375);

    this->center = translateX(translateZ(og, relPos.z), relPos.x);
    this->TL = translateX(translateZ(og_TL, relPos.z), relPos.x);
    this->TR = translateX(translateZ(og_TR, relPos.z), relPos.x);
    this->BL = translateX(translateZ(og_BL, relPos.z), relPos.x);
    this->BR = translateX(translateZ(og_BR, relPos.z), relPos.x);

    tiles.clear();
    this->expand();
}

void Tile::setRight(Tile* R)
{
    R->center = extend(this->center, midpoint(this->TR, this->BR));
    R->TL = this->TR;
    R->BL = this->BR;
    R->TR = extend(R->BL, R->center);
    R->BR = extend(R->TL, R->center);
    R->num = num + 1;
}

void Tile::setLeft(Tile* L)
{
    L->center = extend(this->center, midpoint(this->TL, this->BL));
    L->TR = this->TL;
    L->BR = this->BL;
    L->TL = extend(L->BR, L->center);
    L->BL = extend(L->TR, L->center);
    L->num = num + 1;
}

void Tile::setUp(Tile* U)
{
    U->center = extend(this->center, midpoint(this->TL, this->TR));
    U->BL = this->TL;
    U->BR = this->TR;
    U->TL = extend(U->BR, U->center);
    U->TR = extend(U->BL, U->center);
    U->num = num + 1;
}

void Tile::setDown(Tile* D)
{
    D->center = extend(this->center, midpoint(this->BL, this->BR));
    D->TL = this->BL;
    D->TR = this->BR;
    D->BL = extend(D->TR, D->center);
    D->BR = extend(D->TL, D->center);
    D->num = num + 1;
}

bool Tile::inTiles()
{
    if (std::find(tiles.begin(), tiles.end(), this) != tiles.end())
        return true;
    return false;
}