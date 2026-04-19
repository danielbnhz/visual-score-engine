#pragma once
#include <SFML/Graphics.hpp>

class Pattern1 {
public:
    Pattern1();
    void update(float dt);
    void draw(sf::RenderWindow& window);

private:
    float phase;
    float brightnessPhase;
    int pointCount;
    float frequency;
    float amplitude;
    float speed;
    float brightnessSpeed;
};