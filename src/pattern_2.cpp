#include "pattern_2.h"
#include <cmath>
#include <vector>
#include <stdexcept>

// ─────────────────────────────────────────────
//  Vertex shader — simple MVP with no matrices,
//  we bake NDC directly so we stay coord-aligned
//  with SFML's 1280x720 canvas.
// ─────────────────────────────────────────────
static const char* VERT_SRC = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in float aFaceId;  // 0 = front, 1 = top, 2 = back

out float faceId;
out float zDepth;

uniform float uTiltX;   // subtle perspective tilt
uniform float uTiltY;

void main() {
    // Lean the ribbon toward the viewer with a mild perspective skew
    vec3 p = aPos;
    p.x += p.z * uTiltX;
    p.y += p.z * uTiltY;

    gl_Position = vec4(p, 1.0);
    faceId  = aFaceId;
    zDepth  = aPos.z;
}
)";

// ─────────────────────────────────────────────
//  Fragment shader — oscillating cyan/teal palette
//  synced via uBrightness uniform
// ─────────────────────────────────────────────
static const char* FRAG_SRC = R"(
#version 330 core
in  float faceId;
in  float zDepth;
out vec4  fragColor;

uniform float uBrightness;   // 0..1, mirrors brightnessPhase sin

void main() {
    // Base hue: cyan family, matching Pattern1
    vec3 frontColor = vec3(0.0, uBrightness, uBrightness * 0.85);
    vec3 topColor   = vec3(0.0, uBrightness * 0.6, uBrightness * 0.5);
    vec3 backColor  = vec3(0.0, uBrightness * 0.25, uBrightness * 0.2);

    vec3 color;
    if (faceId < 0.5)       color = frontColor;
    else if (faceId < 1.5)  color = topColor;
    else                    color = backColor;

    // Fade edges toward transparency for soft layering
    float alpha = 0.55 + 0.35 * uBrightness;

    fragColor = vec4(color, alpha);
}
)";

// ─────────────────────────────────────────────
//  Ctor / Dtor
// ─────────────────────────────────────────────
Pattern2::Pattern2()
    : phase(0.f)
    , brightnessPhase(0.f)
    , frequency(4.f)
    , amplitude(120.f)
    , speed(1.5f)
    , brightnessSpeed(2.f)
    , pointCount(320)       // fewer points than Pattern1; ribbon quads fill the gaps
    , ribbonDepth(0.06f)    // NDC units of Z extrusion
    , vao(0), vbo(0)
    , shaderProgram(0)
    , initialized(false)
{}

Pattern2::~Pattern2() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

