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
    bool hitCurvedSurface = false;

    float t = -1.0f;

    if (discriminant >= 0) {
        float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
        float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

        t = (t1 > 1e-6f) ? t1 : ((t2 > 1e-6f) ? t2 : -1.0f);

        if (t > 1e-6f) {
            Vector3 intersection = ray.origin + ray.direction * t;
            float heightCheck = (intersection - center).dot(axis);
            if (heightCheck >= -height / 2 && heightCheck <= height / 2) {
                hitCurvedSurface = true;
            }
        }
    }

    float tTop = ((center + axis * (height / 2)) - ray.origin).dot(axis) /
                 ray.direction.dot(axis);
    bool hitTop = false;

    if (tTop > 1e-6f) {
        Vector3 p = ray.origin + ray.direction * tTop;
        if ((p - (center + axis * (height / 2))).length() <= radius) {
            hitTop = true;
            t = (t < 0 || tTop < t) ? tTop : t;
        }
    }

    float tBottom = ((center - axis * (height / 2)) - ray.origin).dot(axis) /
                    ray.direction.dot(axis);
    bool hitBottom = false;

    if (tBottom > 1e-6f) {
        Vector3 p = ray.origin + ray.direction * tBottom;
        if ((p - (center - axis * (height / 2))).length() <= radius) {
            hitBottom = true;
            t = (t < 0 || tBottom < t) ? tBottom : t;
        }
    }

    return hitCurvedSurface || hitTop || hitBottom;
}

float Cylinder::getIntersectionDistance(const Ray &ray) const {
    Vector3 oc = ray.origin - center;
    Vector3 d = ray.direction - axis * ray.direction.dot(axis);
    Vector3 o = oc - axis * oc.dot(axis);

    float a = d.dot(d);
    float b = 2.0f * o.dot(d);
    float c = o.dot(o) - radius * radius;

    float discriminant = b * b - 4 * a * c;
    float t = -1.0f;

    if (discriminant >= 0) {
        float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
        float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

        t = (t1 > 1e-6f) ? t1 : ((t2 > 1e-6f) ? t2 : -1.0f);

        if (t > 1e-6f) {
            Vector3 intersection = ray.origin + ray.direction * t;
            float heightCheck = (intersection - center).dot(axis);
            if (heightCheck < -height / 2 || heightCheck > height / 2) {
                t = -1.0f;
            }
        }
    }

    float tTop = ((center + axis * (height / 2)) - ray.origin).dot(axis) /
                 ray.direction.dot(axis);
    if (tTop > 1e-6f) {
        Vector3 p = ray.origin + ray.direction * tTop;
        if ((p - (center + axis * (height / 2))).length() <= radius) {
            t = (t < 0 || tTop < t) ? tTop : t;
        }
    }

    float tBottom = ((center - axis * (height / 2)) - ray.origin).dot(axis) /
                    ray.direction.dot(axis);
    if (tBottom > 1e-6f) {
        Vector3 p = ray.origin + ray.direction * tBottom;
        if ((p - (center - axis * (height / 2))).length() <= radius) {
            t = (t < 0 || tBottom < t) ? tBottom : t;
        }
    }

    return t;
}

Vector3 Cylinder::getNormal(const Vector3 &point) const {
    Vector3 projection = axis * ((point - center).dot(axis));
    float heightCheck = (point - center).dot(axis);

    if (std::abs(heightCheck - height / 2) < 1e-6f) {
        return axis;
    } else if (std::abs(heightCheck + height / 2) < 1e-6f) {
        return -axis;
    }

    return (point - center - projection).normalize(); // Curved surface
}

Color Cylinder::getColor() const {
    return color;
}
