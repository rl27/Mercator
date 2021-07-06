#ifndef HYPER_H
#define HYPER_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>

// Evaluate on hyperboloid
static float hypEval(glm::vec3 v)
{
    return v[1] * v[1] - v[0] * v[0] - v[2] * v[2];
}

// Minkowski dot product
static float minkDot(glm::vec3 a, glm::vec3 b)
{
    // return (hypEval(a+b) - hypEval(a) - hypEval(b))/2
    return a[1] * b[1] - a[0] * b[0] - a[2] * b[2];
}

// Normalize to hyperboloid
static glm::vec3 hypNormalize(glm::vec3 v)
{
    return v / sqrt(abs(hypEval(v)));
}

// Hyperbolic midpoint of a and b
static glm::vec3 midpoint(glm::vec3 a, glm::vec3 b)
{
    return hypNormalize((a + b) / 2.0f);
}

// Minkowski distance between a and b
// https://en.wikipedia.org/wiki/Hyperboloid_model#Minkowski_quadratic_form
static float dist(glm::vec3 a, glm::vec3 b)
{
    return acosh(minkDot(a, b));
}

// Minkowski projection of a onto b
static glm::vec3 minkProjection(glm::vec3 a, glm::vec3 b)
{
    return b * (minkDot(a, b) / minkDot(b, b));
}

// Extend a through b to c, such that the midpoint of a and c is b.
// https://en.wikipedia.org/wiki/Hyperboloid_model#Straight_lines
static glm::vec3 extend(glm::vec3 a, glm::vec3 b)
{
    float w = dist(a, b);
    glm::vec3 proj = hypNormalize(b - minkProjection(b, a));
    return a * cosh(2 * w) + proj * sinh(2 * w);
}

// Get Poincare projection from hyperboloid to (0,-1,0)
static glm::vec3 getPoincare(glm::vec3 v)
{
    float y1 = v.y + 1;
    return glm::vec3(v.x / y1, 0, v.z / y1);
}

// Get Beltrami-Klein projection from hyperboloid to (0,0,0)
static glm::vec3 getBeltrami(glm::vec3 v)
{
    return glm::vec3(v.x / v.y, 0, v.z / v.y);
}

/*******************
* Note on translations: do in reverse order. I.e. to get RUL, do left -> up -> right translations.
*/

// Translate vector in x-direction
static glm::vec3 translateX(glm::vec3 v, float dist)
{
    float co = cosh(dist);
    float si = sinh(dist);
    return glm::vec3(co * v.x + si * v.y, si * v.x + co * v.y, v.z);
}

// Translate vector in z-direction
static glm::vec3 translateZ(glm::vec3 v, float dist)
{
    float co = cosh(dist);
    float si = sinh(dist);
    return glm::vec3(v.x, si * v.z + co * v.y, co * v.z + si * v.y);
}

// Get hyperboloid coordinates from x/z pair
static glm::vec3 fromOrigin(float x, float z)
{
    if (x == 0 && z == 0)
        return glm::vec3(0, 1, 0);

    float dist = sqrt(pow(x, 2.0f) + pow(z, 2.0f));
    float y = cosh(dist);
    float ratio = sqrt((pow(y, 2) - 1) / (pow(dist, 2)));

    return glm::vec3(ratio * x, y, ratio * z);
}

// Check if two coords are very close
static bool close(glm::vec3 a, glm::vec3 b)
{
    float dist = sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2) + pow(a[2] - b[2], 2));
    if (dist < 0.3)
        return true;
    return false;
}

#endif