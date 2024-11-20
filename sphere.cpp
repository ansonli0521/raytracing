#include "sphere.h"
#include <cmath>

Sphere::Sphere(const Vector3 &center, float radius, const Color &color)
    : center(center), radius(radius), color(color) {}

bool Sphere::doesIntersect(const Ray &ray) const {
    Vector3 oc = ray.origin - center;
    float a = ray.direction.dot(ray.direction);
    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return discriminant > 0;
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

Color Sphere::getColor() const {
    return color;
}
