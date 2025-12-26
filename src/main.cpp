#include <SFML/Graphics.hpp>
#include "pattern_1.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Master Visual Engine");
    pattern_1(window);  // draw the static pattern
    return 0;
}
