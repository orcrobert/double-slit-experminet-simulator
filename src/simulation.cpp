#include <GL/glew.h>
#include "simulation.h"
#include <cmath>

GLuint shaderProgram;
GLuint slitShaderProgram;
GLuint VAO, VBO, colorVBO;

QuantumSimulation::QuantumSimulation(float screenWidth, float screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
    slitSeparation(0.22f), slitWidth(0.03f), distanceToScreen(1.0f) {
        
    initOpenGL();
}

void QuantumSimulation::initOpenGL() {
    shaderProgram = loadShaders("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    slitShaderProgram = loadShaders("shaders/slit_vertex_shader.glsl", "shaders/slit_fragment_shader.glsl");

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
    float wavelength = 0.05f;

    for (int i = 0; i < numParticles; ++i) {
        float x = rand() % (int)screenWidth;
        float y = rand() % (int)screenHeight;

        float normalizedX = x / screenWidth * 2.0f - 1.0f;
        float normalizedY = y / screenHeight * 2.0f - 1.0f;

        float d1 = sqrt(pow(normalizedX - slitSeparation / 2.0f, 2) + pow(distanceToScreen - 0.0f, 2));
        float d2 = sqrt(pow(normalizedX + slitSeparation / 2.0f, 2) + pow(distanceToScreen - 0.0f, 2));

        float delta_d = std::abs(d1 - d2);

        float phi = (2 * M_PI * delta_d) / wavelength;
        float probability = 0.5f + 0.5f * cos(phi);

        float beta = (M_PI * slitWidth * (normalizedX / distanceToScreen)) / wavelength;
        float diffractionFactor = 1.0f;
        if (std::abs(beta) > 1e-6) {
            diffractionFactor = pow(sinf(beta) / beta, 2);
        }

        probability *= diffractionFactor;
        probability = std::max(0.0f, std::min(1.0f, probability));

        interferencePattern.push_back({normalizedX, normalizedY, probability});
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


    GLuint slitVAO, slitVBO;
    glGenVertexArrays(1, &slitVAO);
    glGenBuffers(1, &slitVBO);

    float slitX = -0.5f;
    float slitHalfWidth = slitWidth / 2.0f;
    float slitHeight = 0.1f;
    float separation = slitSeparation;

    std::vector<float> slitVertices = {
        slitX - slitHalfWidth,  slitHeight, 0.0f,
        slitX + slitHalfWidth,  slitHeight, 0.0f,
        slitX + slitHalfWidth, -slitHeight, 0.0f,

        slitX + slitHalfWidth, -slitHeight, 0.0f,
        slitX - slitHalfWidth, -slitHeight, 0.0f,
        slitX - slitHalfWidth,  slitHeight, 0.0f,

        slitX + separation - slitHalfWidth,  slitHeight, 0.0f,
        slitX + separation + slitHalfWidth,  slitHeight, 0.0f,
        slitX + separation + slitHalfWidth, -slitHeight, 0.0f,

        slitX + separation + slitHalfWidth, -slitHeight, 0.0f,
        slitX + separation - slitHalfWidth, -slitHeight, 0.0f,
        slitX + separation - slitHalfWidth,  slitHeight, 0.0f
    };

    glBindVertexArray(slitVAO);
    glBindBuffer(GL_ARRAY_BUFFER, slitVBO);
    glBufferData(GL_ARRAY_BUFFER, slitVertices.size() * sizeof(float), slitVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(slitShaderProgram);
    int slitColorLocation = glGetUniformLocation(slitShaderProgram, "slitColor");
    glUniform4f(slitColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 12);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &slitVAO);
    glDeleteBuffers(1, &slitVBO);

    glUseProgram(shaderProgram);
}

float QuantumSimulation::calculateProbability(float x, float y) {
    return 0.0f;
}