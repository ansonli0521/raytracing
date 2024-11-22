#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "vector3.h"
#include "ray.h"
#include <algorithm>

class BoundingBox {
public:
    Vector3 min;
    Vector3 max;

    BoundingBox() : min(Vector3(0, 0, 0)), max(Vector3(0, 0, 0)) {}
    BoundingBox(const Vector3& min, const Vector3& max) : min(min), max(max) {}
    
    // Merge two bounding boxes
    static BoundingBox merge(const BoundingBox& a, const BoundingBox& b) {
        return BoundingBox(
            Vector3(std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y), std::min(a.min.z, b.min.z)),
            Vector3(std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y), std::max(a.max.z, b.max.z))
        );
    }

    // Check if a ray intersects the bounding box
    bool doesIntersect(const Ray& ray) const {
        float tmin = (min.x - ray.origin.x) / ray.direction.x;
        float tmax = (max.x - ray.origin.x) / ray.direction.x;
        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (min.y - ray.origin.y) / ray.direction.y;
        float tymax = (max.y - ray.origin.y) / ray.direction.y;
        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        tmin = std::max(tmin, tymin);
        tmax = std::min(tmax, tymax);

        float tzmin = (min.z - ray.origin.z) / ray.direction.z;
        float tzmax = (max.z - ray.origin.z) / ray.direction.z;
        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        return (tmin <= tzmax) && (tzmin <= tmax);
    }
};

#endif
