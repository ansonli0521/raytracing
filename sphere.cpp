#include "sphere.h"
#include "boundingbox.h"
#include <cmath>

Sphere::Sphere(const Vector3 &center, float radius, const Color &color, 
               float reflectivity, float transparency, float refractiveIndex, 
               Texture* texture)
    : center(center), radius(radius), color(color), reflectivity(reflectivity), 
      transparency(transparency), refractiveIndex(refractiveIndex), texture(texture) {}

bool Sphere::doesIntersect(const Ray &ray) const {
    Vector3 oc = ray.origin - center;
    float a = ray.direction.dot(ray.direction);
    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false; // No intersection
    }

    float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
    float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

    return (t1 > 1e-6f || t2 > 1e-6f);
}

float Sphere::getIntersectionDistance(const Ray &ray) const {
    Vector3 oc = ray.origin - center;
    float a = ray.direction.dot(ray.direction);
    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1.0f; // No intersection
    }
    float t = (-b - std::sqrt(discriminant)) / (2.0f * a);
    if (t > 1e-6f) {
        return t;
    }
    t = (-b + std::sqrt(discriminant)) / (2.0f * a);
    return t > 1e-6f ? t : -1.0f;
}

Vector3 Sphere::getCenter() const {
    return center;
}

Color Sphere::getColor(const Vector3 &hitPoint) const {
    if (texture) {
        Vector3 normal = (hitPoint - center).normalize();
        float u = 0.5f + std::atan2(normal.z, normal.x) / (2.0f * M_PI);
        float v = 0.5f - std::asin(normal.y) / M_PI;

        return texture->getColorAt(u, v);
    }
    return color;
}

float Sphere::getReflectivity() const {
    return reflectivity;
}

float Sphere::getTransparency() const {
    return transparency;
}

float Sphere::getRefractiveIndex() const {
    return refractiveIndex;
}

BoundingBox Sphere::getBoundingBox() const {
    Vector3 min = center - Vector3(radius, radius, radius);
    Vector3 max = center + Vector3(radius, radius, radius);
    return BoundingBox(min, max);
}
