
#ifndef RAY_H
#define RAY_H

#include "vector3.h"

class Ray {
public:
    Ray(const Vector3 &origin, const Vector3 &direction)
        : origin(origin), direction(direction.normalize()) {}

    Vector3 origin;
    Vector3 direction;
};

#endif
