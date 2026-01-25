#include "chops_module.h"

void runExperimentLoop(ExperimentFunc experiment, unsigned int width, unsigned int height) {
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML Chops Module");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        // Call the experiment each frame
        experiment(window);

        window.display();
    }
}
