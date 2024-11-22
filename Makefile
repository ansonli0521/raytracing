CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O3 -I.
TARGET = raytracer
SRC = raytracer.cpp camera.cpp scene.cpp sphere.cpp triangle.cpp cylinder.cpp texture.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
