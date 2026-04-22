#pragma once
#include <SFML/Graphics.hpp>
#include "engine_state.h"

class Pattern1 {
public:
    Pattern1();
    void draw(sf::RenderWindow& window, const EngineState& state);
};