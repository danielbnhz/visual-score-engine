#include <glad/glad.h>          // GLAD must come before any SFML OpenGL headers
#include <SFML/Graphics.hpp>
#include "pattern_1.h"
#include "pattern_1_blur.h"
#include "pattern_2.h"

int main() {
    // Request an OpenGL 3.3 core context through SFML
    sf::ContextSettings settings;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;
    settings.depthBits = 24;

    sf::RenderWindow window(
        sf::VideoMode(1280, 720),
        "visual-score-engine",
        sf::Style::Default,
        settings
    );
    window.setFramerateLimit(60);

    // Initialise GLAD against the live GL context
    if (!gladLoadGL()) {
        return -1;
    }

    Pattern1     pattern;
    Pattern1Blur patternBlur;
    Pattern2     pattern2;
    pattern2.init();            // must happen after gladLoadGL()

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.restart().asSeconds();

        pattern.update(dt);
        patternBlur.update(dt);
        pattern2.update(dt);

        window.clear(sf::Color::Black);

        // SFML draws first (2D layer)
        patternBlur.draw(window);
        pattern.draw(window);

        window.resetGLStates();   // ← neutralise SFML's state pollution
        pattern2.draw(window);

        window.setActive(true);
        window.display();
    }

    return 0;
}