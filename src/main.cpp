#include <SFML/Graphics.hpp>
#include "pattern_1.h"
#include "pattern_1_blur.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "visual-score-engine");
    window.setFramerateLimit(60);

    Pattern1     pattern;
    Pattern1Blur patternBlur;

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

        window.clear(sf::Color::Black);
        patternBlur.draw(window);  // glow drawn first, under the sharp line
        pattern.draw(window);      // sharp core drawn on top
        window.display();
    }

    return 0;
}