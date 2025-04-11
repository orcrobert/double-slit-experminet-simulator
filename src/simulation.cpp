#include <GL/glew.h>
#include "simulation.h"

GLuint shaderProgram;
GLuint VAO, VBO, colorVBO;

QuantumSimulation::QuantumSimulation(float screenWidth, float screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight) {

    initOpenGL();
}

void QuantumSimulation::initOpenGL() {
    shaderProgram = loadShaders("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorVBO);
}

GLuint QuantumSimulation::loadShaders(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vertexFile(vertexPath);
    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();
    vertexCode = vertexStream.str();

    std::ifstream fragmentFile(fragmentPath);
    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();
    fragmentCode = fragmentStream.str();

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void QuantumSimulation::simulateParticles(int numParticles) {
    interferencePattern.clear();
    for (int i = 0; i < numParticles; ++i) {
        float x = rand() % (int)screenWidth;
        float y = rand() % (int)screenHeight;
        float probability = calculateProbability(x, y);

        interferencePattern.push_back({x / screenWidth * 2.0f - 1.0f, y / screenHeight * 2.0f - 1.0f, probability});
    }
}

void QuantumSimulation::drawPattern() {
    std::vector<float> positions;
    std::vector<float> colors;

    for (const auto& particle : interferencePattern) {
        positions.push_back(particle.x);
        positions.push_back(particle.y);
        colors.push_back(particle.probability);
        colors.push_back(0.0f);
        colors.push_back(1.0f - particle.probability);
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);
    glDrawArrays(GL_POINTS, 0, positions.size() / 2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

float QuantumSimulation::calculateProbability(float x, float y) {
    return static_cast<float>(rand()) / RAND_MAX;
}