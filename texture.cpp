#include "texture.h"
#include <iostream>
#include <fstream>
#include <sstream>

Texture::Texture(const std::string& filePath) {
    if (!loadTexture(filePath)) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
    }
}

bool Texture::loadTexture(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    std::string header;
    file >> header;

    if (header != "P6") {
        return false;
    }

    file >> width >> height;
    int maxColor;
    file >> maxColor;
    file.ignore();

    data.resize(width * height);

    for (int i = 0; i < width * height; ++i) {
        unsigned char rgb[3];
        file.read(reinterpret_cast<char*>(rgb), 3);
        data[i] = {rgb[0] / 255.0f, rgb[1] / 255.0f, rgb[2] / 255.0f};
    }

    return true;
}

Color Texture::getColorAt(float u, float v) const {
    int x = static_cast<int>(u * width) % width;
    int y = static_cast<int>(v * height) % height;
    return data[y * width + x];
}
