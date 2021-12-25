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
    return hypNormalize((a + b) / 2.0f);  // This "/ 2.0f" isn't necessary
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

// Form line from a to b and return c, such that the dist from a to c is d.
static glm::vec3 line(glm::vec3 a, glm::vec3 b, float d)
{
    glm::vec3 proj = hypNormalize(b - minkProjection(b, a));
    return a * cosh(d) + proj * sinh(d);
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
********************/

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

// To find a symmetric translation, from (0,1,0), find x->z, and find z->x, for any variable x,z.
// Since one value must be modified, let it be f(x) or f(z). Then set the values 
// equal to each other and solve.
// 
// Previous symmetric translation that modified the second value:
//     v = translateX(v, xdist);
//     v = translateZ(v, asinh(sinh(zdist)/cosh(xdist)));
// 
// Symmetric translation that modifies first value:
//   sinh(f(x)) = sinh(x) * cosh(asinh(sinh(z) * cosh(f(x))))
//              = sinh(x) * sqrt( 1 + (sinh(z) * cosh(f(x)))^2 )
// Note that cosh(asinh(w)) = sqrt(1 + w^2)
// Solve to get f(x) = acosh(sqrt((1+a)/(1-ab))), where a = (sinhx)^2  and  b = (sinhz)^2

// Translate vector in both directions
static glm::vec3 translateXZ(glm::vec3 v, float xdist, float zdist)
{
    float a = pow(sinh(xdist), 2.0f);
    float b = pow(sinh(zdist), 2.0f);
    float fx = acosh(sqrt((1 + a) / (1 - a*b)));
    if (xdist > 0)
        v = translateX(v, fx);
    else
        v = translateX(v, -fx);
    v = translateZ(v, zdist);
    return v;
}

// Reverse translateXZ
static glm::vec3 reverseXZ(glm::vec3 v, float xdist, float zdist)
{
    float a = pow(sinh(xdist), 2.0f);
    float b = pow(sinh(zdist), 2.0f);
    float fx = acosh(sqrt((1 + a) / (1 - a * b)));
    v = translateZ(v, -zdist);
    if (xdist > 0)
        v = translateX(v, -fx);
    else
        v = translateX(v, fx);
    return v;
}

// Get x and z for translateXZ from vector v. translateXZ(origin, x, z) will give v.
static glm::vec3 getXZ(glm::vec3 v)
{
    float fx = asinh(v.x);
    float zdist = asinh(v.z / cosh(fx));

    float left = pow(cosh(fx), 2.0f);
    float a = (left - 1) / (1 + left * pow(sinh(zdist), 2.0f));
    float xdist = 0;
    if (fx > 0)
        xdist = asinh(sqrt(a));
    else
        xdist = -asinh(sqrt(a));

    return glm::vec3(xdist, 0, zdist);
}

// XZ translation that preserves x and z. I.e. translating x and z from the origin gets (x, _, z).
static glm::vec3 translateXZ2(glm::vec3 v, float x, float z)
{
    float xdist = asinh(x);
    v = translateX(v, xdist);
    float zdist = asinh(z / cosh(xdist));
    v = translateZ(v, zdist);
    return v;
}

// Reverse of XZ2. Will reverse a translateXZ2 call given the same x and z.
static glm::vec3 reverseXZ2(glm::vec3 v, float x, float z)
{
    float xdist = asinh(x);
    float zdist = asinh(z / cosh(xdist));
    v = translateZ(v, -zdist);
    v = translateX(v, -xdist);
    return v;
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
    float dist = pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2) + pow(a[2] - b[2], 2);
    if (dist < 0.1)
        return true;
    return false;
}

// Counter-clockwise rotation, preserving y
static glm::vec3 rotate(glm::vec3 v, float angle)
{
    return glm::vec3(v.x * cos(angle) - v.z * sin(angle), v.y, v.x * sin(angle) + v.z * cos(angle));
}

#endif