#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include "pattern_1.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "visual-score-engine");
    window.setFramerateLimit(60);

    Pattern1 pattern;

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.restart().asSeconds();

        pattern.update(dt);

        window.clear(sf::Color::Black);
        pattern.draw(window);
        window.display();
    }

    return 0;
}