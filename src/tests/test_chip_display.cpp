#include "../chip.hpp"
#include "../keyboard.hpp"
#include <array>

class MockKeyboard final : public Keyboard
{
  public:
    explicit MockKeyboard() = default;

    ~MockKeyboard() = default;

    bool IsKeyPressed(Key key) const override {
        return state[static_cast<std::array<bool, KeyCount>::size_type>(key)];
    }

    std::array<bool, KeyCount> state = {};
};

int main() {
    MockKeyboard keyboard;
    Chip chip(keyboard);

    for (const auto pixel : chip.GetDisplayBuffer()) {
        if (pixel) {
            return 1;
        }
    }

    return 0;
}
