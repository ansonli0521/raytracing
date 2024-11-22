#ifndef CYLINDER_H
#define CYLINDER_H

#include "vector3.h"
#include "ray.h"
#include "color.h"
#include "texture.h"

class Cylinder {
public:
    Cylinder(const Vector3 &c, const Vector3 &a, float r, float h, const Color &color,
             float reflectivity = 0.0f, float transparency = 0.0f,
             float refractiveIndex = 1.0f, Texture* texture = nullptr);

    bool doesIntersect(const Ray &ray) const;
    float getIntersectionDistance(const Ray &ray) const;
    Vector3 getNormal(const Vector3 &point) const;
    Color getColor(const Vector3 &hitPoint) const;
    float getReflectivity() const;
    float getTransparency() const;
    float getRefractiveIndex() const;

private:
    Vector3 center;
    Vector3 axis;
    float radius;
    float height;
    Color color;
    float reflectivity;
    float transparency;
    float refractiveIndex;
    Texture* texture;
};

#endif
