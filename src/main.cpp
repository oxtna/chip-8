#include "chip.hpp"
#include "sfml_keyboard.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>

static void usage(const char* name) {
    std::cout << "Usage:\n\t" << name << " FILE" << std::endl;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    std::string filename(argv[1]);
    if (!std::filesystem::exists(filename)) {
        std::cerr << "Error: '" << filename << "' does not exist." << std::endl;
        return 1;
    }

    std::ifstream file(filename, std::ios::binary | std::ios::in);
    if (file.fail()) {
        std::cerr << "Error: '" << filename << "' could not be opened." << std::endl;
        return 1;
    }

    sf::SoundBuffer audioBuffer;
    sf::Sound sound;
    if (!audioBuffer.loadFromFile("beep.wav")) {
        std::cerr << "Audio file 'beep.wav' was not found\n";
    }
    else {
        sound.setBuffer(audioBuffer);
    }

    constexpr auto pixelSize = 10;
    sf::RectangleShape pixel({pixelSize, pixelSize});
    pixel.setFillColor(sf::Color::Green);

    sf::RenderWindow window(
        {Chip::Width * pixelSize, Chip::Height * pixelSize},
        "Chip-8 Emulator",
        sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    SFMLKeyboard keyboard;
    Chip chip(keyboard);
    if (!chip.LoadProgram(std::move(file))) {
        std::cerr << "Error: '" << filename << "' could not be loaded." << std::endl;
        return 1;
    }

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

        for (auto i = 0; i < 8; i++) {
            chip.EmulateCycle();
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
