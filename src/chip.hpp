#pragma once
#include <array>
#include <cstdint>

class Chip
{
  public:
    Chip();
    void ProcessInstruction(uint16_t instruction);

  private:
    std::array<int8_t, 64 * 32> m_display;
    std::array<uint8_t, 4096> m_memory;
    std::array<uint16_t, 16> m_stack;
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
