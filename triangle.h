#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector3.h"
#include "ray.h"
#include "color.h"
#include "texture.h"
#include "boundingbox.h"

class Triangle {
public:
    Triangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2,
             const Color &color, float reflectivity = 0.0f,
             float transparency = 0.0f, float refractiveIndex = 1.0f,
             Texture* texture = nullptr);

    bool doesIntersect(const Ray &ray) const;
    float getIntersectionDistance(const Ray &ray) const;
    Vector3 getNormal(const Vector3 &point) const;
    Color getColor(const Vector3& hitPoint) const;
    float getReflectivity() const;
    float getTransparency() const;
    float getRefractiveIndex() const;
    BoundingBox getBoundingBox() const;

private:
    Vector3 v0, v1, v2;
    Color color;
    float reflectivity;
    float transparency;
    float refractiveIndex;
    Texture* texture;
};

#endif
