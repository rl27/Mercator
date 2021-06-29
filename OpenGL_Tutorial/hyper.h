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
static glm::vec3 extend(glm::vec3 a, glm::vec3 b)
{
    float w = dist(a, b);
    glm::vec3 proj = hypNormalize(b - minkProjection(b, a));
    return a * cosh(2 * w) + proj * sinh(2 * w);
}

// Get Poincare projection from hyperboloid to y=0 plane
static glm::vec3 getPoincare(glm::vec3 v)
{
    float y1 = v.y + 1;
    return glm::vec3(v.x / y1, 0, v.z / y1);
}

#endif