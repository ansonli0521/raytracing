
#include "sphere.h"
#include <cmath>

Sphere::Sphere(const Vector3 &center, float radius, const Color &color)
    : center(center), radius(radius), color(color) {}

bool Sphere::doesIntersect(const Ray &ray) const {
    Vector3 oc = ray.origin - center;
    float a = ray.direction.dot(ray.direction);
    float b = 2.0 * oc.dot(ray.direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return discriminant > 0;
}

Color Sphere::getColor() const {
    return color;
}