// ─────────────────────────────────────────────
//  init() — call after SFML window creation
// ─────────────────────────────────────────────
void Pattern2::init() {
    if (initialized) return;

    buildShaderProgram();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // Allocate max buffer — updated each frame
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Each segment: 3 quads (front face, top face, back face)
    // Each quad = 2 triangles = 6 vertices
    // Each vertex: vec3 pos + float faceId = 4 floats
    size_t maxVerts = (size_t)(pointCount - 1) * 3 * 6;
    glBufferData(GL_ARRAY_BUFFER, maxVerts * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // aFaceId
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    initialized = true;
}

// ─────────────────────────────────────────────
//  update — identical timing to Pattern1
// ─────────────────────────────────────────────
void Pattern2::update(float dt) {
    phase         += speed         * dt;
    brightnessPhase += brightnessSpeed * dt;
}

// ─────────────────────────────────────────────
//  draw
// ─────────────────────────────────────────────
void Pattern2::draw(sf::RenderWindow& window) {
    if (!initialized) return;

    float W = 1280.f;
    float H = 720.f;

    // ── build geometry ──────────────────────────
    // We work in NDC: x in [-1,1], y in [-1,1]
    // Map pixel coords → NDC inline

    auto toNDC_X = [&](float px) { return (px / W) * 2.f - 1.f; };
    auto toNDC_Y = [&](float py) { return 1.f - (py / H) * 2.f; };  // flip Y

    float centerY = H / 2.f;
    float zFront  =  ribbonDepth * 0.5f;
    float zBack   = -ribbonDepth * 0.5f;

    // Compute sawtooth Y for each column sample
    struct Sample { float x, yTop, yBot; };  // yTop = wave edge, yBot = center line
    std::vector<Sample> samples(pointCount);

    for (int i = 0; i < pointCount; ++i) {
        float px = (float)i / (float)(pointCount - 1) * W;
        float t  = (px / W) * frequency + phase;
        float saw = 2.f * (t - std::floor(t)) - 1.f;
        float py  = centerY - saw * amplitude;

        samples[i].x    = toNDC_X(px);
        samples[i].yTop = toNDC_Y(py);
        samples[i].yBot = toNDC_Y(centerY);  // ribbon base at center line
    }

    // Build triangle list
    // For each segment [i, i+1] we emit 3 quad faces:
    //   face 0 (front):  (xL,yTopL,zFront) → (xR,yTopR,zFront) × (xL,yBotL,zFront) etc
    //   face 1 (top):    front top edge → back top edge
    //   face 2 (back):   reversed winding

    std::vector<float> verts;
    verts.reserve((pointCount - 1) * 3 * 6 * 4);

    auto push = [&](float x, float y, float z, float fid) {
        verts.push_back(x);
        verts.push_back(y);
        verts.push_back(z);
        verts.push_back(fid);
    };

    auto quad = [&](
        float x0, float y0, float z0,
        float x1, float y1, float z1,
        float x2, float y2, float z2,
        float x3, float y3, float z3,
        float faceId)
    {
        // Triangle 1: 0,1,2
        push(x0,y0,z0,faceId); push(x1,y1,z1,faceId); push(x2,y2,z2,faceId);
        // Triangle 2: 0,2,3
        push(x0,y0,z0,faceId); push(x2,y2,z2,faceId); push(x3,y3,z3,faceId);
    };

    for (int i = 0; i < pointCount - 1; ++i) {
        float xL = samples[i].x,   xR = samples[i+1].x;
        float tL = samples[i].yTop, tR = samples[i+1].yTop;
        float bL = samples[i].yBot, bR = samples[i+1].yBot;

        // Front face (z = zFront)
        quad(xL,tL,zFront, xR,tR,zFront, xR,bR,zFront, xL,bL,zFront, 0.f);

        // Top face (wave crest, zFront → zBack)
        quad(xL,tL,zFront, xR,tR,zFront, xR,tR,zBack, xL,tL,zBack,   1.f);

        // Back face (z = zBack, reversed winding so normals face outward)
        quad(xL,bL,zBack, xR,bR,zBack, xR,tR,zBack, xL,tL,zBack,     2.f);
    }

    // ── upload ──────────────────────────────────
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(float), verts.data());

    // ── render state ────────────────────────────
    window.setActive(true);

    // Preserve SFML's blend expectation
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);   // painter's order, layered on top of SFML draw

    glUseProgram(shaderProgram);

    // Brightness uniform — mirrors Pattern1's formula exactly
    float brightness = 160.f + 95.f * (0.5f + 0.5f * std::sin(brightnessPhase));
    float bNorm = brightness / 255.f;
    glUniform1f(glGetUniformLocation(shaderProgram, "uBrightness"), bNorm);

    // Subtle perspective tilt — ribbon leans slightly up-right
    glUniform1f(glGetUniformLocation(shaderProgram, "uTiltX"),  0.04f);
    glUniform1f(glGetUniformLocation(shaderProgram, "uTiltY"), -0.06f);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(verts.size() / 4));

    glUseProgram(0);
    glBindVertexArray(0);

    // Hand control back to SFML
    window.setActive(false);
}

// ─────────────────────────────────────────────
//  Shader helpers
// ─────────────────────────────────────────────
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