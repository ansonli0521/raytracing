#ifndef SPHERE_H
#define SPHERE_H

#include "vector3.h"
#include "ray.h"
#include "color.h"

class Sphere {
public:
    Sphere(const Vector3 &center, float radius, const Color &color);
    bool doesIntersect(const Ray &ray) const;
    float getIntersectionDistance(const Ray &ray) const;
    Vector3 getCenter() const;
    Color getColor() const;

private:
    Vector3 center;
    float radius;
    Color color;
};

#endif
