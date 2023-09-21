#include "chip.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    Chip chip;

    auto window = sf::RenderWindow{{640u, 320u}, "Chip-8 Emulator"};
    window.setFramerateLimit(60);
    while (window.isOpen()) {
        auto event = sf::Event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        window.display();
    }

    return 0;
}
