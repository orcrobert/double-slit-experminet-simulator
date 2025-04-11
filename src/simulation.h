#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <cmath>
#include <iostream>

class QuantumSimulation {
    
private:
    float screenWidth;
    float screenHeight;
    std::vector<float> interferencePattern;

    float calculateProbability(float x, float y);

public:
    QuantumSimulation(float screenWidth, float screenHeight);
    void simulateParticles(int numParticles);
    void drawPattern();

};

#endif