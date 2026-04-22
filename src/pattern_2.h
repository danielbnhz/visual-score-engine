#pragma once
#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include "engine_state.h"

class Pattern2 {
public:
    Pattern2();
    ~Pattern2();

    void init();
    void draw(sf::RenderWindow& window, const EngineState& state);

private:
    float ribbonDepth;
    int   pointCount;

    GLuint vao;
    GLuint vbo;
    GLuint shaderProgram;
    bool   initialized;

    GLuint compileShader(GLenum type, const char* src);
    void   buildShaderProgram();
};