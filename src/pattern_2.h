#pragma once
#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <vector>

class Pattern2 {
public:
    Pattern2();
    ~Pattern2();

    void init();                        // call once after OpenGL context is active
    void update(float dt);
    void draw(sf::RenderWindow& window);

private:
    // Shared phase state — mirrors Pattern1 exactly for lock
    float phase;
    float brightnessPhase;
    float frequency;
    float amplitude;
    float speed;
    float brightnessSpeed;

    // Ribbon geometry
    int   pointCount;
    float ribbonDepth;   // Z extrusion depth of the ribbon

    // OpenGL handles
    GLuint vao;
    GLuint vbo;
    GLuint shaderProgram;

    bool   initialized;

    // Internal helpers
    GLuint compileShader(GLenum type, const char* src);
    void   buildShaderProgram();
    void   buildGeometry(std::vector<float>& vertices);
};