#include "simulation.h"

QuantumSimulation::QuantumSimulation(float screenWidth, float screenHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
}

void QuantumSimulation::simulateParticles(int numParticles) {
    this->interferencePattern.clear();

    for (int i = 0; i < numParticles; ++i) {
        float x = rand() % (int)this->screenWidth;
        float y = rand() % (int)this->screenHeight;
        float probability;

        probability = this->calculateProbability(x, y);
        this->interferencePattern.push_back(probability);
    }
}

float QuantumSimulation::calculateProbability(float x, float y) {
    float distance = std::sqrt(x * x + y * y);
    return 0.5f * (1 + std::sin(distance / 10.0f));
}

void QuantumSimulation::drawPattern() {
    for (const auto& prob : interferencePattern) {
        std::cout << "Probability: " << prob << std::endl;
    }
}