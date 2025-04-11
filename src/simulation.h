#ifndef SIMULATION_H
#define SIMULATION_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

struct Particle {
    float x, y, probability;
};

class QuantumSimulation {

private:
    float screenWidth;
    float screenHeight;

    float slitSeparation;
    float slitWidth;
    float distanceToScreen;

    std::vector<Particle> interferencePattern;

    float calculateProbability(float x, float y);
    void initOpenGL();
    GLuint loadShaders(const char* vertexPath, const char* fragmentPath);

public:
    QuantumSimulation(float screenWidth, float screenHeight);
    void simulateParticles(int numParticles);
    void drawPattern();

};

#endif