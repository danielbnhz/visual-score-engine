#include <cmath>
#include <glad/glad.h>
#include <SFML/Graphics.hpp>
#include "engine_state.h"
#include "pattern_1.h"
#include "pattern_1_blur.h"
#include "pattern_2.h"

// ── keyboard input ────────────────────────────────────────
void handleInput(EngineState& state, float dt) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        state.amplitude = std::min(state.amplitude + 80.f * dt, 300.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        state.amplitude = std::max(state.amplitude - 80.f * dt, 20.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        state.frequency += 2.f * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        state.frequency  = std::max(state.frequency - 2.f * dt, 0.5f);
}

int main() {
    sf::ContextSettings settings;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.depthBits    = 24;

    sf::RenderWindow window(
        sf::VideoMode(1280, 720),
        "visual-score-engine",
        sf::Style::Default,
        settings
    );
    window.setFramerateLimit(60);

    if (!gladLoadGL()) return -1;

    // ── single source of truth ────────────────
    EngineState state;

    Pattern1     pattern1;
    Pattern1Blur patternBlur;
    Pattern2     pattern2;
    pattern2.init();

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // one-shot events live here, not in handleInput
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space)
                    state.triggerKick();
                if (event.key.code == sf::Keyboard::R)
                    state = EngineState{};   // full reset to defaults
            }
        }

        float dt = clock.restart().asSeconds();

        handleInput(state, dt);
        state.update(dt);   // one update drives everything

        window.clear(sf::Color::Black);

        patternBlur.draw(window, state);
        pattern1.draw(window, state);

        window.resetGLStates();
        pattern2.draw(window, state);

        window.setActive(true);
        window.display();
    }

    return 0;
}