#pragma once
#include "keyboard.hpp"
#include <SFML/Graphics.hpp>
#include <array>

class SFMLKeyboard final : public Keyboard
{
  public:
    explicit SFMLKeyboard();

    ~SFMLKeyboard() = default;

    SFMLKeyboard(const SFMLKeyboard&) = delete;
    SFMLKeyboard(SFMLKeyboard&&) = delete;

    SFMLKeyboard& operator=(const SFMLKeyboard&) = delete;
    SFMLKeyboard& operator=(SFMLKeyboard&&) = delete;

    bool IsKeyPressed(Key key) const override;

  private:
    std::array<sf::Keyboard::Key, KeyCount> m_mapping;
};
