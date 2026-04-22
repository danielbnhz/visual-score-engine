#pragma once
#include <SFML/Graphics.hpp>
#include "engine_state.h"

class Pattern1Blur {
public:
    Pattern1Blur();
    void draw(sf::RenderWindow& window, const EngineState& state);

private:
    float driftPhase;
    float driftSpeed;
    float driftAmount;

    void buildRibbon(sf::VertexArray& va, float halfWidth, sf::Color color,
                     const EngineState& state, float centerY);
};