#include <SFML/Graphics.hpp>
#include <random>

// --------------------------------------------------
// Internal recursive function
// --------------------------------------------------
static void subdivideRect(
    sf::Image& image,
    int x,
    int y,
    int w,
    int h,
    int depth,
    int maxDepth,
    std::mt19937& rng
) {
    if (depth >= maxDepth || w <= 2 || h <= 2)
        return;

    // Random chance to stop early (adds uneven density)
    std::uniform_real_distribution<float> stopDist(0.f, 1.f);
    if (stopDist(rng) < 0.1f) return; // 10% early stop

    // Randomly decide orientation
    std::uniform_int_distribution<int> orientDist(0, 1);
    bool vertical = orientDist(rng) == 1;

    int split;
    if (vertical) {
        // Random split between 1/3 and 2/3 width
        std::uniform_int_distribution<int> splitDist(w / 3, 2 * w / 3);
        split = splitDist(rng);

        // Draw dividing line
        for (int j = y; j < y + h; ++j)
            image.setPixel(x + split, j, sf::Color::White);

        // Recurse
        subdivideRect(image, x, y, split, h, depth + 1, maxDepth, rng);
        subdivideRect(image, x + split, y, w - split, h, depth + 1, maxDepth, rng);

    } else {
        // Random split between 1/3 and 2/3 height
        std::uniform_int_distribution<int> splitDist(h / 3, 2 * h / 3);
        split = splitDist(rng);

        // Draw dividing line
        for (int i = x; i < x + w; ++i)
            image.setPixel(i, y + split, sf::Color::White);

        // Recurse
        subdivideRect(image, x, y, w, split, depth + 1, maxDepth, rng);
        subdivideRect(image, x, y + split, w, h - split, depth + 1, maxDepth, rng);
    }
}

// --------------------------------------------------
// Public interface
// --------------------------------------------------
sf::Image generatePattern2(
    unsigned int width,
    unsigned int height,
    int recursionDepth
) {
    sf::Image image;
    image.create(width, height, sf::Color::Black);

    // Outer frame
    for (unsigned int x = 0; x < width; ++x) {
        image.setPixel(x, 0, sf::Color::White);
        image.setPixel(x, height - 1, sf::Color::White);
    }
    for (unsigned int y = 0; y < height; ++y) {
        image.setPixel(0, y, sf::Color::White);
        image.setPixel(width - 1, y, sf::Color::White);
    }

    // Random number generator
    std::mt19937 rng(std::random_device{}()); // different each run
    // For reproducible patterns, use fixed seed: std::mt19937 rng(42);

    // Recursive subdivision
    subdivideRect(image, 1, 1, width - 2, height - 2, 0, recursionDepth, rng);

    return image;
}
