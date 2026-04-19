#include "pattern_1.h"
#include <cmath>

Pattern1::Pattern1()
    : phase(0.f)
    , brightnessPhase(0.f)
    , pointCount(1280)
    , frequency(4.f)
    , amplitude(120.f)
    , speed(1.5f)
    , brightnessSpeed(2.f)
{}

void Pattern1::update(float dt) {
    phase += speed * dt;
    brightnessPhase += brightnessSpeed * dt;
}

void Pattern1::draw(sf::RenderWindow& window) {
    sf::VertexArray waveform(sf::LineStrip, pointCount);

    float width = 1280.f;
    float height = 720.f;
    float centerY = height / 2.f;

    // brightness pulse between 80 and 255
    float brightness = 160.f + 95.f * (0.5f + 0.5f * std::sin(brightnessPhase));
    sf::Uint8 b = static_cast<sf::Uint8>(brightness);

    for (int i = 0; i < pointCount; i++) {
        float x = (float)i;
        float t = (x / width) * frequency + phase;

        // sawtooth: fractional part of t mapped to -1..1
        float saw = 2.f * (t - std::floor(t)) - 1.f;
        float y = centerY - saw * amplitude;

        waveform[i].position = sf::Vector2f(x, y);
        waveform[i].color = sf::Color(0, b, static_cast<sf::Uint8>(b * 0.85f));
    }

    window.draw(waveform);
}