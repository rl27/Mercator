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

    texture = -1;
}

void Tile::expand(bool create)
{
    /*
    * DOESN'T CHECK FOR DUPLICATES YET
    */

    // Add to list of seen tiles
    //tiles.push_back(this);

    // Return if tile is too far away from origin
    //if (num > 2)
    //    return;

    if (create)
    {
        if (Up == NULL)
        {
            Up = new Tile(name + 'U');
            Up->Down = this;
            created.push_back(Up);
            setUp(Up);
            Up->connectInTiles();
        }
        if (Right == NULL)
        {
            Right = new Tile(name + 'R');
            Right->Left = this;
            created.push_back(Right);
            setRight(Right);
            Right->connectInTiles();
        }
        if (Down == NULL)
        {
            Down = new Tile(name + 'D');
            Down->Up = this;
            created.push_back(Down);
            setDown(Down);
            Down->connectInTiles();
        }
        if (Left == NULL)
        {
            Left = new Tile(name + 'L');
            Left->Right = this;
            created.push_back(Left);
            setLeft(Left);
            Left->connectInTiles();
        }
    }

    if (Up && !Up->inTiles())
    {
        setUp(Up);
        next.push_back(Up);
        tiles.push_back(Up);
        //Up->expand();
    }
    if (Right && !Right->inTiles())
    {
        setRight(Right);
        next.push_back(Right);
        tiles.push_back(Right);
        //Right->expand();
    }
    if (Down && !Down->inTiles())
    {
        setDown(Down);
        next.push_back(Down);
        tiles.push_back(Down);
        //Down->expand();
    }
    if (Left && !Left->inTiles())
    {
        setLeft(Left);
        next.push_back(Left);
        tiles.push_back(Left);
        //Left->expand();
    }
}

void Tile::setStart(glm::vec3 relPos)
{
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

    /*this->center = translateXZ(og, relPos.x, relPos.z);
    this->TL = translateXZ(og_TL, relPos.x, relPos.z);
    this->TR = translateXZ(og_TR, relPos.x, relPos.z);
    this->BL = translateXZ(og_BL, relPos.x, relPos.z);
    this->BR = translateXZ(og_BR, relPos.x, relPos.z);*/

    std::vector<Tile*> copy;

    next.clear();
    next.push_back(this);

    tiles.clear();
    tiles.push_back(this);

    created.clear();

    int breadth = 6;
    for (int i = 0; i <= breadth; i++)
    {
        copy.clear();
        for (Tile* t : next)
            copy.push_back(t);
        next.clear();
        created.clear();
        if (i <= breadth-4)
        {
            for (Tile* t : copy)
                t->expand(true);
        }
        else
        {
            for (Tile* t : copy)
                t->expand(false);
        }

        for (Tile* t : created)
        {
            t->connectInTiles();
        }
    }
}

void Tile::setRight(Tile* t)
{
    t->center = getRight();

    if (t->Left && t->Left == this)
    {
        t->TL = this->TR;
        t->BL = this->BR;
        t->TR = extend(t->BL, t->center);
        t->BR = extend(t->TL, t->center);
    }
    if (t->Down && t->Down == this)
    {
        t->BL = this->TR;
        t->BR = this->BR;
        t->TR = extend(t->BL, t->center);
        t->TL = extend(t->BR, t->center);
    }
    if (t->Up && t->Up == this)
    {
        t->TR = this->TR;
        t->TL = this->BR;
        t->BR = extend(t->TL, t->center);
        t->BL = extend(t->TR, t->center);
    }
    if (t->Right && t->Right == this)
    {
        t->BR = this->TR;
        t->TR = this->BR;
        t->TL = extend(t->BR, t->center);
        t->BL = extend(t->TR, t->center);
    }
}

void Tile::setLeft(Tile* t)
{
    t->center = getLeft();

    if (t->Left && t->Left == this)
    {
        t->TL = this->BL;
        t->BL = this->TL;
        t->TR = extend(t->BL, t->center);
        t->BR = extend(t->TL, t->center);
    }
    if (t->Down && t->Down == this)
    {
        t->BL = this->BL;
        t->BR = this->TL;
        t->TR = extend(t->BL, t->center);
        t->TL = extend(t->BR, t->center);
    }
    if (t->Up && t->Up == this)
    {
        t->TR = this->BL;
        t->TL = this->TL;
        t->BR = extend(t->TL, t->center);
        t->BL = extend(t->TR, t->center);
    }
    if (t->Right && t->Right == this)
    {
        t->BR = this->BL;
        t->TR = this->TL;
        t->TL = extend(t->BR, t->center);
        t->BL = extend(t->TR, t->center);
    }
}

