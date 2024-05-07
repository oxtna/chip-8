#pragma once
#include "keyboard.hpp"
#include <array>
#include <cstdint>
#include <random>
#include <stack>

class Chip
{
  public:
    static constexpr uint16_t Width = 64;
    static constexpr uint16_t Height = 32;
    using DisplayBuffer = std::array<bool, Width * Height>;
    using Memory = std::array<uint8_t, 4096>;

    explicit Chip(Keyboard& keyboard);

    ~Chip() = default;

    Chip(const Chip&) = delete;
    Chip(Chip&&) = delete;

    Chip& operator=(const Chip&) = delete;
    Chip& operator=(Chip&&) = delete;

    const DisplayBuffer& GetDisplayBuffer() const;
    void ProcessInstruction(uint16_t instruction);
    void DecrementTimers();
    bool IsSoundPlaying() const;

  private:
    std::mt19937 m_rng;
    std::uniform_int_distribution<uint32_t> m_distribution;
    Keyboard& m_keyboard;
    DisplayBuffer m_display;
    Memory m_memory;
    std::stack<uint16_t> m_stack;
    // general purpose registers
    union {
        struct
        {
            uint8_t m_V0;
            uint8_t m_V1;
            uint8_t m_V2;
            uint8_t m_V3;
            uint8_t m_V4;
            uint8_t m_V5;
            uint8_t m_V6;
            uint8_t m_V7;
            uint8_t m_V8;
            uint8_t m_V9;
            uint8_t m_VA;
            uint8_t m_VB;
            uint8_t m_VC;
            uint8_t m_VD;
            uint8_t m_VE;
            uint8_t m_VF;
        };
        uint8_t m_V[16];
    };
    // program counter
    uint16_t m_PC;
    // address pointer
    uint16_t m_I;
    // stack pointer
    uint8_t m_SP;
    // delay timer register
    uint8_t m_DT;
    // sound timer register
    uint8_t m_ST;
};
