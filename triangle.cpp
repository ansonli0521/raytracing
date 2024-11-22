#include "triangle.h"
#include <cmath>

Triangle::Triangle(const Vector3 &vertex0, const Vector3 &vertex1, const Vector3 &vertex2, 
                   const Color &color, float reflectivity, float transparency, 
                   float refractiveIndex, Texture* texture)
    : v0(vertex0), v1(vertex1), v2(vertex2), color(color), reflectivity(reflectivity), 
      transparency(transparency), refractiveIndex(refractiveIndex), texture(texture) {}

bool Triangle::doesIntersect(const Ray &ray) const {
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    Vector3 h = ray.direction.cross(edge2);
    float a = edge1.dot(h);

    if (std::abs(a) < 1e-6f) {
        return false; // Parallel ray
    }

    float f = 1.0f / a;
    Vector3 s = ray.origin - v0;
    float u = f * s.dot(h);

    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    Vector3 q = s.cross(edge1);
    float v = f * ray.direction.dot(q);

    if (v < 0.0f || (u + v) > 1.0f) {
        return false;
    }

    float t = f * edge2.dot(q);
    return t > 1e-6f;
}

float Triangle::getIntersectionDistance(const Ray &ray) const {
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    Vector3 h = ray.direction.cross(edge2);
    float a = edge1.dot(h);

    if (std::abs(a) < 1e-6f) {
        return -1.0f; // Parallel ray
    }

    float f = 1.0f / a;
    Vector3 s = ray.origin - v0;
    float u = f * s.dot(h);

    if (u < 0.0f || u > 1.0f) {
        return -1.0f;
    }

    Vector3 q = s.cross(edge1);
    float v = f * ray.direction.dot(q);

    if (v < 0.0f || (u + v) > 1.0f) {
        return -1.0f;
    }

    float t = f * edge2.dot(q);
    return t > 1e-6f ? t : -1.0f;
}

Vector3 Triangle::getNormal(const Vector3 &) const {
    return (v1 - v0).cross(v2 - v0).normalize();
}

Color Triangle::getColor(const Vector3 &hitPoint) const {
    if (texture) {
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;
        Vector3 pointVector = hitPoint - v0;

        float d00 = edge1.dot(edge1);
        float d01 = edge1.dot(edge2);
        float d11 = edge2.dot(edge2);
        float d20 = pointVector.dot(edge1);
        float d21 = pointVector.dot(edge2);
        float denom = d00 * d11 - d01 * d01;

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        return texture->getColorAt(u, v);
    }
    return color;
}

float Triangle::getReflectivity() const {
    return reflectivity;
}

float Triangle::getTransparency() const {
    return transparency;
}

float Triangle::getRefractiveIndex() const {
    return refractiveIndex;
}
