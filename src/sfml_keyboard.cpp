#include "sfml_keyboard.hpp"

SFMLKeyboard::SFMLKeyboard()
    : Keyboard(),
      m_mapping{
          sf::Keyboard::X,
          sf::Keyboard::Num1,
          sf::Keyboard::Num2,
          sf::Keyboard::Num3,
          sf::Keyboard::Q,
          sf::Keyboard::W,
          sf::Keyboard::E,
          sf::Keyboard::A,
          sf::Keyboard::S,
          sf::Keyboard::D,
          sf::Keyboard::Z,
          sf::Keyboard::C,
          sf::Keyboard::Num4,
          sf::Keyboard::R,
          sf::Keyboard::F,
          sf::Keyboard::V,
      } {
}

bool SFMLKeyboard::IsKeyPressed(Key key) const {
    return sf::Keyboard::isKeyPressed(
        m_mapping[static_cast<std::array<sf::Keyboard::Key, 16>::size_type>(key)]);
}
