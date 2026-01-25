#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
// #include "pattern_1.h"
#include "chops_module.h"


int main() {
    // Example experiment: moving rectangle
    runExperimentLoop([](sf::RenderWindow& window){
        static sf::RectangleShape rect(sf::Vector2f(100.f, 50.f));
        static bool initialized = false;
        static float x = 0.f;

        if (!initialized) {
            rect.setFillColor(sf::Color::Red);
            rect.setPosition(x, 100.f);
            initialized = true;
        }

        x += 0.5f;
        if (x > window.getSize().x) x = 0;
        rect.setPosition(x, 100.f);

        window.draw(rect);
    });

    return 0;
}