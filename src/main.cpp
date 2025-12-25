#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Floating Triangles");

    sf::ConvexShape triangle;
    triangle.setPointCount(3);
    triangle.setPoint(0, sf::Vector2f(0.f, 0.f));
    triangle.setPoint(1, sf::Vector2f(100.f, 0.f));
    triangle.setPoint(2, sf::Vector2f(50.f, 100.f));
    triangle.setFillColor(sf::Color::Green);
    triangle.setPosition(350.f, 250.f);

    sf::Vector2f velocity(0.2f, 0.15f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        triangle.move(velocity);

        sf::Vector2f pos = triangle.getPosition();
        if (pos.x < 0 || pos.x + 100 > 800)
            velocity.x = -velocity.x;
        if (pos.y < 0 || pos.y + 100 > 600)
            velocity.y = -velocity.y;

        window.clear(sf::Color::Black);
        window.draw(triangle);
        window.display();
    }

    return 0;
}
