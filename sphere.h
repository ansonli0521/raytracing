#ifndef SPHERE_H
#define SPHERE_H

#include "vector3.h"
#include "ray.h"
#include "color.h"
#include "texture.h"
#include "boundingbox.h"

class Sphere {
public:
    Sphere(const Vector3 &center, float radius, const Color &color,
           float reflectivity = 0.0f, float transparency = 0.0f,
           float refractiveIndex = 1.0f, Texture* texture = nullptr);

    bool doesIntersect(const Ray &ray) const;
    float getIntersectionDistance(const Ray &ray) const;
    Vector3 getCenter() const;
    Color getColor(const Vector3& hitPoint) const;
    float getReflectivity() const;
    float getTransparency() const;
    float getRefractiveIndex() const;
    BoundingBox getBoundingBox() const;

private:
    Vector3 center;
    float radius;
    Color color;
    float reflectivity;
    float transparency;
    float refractiveIndex;
    Texture* texture;
};

#endif
