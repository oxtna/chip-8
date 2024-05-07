#include "chip.hpp"
#include "sfml_keyboard.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::SoundBuffer audioBuffer;
    sf::Sound sound;
    if (!audioBuffer.loadFromFile("beep.wav")) {
        std::cerr << "Audio file 'beep.wav' was not found\n";
    }
    else {
        sound.setBuffer(audioBuffer);
    }
    sf::RenderWindow window(
        {640u, 320u}, "Chip-8 Emulator", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    SFMLKeyboard keyboard;
    Chip chip(keyboard);

    constexpr auto pixelSize = 10;
    sf::RectangleShape pixel({pixelSize, pixelSize});
    pixel.setFillColor(sf::Color::White);

    while (window.isOpen()) {
        if (chip.IsSoundPlaying() && audioBuffer.getSampleCount() > 0) {
            sound.play();
        }
        chip.DecrementTimers();
        auto event = sf::Event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        auto& displayBuffer = chip.GetDisplayBuffer();
        for (std::size_t i = 0; i < displayBuffer.size(); i++) {
            if (displayBuffer[i]) {
                pixel.setPosition(
                    static_cast<float>(i % Chip::Width * pixelSize),
                    static_cast<float>(i / Chip::Width * pixelSize));
                window.draw(pixel);
            }
        }
        window.display();
    }

    return 0;
}
