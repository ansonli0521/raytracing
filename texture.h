#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include "color.h"

class Texture {
public:
    Texture(const std::string& filePath);
    Color getColorAt(float u, float v) const;

private:
    int width, height;
    std::vector<Color> data;
    bool loadTexture(const std::string& filePath);
};

#endif
