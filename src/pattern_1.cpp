#include "pattern_1.h"
#include <cmath>

Pattern1::Pattern1() {}

void Pattern1::draw(sf::RenderWindow& window, const EngineState& state) {
    sf::VertexArray waveform(sf::LineStrip, 1280);

    float width   = 1280.f;
    float height  = 720.f;
    float centerY = height / 2.f;

    float brightness = 160.f + 95.f * (0.5f + 0.5f * std::sin(state.brightnessPhase));
    sf::Uint8 b = static_cast<sf::Uint8>(brightness);

    for (int i = 0; i < 1280; i++) {
        float x   = (float)i;
        float t   = (x / width) * state.frequency + state.phase;
        float saw = 2.f * (t - std::floor(t)) - 1.f;
        float y   = centerY - saw * state.effectiveAmplitude;  // ← kicks land here

        waveform[i].position = sf::Vector2f(x, y);
        waveform[i].color    = sf::Color(0, b, static_cast<sf::Uint8>(b * 0.85f));
    }

    window.draw(waveform);
}