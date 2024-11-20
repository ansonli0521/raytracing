#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector3.h"
#include "ray.h"
#include "color.h"

class Triangle {
public:
    Triangle(const Vector3 &v0, const Vector3 &v1,
             const Vector3 &v2, const Color &color);
    bool doesIntersect(const Ray &ray) const;
    float getIntersectionDistance(const Ray &ray) const;
    Vector3 getNormal(const Vector3 &point) const;
    Color getColor() const;

private:
    Vector3 v0, v1, v2;
    Color color;
};

#endif