void Tile::setUp(Tile* t)
{
    t->center = getUp();

    if (t->Left && t->Left == this)
    {
        t->TL = this->TL;
        t->BL = this->TR;
        t->TR = extend(t->BL, t->center);
        t->BR = extend(t->TL, t->center);
    }
    if (t->Down && t->Down == this)
    {
        t->BL = this->TL;
        t->BR = this->TR;
        t->TR = extend(t->BL, t->center);
        t->TL = extend(t->BR, t->center);
    }
    if (t->Up && t->Up == this)
    {
        t->TR = this->TL;
        t->TL = this->TR;
        t->BR = extend(t->TL, t->center);
        t->BL = extend(t->TR, t->center);
    }
    if (t->Right && t->Right == this)
    {
        t->BR = this->TL;
        t->TR = this->TR;
        t->TL = extend(t->BR, t->center);
        t->BL = extend(t->TR, t->center);
    }
}

void Tile::setDown(Tile* t)
{
    t->center = getDown();

    if (t->Left && t->Left == this)
    {
        t->TL = this->BR;
        t->BL = this->BL;
        t->TR = extend(t->BL, t->center);
        t->BR = extend(t->TL, t->center);
    }
    if (t->Down && t->Down == this)
    {
        t->BL = this->BR;
        t->BR = this->BL;
        t->TR = extend(t->BL, t->center);
        t->TL = extend(t->BR, t->center);
    }
    if (t->Up && t->Up == this)
    {
        t->TR = this->BR;
        t->TL = this->BL;
        t->BR = extend(t->TL, t->center);
        t->BL = extend(t->TR, t->center);
    }
    if (t->Right && t->Right == this)
    {
        t->BR = this->BR;
        t->TR = this->BL;
        t->TL = extend(t->BR, t->center);
        t->BL = extend(t->TR, t->center);
    }
}

glm::vec3 Tile::getRight()
{
    return extend(center, midpoint(TR, BR));
}
glm::vec3 Tile::getLeft()
{
    return extend(center, midpoint(TL, BL));
}
glm::vec3 Tile::getUp()
{
    return extend(center, midpoint(TL, TR));
}
glm::vec3 Tile::getDown()
{
    return extend(center, midpoint(BL, BR));
}

bool Tile::inTiles()
{
    if (std::find(tiles.begin(), tiles.end(), this) != tiles.end())
        return true;
    return false;
}

void Tile::connectInTiles()
{
    if (!Up)
    {
        for (Tile* t2 : tiles)
        {
            if (close(getUp(), t2->center))
            {
                if (!t2->Up && close(center, t2->getUp()))
                    t2->Up = this;
                if (!t2->Right && close(center, t2->getRight()))
                    t2->Right = this;
                if (!t2->Left && close(center, t2->getLeft()))
                    t2->Left = this;
                if (!t2->Down && close(center, t2->getDown()))
                    t2->Down = this;
                Up = t2;
                break;
            }
        }
    }
    if (!Down)
    {
        for (Tile* t2 : tiles)
        {
            if (close(getDown(), t2->center))
            {
                if (!t2->Up && close(center, t2->getUp()))
                    t2->Up = this;
                if (!t2->Right && close(center, t2->getRight()))
                    t2->Right = this;
                if (!t2->Left && close(center, t2->getLeft()))
                    t2->Left = this;
                if (!t2->Down && close(center, t2->getDown()))
                    t2->Down = this;
                Down = t2;
                break;
            }
        }
    }
    if (!Right)
    {
        for (Tile* t2 : tiles)
        {
            if (close(getRight(), t2->center))
            {
                if (!t2->Up && close(center, t2->getUp()))
                    t2->Up = this;
                if (!t2->Right && close(center, t2->getRight()))
                    t2->Right = this;
                if (!t2->Left && close(center, t2->getLeft()))
                    t2->Left = this;
                if (!t2->Down && close(center, t2->getDown()))
                    t2->Down = this;
                Right = t2;
                break;
            }
        }
    }
    if (!Left)
    {
        for (Tile* t2 : tiles)
        {
            if (close(getLeft(), t2->center))
            {
                if (!t2->Up && close(center, t2->getUp()))
                    t2->Up = this;
                if (!t2->Right && close(center, t2->getRight()))
                    t2->Right = this;
                if (!t2->Left && close(center, t2->getLeft()))
                    t2->Left = this;
                if (!t2->Down && close(center, t2->getDown()))
                    t2->Down = this;
                Left = t2;
                break;
            }
        }
    }
}