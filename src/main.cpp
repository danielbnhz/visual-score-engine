#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
// #include "pattern_1.h"
#include "pattern_2.h"


int main()
{
    const std::string outputDir = "output_images";

    // sf::RenderWindow window(sf::VideoMode(800, 600), "Master Visual Engine");
    // pattern_1(window);  // draw the static pattern


    // Create directory if it doesn't exist
    std::filesystem::create_directories(outputDir);

    sf::Image img = generatePattern2(2048, 2048, 9);

    std::string outputPath = outputDir + "/pattern_2.png";

    if (!img.saveToFile(outputPath)) {
        std::cerr << "Failed to save image\n";
        return 1;
    }



    return 0;
}
