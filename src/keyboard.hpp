#pragma once
#include <cstdint>

enum class Key : uint8_t {
    Invalid = 0xFF,
    Num0 = 0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,
    A,
    B,
    C,
    D,
    E,
    F,
};

class Keyboard
{
  public:
    virtual bool IsKeyPressed(Key key) const = 0;

    Keyboard() = default;

    virtual ~Keyboard() = default;

    Keyboard(const Keyboard&) = delete;
    Keyboard(Keyboard&&) = delete;

    Keyboard& operator=(const Keyboard&) = delete;
    Keyboard& operator=(Keyboard&&) = delete;
};
