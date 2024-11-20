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
    if (discriminant < 0) {
        return false;
    }

    float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
    float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

    float t = (t1 > 1e-6f) ? t1 : t2;
    if (t <= 1e-6f) {
        return false;
    }

    Vector3 intersection = ray.origin + ray.direction * t;
    float heightCheck = (intersection - center).dot(axis);
    return heightCheck >= 0 && heightCheck <= height;
}

float Cylinder::getIntersectionDistance(const Ray &ray) const {
    Vector3 oc = ray.origin - center;
    Vector3 d = ray.direction - axis * ray.direction.dot(axis);
    Vector3 o = oc - axis * oc.dot(axis);

    float a = d.dot(d);
    float b = 2.0f * o.dot(d);
    float c = o.dot(o) - radius * radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1.0f;
    }

    float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
    float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

    float t = (t1 > 1e-6f) ? t1 : t2;
    if (t <= 1e-6f) {
        return -1.0f;
    }

    Vector3 intersection = ray.origin + ray.direction * t;
    float heightCheck = (intersection - center).dot(axis);
    return (heightCheck >= 0 && heightCheck <= height) ? t : -1.0f;
}

Vector3 Cylinder::getNormal(const Vector3 &point) const {
    Vector3 projection = axis * ((point - center).dot(axis));
    return (point - center - projection).normalize();
}

Color Cylinder::getColor() const {
    return color;
}
