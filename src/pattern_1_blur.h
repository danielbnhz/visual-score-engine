#pragma once
#include <SFML/Graphics.hpp>

class Pattern1Blur {
public:
    Pattern1Blur();
    void update(float dt);
    void draw(sf::RenderWindow& window);

private:
    float phase;
    float brightnessPhase;
    int   pointCount;
    float frequency;
    float amplitude;
    float speed;
    float brightnessSpeed;
    float driftPhase;
    float driftSpeed;
    float driftAmount;
    void buildRibbon(sf::VertexArray& va, float halfWidth, sf::Color color);
};