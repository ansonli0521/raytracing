
#include "scene.h"

int main() {
    Scene scene;
    scene.loadFromJson("scene.json");

    scene.getCamera()->renderScene(scene, "output.ppm");
    return 0;
}
