#include "pattern_2.h"
#include <cmath>
#include <vector>
#include <stdexcept>

static const char* VERT_SRC = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in float aFaceId;

out float faceId;
out float zDepth;

uniform float uTiltX;
uniform float uTiltY;

void main() {
    vec3 p = aPos;
    p.x += p.z * uTiltX;
    p.y += p.z * uTiltY;

    gl_Position = vec4(p, 1.0);
    faceId  = aFaceId;
    zDepth  = aPos.z;
}
)";

static const char* FRAG_SRC = R"(
#version 330 core
in  float faceId;
in  float zDepth;
out vec4  fragColor;

uniform float uBrightness;
uniform float uKickStrength;   // ← new, pulses the glow on beat

void main() {
    float boost = 1.0 + 0.4 * uKickStrength;   // kick brightens faces

    vec3 frontColor = vec3(0.0, uBrightness * boost, uBrightness * 0.85 * boost);
    vec3 topColor   = vec3(0.0, uBrightness * 0.6,   uBrightness * 0.5);
    vec3 backColor  = vec3(0.0, uBrightness * 0.25,  uBrightness * 0.2);

    vec3 color;
    if (faceId < 0.5)       color = frontColor;
    else if (faceId < 1.5)  color = topColor;
    else                    color = backColor;

    float alpha = 0.55 + 0.35 * uBrightness + 0.15 * uKickStrength;
    fragColor = vec4(color, alpha);
}
)";

// ─────────────────────────────────────────────
Pattern2::Pattern2()
    : ribbonDepth(0.06f)
    , pointCount(320)
    , vao(0), vbo(0)
    , shaderProgram(0)
    , initialized(false)
{}

Pattern2::~Pattern2() {
    if (vao)           glDeleteVertexArrays(1, &vao);
    if (vbo)           glDeleteBuffers(1, &vbo);
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

void Pattern2::init() {
    if (initialized) return;

    buildShaderProgram();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    size_t maxVerts = (size_t)(pointCount - 1) * 3 * 6;
    glBufferData(GL_ARRAY_BUFFER, maxVerts * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    initialized = true;
}

void Pattern2::draw(sf::RenderWindow& window, const EngineState& state) {
    if (!initialized) return;

    float W = 1280.f;
    float H = 720.f;

    auto toNDC_X = [&](float px) { return (px / W) * 2.f - 1.f; };
    auto toNDC_Y = [&](float py) { return 1.f - (py / H) * 2.f; };

    float centerY = H / 2.f;
    float zFront  =  ribbonDepth * 0.5f;
    float zBack   = -ribbonDepth * 0.5f;

    struct Sample { float x, yTop, yBot; };
    std::vector<Sample> samples(pointCount);

    for (int i = 0; i < pointCount; ++i) {
        float px  = (float)i / (float)(pointCount - 1) * W;
        float t   = (px / W) * state.frequency + state.phase;
        float saw = 2.f * (t - std::floor(t)) - 1.f;
        float py  = centerY - saw * state.effectiveAmplitude;  // kicks land here

        samples[i].x    = toNDC_X(px);
        samples[i].yTop = toNDC_Y(py);
        samples[i].yBot = toNDC_Y(centerY);
    }

    std::vector<float> verts;
    verts.reserve((pointCount - 1) * 3 * 6 * 4);

    auto push = [&](float x, float y, float z, float fid) {
        verts.push_back(x); verts.push_back(y);
        verts.push_back(z); verts.push_back(fid);
    };

    auto quad = [&](
        float x0, float y0, float z0,
        float x1, float y1, float z1,
        float x2, float y2, float z2,
        float x3, float y3, float z3,
        float faceId)
    {
        push(x0,y0,z0,faceId); push(x1,y1,z1,faceId); push(x2,y2,z2,faceId);
        push(x0,y0,z0,faceId); push(x2,y2,z2,faceId); push(x3,y3,z3,faceId);
    };

    for (int i = 0; i < pointCount - 1; ++i) {
        float xL = samples[i].x,    xR = samples[i+1].x;
        float tL = samples[i].yTop, tR = samples[i+1].yTop;
        float bL = samples[i].yBot, bR = samples[i+1].yBot;

        quad(xL,tL,zFront, xR,tR,zFront, xR,bR,zFront, xL,bL,zFront, 0.f);
        quad(xL,tL,zFront, xR,tR,zFront, xR,tR,zBack,  xL,tL,zBack,  1.f);
        quad(xL,bL,zBack,  xR,bR,zBack,  xR,tR,zBack,  xL,tL,zBack,  2.f);
    }

    window.setActive(true);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(float), verts.data());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(shaderProgram);

    float brightness = 160.f + 95.f * (0.5f + 0.5f * std::sin(state.brightnessPhase));
    float bNorm = brightness / 255.f;

    glUniform1f(glGetUniformLocation(shaderProgram, "uBrightness"),   bNorm);
    glUniform1f(glGetUniformLocation(shaderProgram, "uKickStrength"), state.kickStrength);
    glUniform1f(glGetUniformLocation(shaderProgram, "uTiltX"),  0.04f);
    glUniform1f(glGetUniformLocation(shaderProgram, "uTiltY"), -0.06f);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(verts.size() / 4));

    glUseProgram(0);
    glBindVertexArray(0);

    window.setActive(false);
}

GLuint Pattern2::compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        glDeleteShader(s);
        throw std::runtime_error(std::string("Shader compile error: ") + log);
    }
    return s;
}

void Pattern2::buildShaderProgram() {
    GLuint vert = compileShader(GL_VERTEX_SHADER,   VERT_SRC);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, FRAG_SRC);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);

    GLint ok = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        glDeleteProgram(shaderProgram);
        throw std::runtime_error(std::string("Shader link error: ") + log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}