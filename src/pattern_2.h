#ifndef PATTERN_2_H
#define PATTERN_2_H

#include <SFML/Graphics.hpp>

// Generates a static rectangular fractal pattern
// width, height  -> image dimensions
// recursionDepth -> level of detail
sf::Image generatePattern2(unsigned int width, unsigned int height, int recursionDepth);

#endif
