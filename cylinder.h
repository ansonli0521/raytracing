#ifndef CYLINDER_H
#define CYLINDER_H

#include "vector3.h"
#include "ray.h"
#include "color.h"

class Cylinder {
public:
    Cylinder(const Vector3 &c, const Vector3 &a, float r, float h, const Color &col);
    bool doesIntersect(const Ray &ray) const;
    float getIntersectionDistance(const Ray &ray) const;
    Vector3 getNormal(const Vector3 &point) const;
    Color getColor() const;

private:
    Vector3 center;
    Vector3 axis;
    float radius;
    float height;
    Color color;
};

#endif
