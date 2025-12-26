#include "pattern_1.h"
#include <vector>
#include <cstdlib>
#include <ctime>

// Implementation of the static black & purple pattern
void pattern_1(sf::RenderWindow& window)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    window.clear(sf::Color::Black);

    const int rows = 15;
    const int cols = 20;
    const float triangleSize = 60.f;
    const float paddingX = 50.f;
    const float paddingY = 40.f;

    std::vector<sf::ConvexShape> triangles;

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            sf::ConvexShape triangle;
            triangle.setPointCount(3);

            float angle = std::rand() % 360;
            triangle.setPoint(0, sf::Vector2f(0.f, 0.f));
            triangle.setPoint(1, sf::Vector2f(triangleSize, 0.f));
            triangle.setPoint(2, sf::Vector2f(triangleSize / 2.f, triangleSize));

            float x = j * paddingX + (std::rand() % 20 - 10);
            float y = i * paddingY + (std::rand() % 20 - 10);
            triangle.setPosition(x, y);

            sf::Uint8 r = 50 + std::rand() % 50;      // dark purple tones
            sf::Uint8 g = 0;
            sf::Uint8 b = 100 + std::rand() % 155;
            sf::Uint8 alpha = 180;

            triangle.setFillColor(sf::Color(r, g, b, alpha));
            triangle.setRotation(angle);

            triangles.push_back(triangle);
        }
    }

    for (const auto& t : triangles)
        window.draw(t);

    window.display();

    // Wait until window is closed
    sf::Event event;
    while (window.waitEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}
