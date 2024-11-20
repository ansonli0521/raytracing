#ifndef COLOR_H
#define COLOR_H

struct Color {
    float r, g, b;

    Color(float r = 0, float g = 0, float b = 0) : r(r), g(g), b(b) {}

    // Addition of two colors
    Color operator+(const Color &other) const {
        return Color(r + other.r, g + other.g, b + other.b);
    }

    // Multiplication of color by a scalar
    Color operator*(float scalar) const {
        return Color(r * scalar, g * scalar, b * scalar);
    }

    // Multiplication of two colors (component-wise)
    Color operator*(const Color &other) const {
        return Color(r * other.r, g * other.g, b * other.b);
    }

    // Compound addition assignment
    Color& operator+=(const Color &other) {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }
};

#endif
