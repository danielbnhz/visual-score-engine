#pragma once


#include <functional>
#include <SFML/Graphics.hpp>

using ExperimentFunc = std::function<void(sf::RenderWindow&)>;

// Main loop for experiments
void runExperimentLoop(ExperimentFunc experiment, unsigned int width = 800, unsigned int height = 600);

