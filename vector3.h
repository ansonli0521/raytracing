#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>

class Vector3 {
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Vector addition
    Vector3 operator+(const Vector3 &other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // Vector subtraction
    Vector3 operator-(const Vector3 &other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // Unary negation
    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }

    // Dot product
    float dot(const Vector3 &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vector3 cross(const Vector3 &other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Scalar multiplication
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // Scalar division
    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    // Normalize vector
    Vector3 normalize() const {
        float magnitude = std::sqrt(x * x + y * y + z * z);
        return (*this) / magnitude;
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
};

inline Vector3 operator*(float scalar, const Vector3 &vector) {
    return Vector3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

#endif
