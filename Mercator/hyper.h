#ifndef HYPER_H
#define HYPER_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>

// Evaluate on hyperboloid
static double hypEval(glm::dvec3 v)
{
    return v[1] * v[1] - v[0] * v[0] - v[2] * v[2];
}

// Minkowski dot product
static double minkDot(glm::dvec3 a, glm::dvec3 b)
{
    // return (hypEval(a+b) - hypEval(a) - hypEval(b))/2
    return a[1] * b[1] - a[0] * b[0] - a[2] * b[2];
}

// Normalize to hyperboloid
static glm::dvec3 hypNormalize(glm::dvec3 v)
{
    return v / sqrt(abs(hypEval(v)));
}

// Hyperbolic midpoint of a and b
static glm::dvec3 midpoint(glm::dvec3 a, glm::dvec3 b)
{
    return hypNormalize((a + b) / 2.0);  // This "/ 2.0" isn't necessary
}

// Minkowski distance between a and b
// https://en.wikipedia.org/wiki/Hyperboloid_model#Minkowski_quadratic_form
static double dist(glm::dvec3 a, glm::dvec3 b)
{
    return acosh(minkDot(a, b));
}

// Minkowski projection of a onto b
static glm::dvec3 minkProjection(glm::dvec3 a, glm::dvec3 b)
{
    return b * (minkDot(a, b) / minkDot(b, b));
}

// Extend a through b to c, such that the midpoint of a and c is b.
// https://en.wikipedia.org/wiki/Hyperboloid_model#Straight_lines
static glm::dvec3 extend(glm::dvec3 a, glm::dvec3 b)
{
    double w = dist(a, b);
    glm::dvec3 proj = hypNormalize(b - minkProjection(b, a));
    return a * cosh(2 * w) + proj * sinh(2 * w);
}

// Form line from a to b and return c, such that the dist from a to c is d.
static glm::dvec3 line(glm::dvec3 a, glm::dvec3 b, double d)
{
    glm::dvec3 proj = hypNormalize(b - minkProjection(b, a));
    return a * cosh(d) + proj * sinh(d);
}

// Given x and two points p and q, construct point y such that dist(x,p) = dist(y,q) and vice versa.
// Essentially "reflects" across the line passing orthogonally through the midpoint of pq. Parallel?
static glm::dvec3 symmetry(glm::dvec3 x, glm::dvec3 p, glm::dvec3 q)
{
    glm::dvec3 u = (p - q) / sqrt(-hypEval(p - q));
    return x - 2 * (-minkDot(x, u)) * u;
}

// Get Poincare projection from hyperboloid to (0,-1,0)
static glm::dvec3 getPoincare(glm::dvec3 v)
{
    double y1 = v.y + 1;
    return glm::dvec3(v.x / y1, 0, v.z / y1);
}

// Project from (0,-1,0) to hyperboloid through a point (a,b) within the unit circle.
// Derivation: y^2 = 1 + x^2 + z^2, y = x/a - 1, y = z/c - 1, sub x and z in the first equation.
static glm::dvec3 reversePoincare(double a, double b)
{
    double d = pow(a, 2.0f) + pow(b, 2.0f);
    assert(d < 1);
    double y = (1 + d) / (1 - d);
    return glm::dvec3(a * (y + 1), y, b * (y + 1));
}

// Get Beltrami-Klein projection from hyperboloid to (0,0,0)
static glm::dvec3 getBeltrami(glm::dvec3 v)
{
    return glm::dvec3(v.x / v.y, 0, v.z / v.y);
}

// Euclidean radius of circumscribed circle, given sides per polygon and polygons per vertex.
static double circleRadius(int n, int k)
{
    return sqrt((tan(M_PI/2 - M_PI/k) - tan(M_PI/n)) / (tan(M_PI/2 - M_PI/k) +  tan(M_PI/n)));
}

/*******************
* Note on translations: do in reverse order. I.e. to get RUL, do left -> up -> right translations.
********************/

// Translate vector in x-direction
static glm::dvec3 translateX(glm::dvec3 v, double dist)
{
    double co = cosh(dist);
    double si = sinh(dist);
    return glm::dvec3(co * v.x + si * v.y, si * v.x + co * v.y, v.z);
}

// Translate vector in z-direction
static glm::dvec3 translateZ(glm::dvec3 v, double dist)
{
    double co = cosh(dist);
    double si = sinh(dist);
    return glm::dvec3(v.x, si * v.z + co * v.y, co * v.z + si * v.y);
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
static glm::dvec3 translateXZ(glm::dvec3 v, double xdist, double zdist)
{
    double a = pow(sinh(xdist), 2.0f);
    double b = pow(sinh(zdist), 2.0f);
    double fx = acosh(sqrt((1 + a) / (1 - a*b)));
    if (xdist > 0)
        v = translateX(v, fx);
    else
        v = translateX(v, -fx);
    v = translateZ(v, zdist);
    return v;
}

// Reverse translateXZ
static glm::dvec3 reverseXZ(glm::dvec3 v, double xdist, double zdist)
{
    double a = pow(sinh(xdist), 2.0f);
    double b = pow(sinh(zdist), 2.0f);
    double fx = acosh(sqrt((1 + a) / (1 - a * b)));
    v = translateZ(v, -zdist);
    if (xdist > 0)
        v = translateX(v, -fx);
    else
        v = translateX(v, fx);
    return v;
}

// Get x and z for translateXZ from vector v. translateXZ(origin, x, z) will give v.
static glm::dvec3 getXZ(glm::dvec3 v)
{
    double fx = asinh(v.x);
    double zdist = asinh(v.z / cosh(fx));

    double left = pow(cosh(fx), 2.0f);
    double a = (left - 1) / (1 + left * pow(sinh(zdist), 2.0f));
    double xdist = 0;
    if (fx > 0)
        xdist = asinh(sqrt(a));
    else
        xdist = -asinh(sqrt(a));

    return glm::dvec3(xdist, 0, zdist);
}

// XZ translation that preserves x and z. I.e. translating x and z from the origin gets (x, _, z).
static glm::dvec3 translateXZ2(glm::dvec3 v, double x, double z)
{
    double xdist = asinh(x);
    v = translateX(v, xdist);
    double zdist = asinh(z / cosh(xdist));
    v = translateZ(v, zdist);
    return v;
}

// Reverse of XZ2. Will reverse a translateXZ2 call given the same x and z.
static glm::dvec3 reverseXZ2(glm::dvec3 v, double x, double z)
{
    double xdist = asinh(x);
    double zdist = asinh(z / cosh(xdist));
    v = translateZ(v, -zdist);
    v = translateX(v, -xdist);
    return v;
}

// Get hyperboloid coordinates from x/z pair
static glm::dvec3 fromOrigin(double x, double z)
{
    if (x == 0 && z == 0)
        return glm::dvec3(0, 1, 0);

    double dist = sqrt(pow(x, 2.0f) + pow(z, 2.0f));
    double y = cosh(dist);
    double ratio = sqrt((pow(y, 2) - 1) / (pow(dist, 2)));

    return glm::dvec3(ratio * x, y, ratio * z);
}

// Check if two coords are very close
static bool close(glm::dvec3 a, glm::dvec3 b)
{
    double dist = pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2) + pow(a[2] - b[2], 2);
    if (dist < 0.1)
        return true;
    return false;
}

// Counter-clockwise rotation, preserving y
static glm::dvec3 rotate(glm::dvec3 v, double angle)
{
    return glm::dvec3(v.x * cos(angle) - v.z * sin(angle), v.y, v.x * sin(angle) + v.z * cos(angle));
}

#endif