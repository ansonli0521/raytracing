#include "cylinder.h"
#include <cmath>

Cylinder::Cylinder(const Vector3 &c, const Vector3 &a, float r, float h, const Color &col)
    : center(c), axis(a.normalize()), radius(r), height(h), color(col) {}

bool Cylinder::doesIntersect(const Ray &ray) const {
    Vector3 oc = ray.origin - center;
    Vector3 d = ray.direction - axis * ray.direction.dot(axis);
    Vector3 o = oc - axis * oc.dot(axis);

    float a = d.dot(d);
    float b = 2.0f * o.dot(d);
    float c = o.dot(o) - radius * radius;

    float discriminant = b * b - 4 * a * c;

    // Check discriminant
    if (discriminant < 0) {
        return false;
    }

    // Calculate intersection distances
    float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
    float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

    // Choose the closest valid intersection
    float t = (t1 > 1e-6f) ? t1 : t2;
    if (t <= 1e-6f) {
        return false;
    }

    // Calculate intersection point
    Vector3 intersection = ray.origin + ray.direction * t;

    // Check height constraint
    float heightCheck = (intersection - center).dot(axis);
    if (heightCheck < 0 || heightCheck > height) {
        return false;
    }

    return true;
}

Color Cylinder::getColor() const {
    return color;
}
