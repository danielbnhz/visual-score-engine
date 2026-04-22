#include "pattern_1_blur.h"
#include <cmath>

Pattern1Blur::Pattern1Blur()
    : driftPhase(0.f)
    , driftSpeed(0.3f)
    , driftAmount(15.f)
{}

void Pattern1Blur::buildRibbon(sf::VertexArray& va, float halfWidth, sf::Color color,
                                const EngineState& state, float centerY) {
    va.setPrimitiveType(sf::TriangleStrip);
    va.resize(1280 * 2);

    float width = 1280.f;

    for (int i = 0; i < 1280; i++) {
        float x   = (float)i;
        float t   = (x / width) * state.frequency + state.phase;
        float saw = 2.f * (t - std::floor(t)) - 1.f;
        float y   = centerY - saw * state.effectiveAmplitude;  // kicks here too

        va[i * 2].position     = sf::Vector2f(x, y - halfWidth);
        va[i * 2].color        = color;
        va[i * 2 + 1].position = sf::Vector2f(x, y + halfWidth);
        va[i * 2 + 1].color    = color;
    }
}

void Pattern1Blur::draw(sf::RenderWindow& window, const EngineState& state) {
    // drift is local — update it here since Pattern1Blur owns it
    driftPhase += driftSpeed * (1.f / 60.f);  // fixed step, fine for drift

    float centerY = 720.f / 2.f + driftAmount * std::sin(driftPhase);

    float brightness = 160.f + 95.f * (0.5f + 0.5f * std::sin(state.brightnessPhase));
    sf::Uint8 b  = static_cast<sf::Uint8>(brightness);
    sf::Uint8 b2 = static_cast<sf::Uint8>(b * 0.85f);

    struct Layer { float halfWidth; float alpha; };
    Layer layers[] = {
        { 18.f, 0.08f },
        {  9.f, 0.18f },
        {  4.f, 0.35f },
    };

    sf::VertexArray ribbon;
    for (auto& layer : layers) {
        sf::Color col(
            0,
            static_cast<sf::Uint8>(b  * layer.alpha),
            static_cast<sf::Uint8>(b2 * layer.alpha),
            static_cast<sf::Uint8>(255.f * layer.alpha)
        );
        buildRibbon(ribbon, layer.halfWidth, col, state, centerY);
        window.draw(ribbon);
    }
}