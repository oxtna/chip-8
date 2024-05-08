#include "chip.hpp"
#include <iomanip>
#include <iostream>
#include <stdexcept>

Chip::Chip(Keyboard& keyboard)
    : m_rng(std::random_device{}()),
      m_distribution(0, 255),
      m_keyboard(keyboard),
      m_display{},
      m_memory{
          // 0
          0xF0,
          0x90,
          0x90,
          0x90,
          0xF0,
          // 1
          0x20,
          0x60,
          0x20,
          0x20,
          0x70,
          // 2
          0xF0,
          0x10,
          0xF0,
          0x80,
          0xF0,
          // 3
          0xF0,
          0x10,
          0xF0,
          0x10,
          0xF0,
          // 4
          0x90,
          0x90,
          0xF0,
          0x10,
          0x10,
          // 5
          0xF0,
          0x80,
          0xF0,
          0x10,
          0xF0,
          // 6
          0xF0,
          0x80,
          0xF0,
          0x90,
          0xF0,
          // 7
          0xF0,
          0x10,
          0x20,
          0x40,
          0x40,
          // 8
          0xF0,
          0x90,
          0xF0,
          0x90,
          0xF0,
          // 9
          0xF0,
          0x90,
          0xF0,
          0x10,
          0xF0,
          // A
          0xF0,
          0x90,
          0xF0,
          0x90,
          0x90,
          // B
          0xE0,
          0x90,
          0xE0,
          0x90,
          0xE0,
          // C
          0xF0,
          0x80,
          0x80,
          0x80,
          0xF0,
          // D
          0xE0,
          0x90,
          0x90,
          0x90,
          0xE0,
          // E
          0xF0,
          0x80,
          0xF0,
          0x80,
          0xF0,
          // F
          0xF0,
          0x80,
          0xF0,
          0x80,
          0x80,
      },
      m_stack{},
      m_V{},
      m_PC{},
      m_I{},
      m_SP{},
      m_DT{},
      m_ST{} {
}

const Chip::DisplayBuffer& Chip::GetDisplayBuffer() const {
    return m_display;
}

bool Chip::LoadProgram(std::ifstream&& file) {
    constexpr auto MaxFileSize = MemoryCapacity - ProgramCounterStart;
    if (file.fail()) {
        return false;
    }
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    if (size > MaxFileSize) {
        return false;
    }
    file.seekg(0, std::ios::beg);
    file.read(
        reinterpret_cast<char*>(m_memory.data()) + ProgramCounterStart, MaxFileSize);
    m_PC = ProgramCounterStart;
    return true;
}

void Chip::EmulateCycle() {
    uint16_t instruction =
        (m_memory[m_PC] << 8) | m_memory[static_cast<Memory::size_type>(m_PC) + 1];
    ProcessInstruction(instruction);
}

void Chip::ProcessInstruction(uint16_t instruction) {
    switch (instruction & 0xF000) {
    case 0x0000:
        if (instruction == 0x00E0) {  // 00E0 - CLS
            m_display = {};
        }
        else if (instruction == 0x00EE) {  // 00EE - RET
            if (m_SP == 0) {
                throw std::runtime_error("Stack underflow");
            }
            m_PC = m_stack.top();
            m_stack.pop();
            m_SP--;
            return;
        }
        // SKIP: 0nnn - SYS addr
        break;
    case 0x1000:  // 1nnn - JP addr
        m_PC = instruction & 0x0FFF;
        return;
    case 0x2000:  // 2nnn - CALL addr
        if (m_SP == 16) {
            throw std::runtime_error("Stack overflow");
        }
        m_stack.push(m_PC + 2);
        m_PC = instruction & 0x0FFF;
        m_SP++;
        return;
    case 0x3000:  // 3xkk - SE Vx, byte
        if (m_V[(instruction & 0x0F00) >> 8] == (instruction & 0x00FF)) {
            m_PC += 2;
        }
        break;
    case 0x4000:  // 4xkk - SNE Vx, byte
        if (m_V[(instruction & 0x0F00) >> 8] != (instruction & 0x00FF)) {
            m_PC += 2;
        }
        break;
    case 0x5000:  // 5xy0 - SE Xv, Vy
        if (m_V[(instruction & 0x0F00) >> 8] == m_V[(instruction & 0x00F0) >> 4]) {
            m_PC += 2;
        }
        break;
    case 0x6000:  // 6xkk - LD Vx, byte
        m_V[(instruction & 0x0F00) >> 8] = instruction & 0x00FF;
        break;
    case 0x7000:  // 7xkk - ADD Vx, byte
        m_V[(instruction & 0x0F00) >> 8] += instruction & 0x00FF;
        break;
    case 0x8000:
        switch (instruction & 0x000F) {
        case 0x0000:  // 8xy0 - LD Vx, Vy
            m_V[(instruction & 0x0F00) >> 8] = m_V[(instruction & 0x00F0) >> 4];
            break;
        case 0x0001:  // 8xy1 - OR Vx, Vy
            m_V[(instruction & 0x0F00) >> 8] |= m_V[(instruction & 0x00F0) >> 4];
            break;
        case 0x0002:  // 8xy2 - AND Vx, Vy
            m_V[(instruction & 0x0F00) >> 8] &= m_V[(instruction & 0x00F0) >> 4];
            break;
        case 0x0003:  // 8xy3 - XOR Vx, Vy
            m_V[(instruction & 0x0F00) >> 8] ^= m_V[(instruction & 0x00F0) >> 4];
            break;
        case 0x0004:  // 8xy4 - ADD Vx, Vy
            if (m_V[(instruction & 0x0F00) >> 8] >
                0xFF - m_V[(instruction & 0x00F0) >> 4]) {
                m_VF = 1;
            }
            else {
                m_VF = 0;
            }
            m_V[(instruction & 0x0F00) >> 8] += m_V[(instruction & 0x00F0) >> 4];
            break;
        case 0x0005:  // 8xy5 - SUB Vx, Vy
        {
            bool borrowed =
                m_V[(instruction & 0x0F00) >> 8] < m_V[(instruction & 0x00F0) >> 4];
            m_V[(instruction & 0x0F00) >> 8] -= m_V[(instruction & 0x00F0) >> 4];
            m_VF = borrowed ? 0 : 1;
        } break;
        case 0x0006:  // 8xy6 - SHR Vx, Vy
            m_VF = m_V[(instruction & 0x0F00) >> 8] & 0x01;
            m_V[(instruction & 0x0F00) >> 8] >>= 1;
            break;
        case 0x0007:  // 8xy7 - SUBN Vx, Vy
        {
            bool borrowed =
                m_V[(instruction & 0x0F00) >> 8] > m_V[(instruction & 0x00F0) >> 4];
            m_V[(instruction & 0x00F0) >> 4] -= m_V[(instruction & 0x0F00) >> 8];
            m_VF = borrowed ? 0 : 1;
        } break;
        case 0x000E:  // 8xyE - SHL Vx, Vy
            m_VF = (m_V[(instruction & 0x0F00) >> 8] & 0x80) >> 7;
            m_V[(instruction & 0x0F00) >> 8] <<= 1;
            break;
        }
        break;
    case 0x9000:  // 9xy0 - SNE Vx, Vy
        if (m_V[(instruction & 0x0F00) >> 8] != m_V[(instruction & 0x00F0) >> 4]) {
            m_PC += 2;
        }
        break;
    case 0xA000:  // Annn - LD I, addr
        m_I = instruction & 0x0FFF;
        break;
    case 0xB000:  // Bnnn - JP V0, addr
        m_PC = (instruction & 0x0FFF) + m_V[0];
        break;
    case 0xC000:  // Cxkk - RND Vx, byte
        m_V[(instruction & 0x0F00) >> 8] =
            static_cast<uint8_t>(m_distribution(m_rng) & (instruction & 0x00FF));
        break;
    case 0xD000:  // Dxyn - DRW Vx, Vy, nibble
    {
        m_VF = 0;
        const auto x = m_V[(instruction & 0x0F00) >> 8];
        const auto y = m_V[(instruction & 0x00F0) >> 4];
        for (uint16_t byteIndex = 0; byteIndex < (instruction & 0x000F); byteIndex++) {
            for (uint16_t bitIndex = 0; bitIndex < 8; bitIndex++) {
                auto source =
                    (m_memory[static_cast<Memory::size_type>(m_I) + byteIndex] >>
                     (7 - bitIndex)) &
                    0x01;
                if (source) {
                    auto destinationIndex =
                        (x + bitIndex) % Width + (y + byteIndex) % Height * Width;
                    if (m_display[destinationIndex]) {
                        m_VF = 1;
                    }
                    m_display[destinationIndex] ^= true;
                }
            }
        }
    } break;
    case 0xE000:
        switch (instruction & 0x00FF) {
        case 0x009E:  // Ex9E - SKP Vx
            if (m_keyboard.IsKeyPressed(
                    static_cast<Key>(m_V[(instruction & 0x0F00) >> 8]))) {
                m_PC += 2;
            }
            break;
        case 0x00A1:  // ExA1 - SKNP Vx
            if (!m_keyboard.IsKeyPressed(
                    static_cast<Key>(m_V[(instruction & 0x0F00) >> 8]))) {
                m_PC += 2;
            }
            break;
        }
        break;
    case 0xF000:
        switch (instruction & 0x00FF) {
        case 0x0007:  // Fx07 - LD Vx, DT
            m_V[(instruction & 0x0F00) >> 8] = m_DT;
            break;
        case 0x000A:  // Fx0A - LD Vx, K
        {
            bool wasKeyPressed = false;
            for (uint8_t i = static_cast<uint8_t>(Key::Num0);
                 i <= static_cast<uint8_t>(Key::F);
                 i++) {
                if (m_keyboard.IsKeyPressed(static_cast<Key>(i))) {
                    m_V[(instruction & 0x0F00) >> 8] = i;
                    wasKeyPressed = true;
                }
            }
            // If no key is pressed, do not increment the program counter
            if (!wasKeyPressed) {
                return;
            }
        } break;
        case 0x0015:  // Fx15 - LD DT, Vx
            m_DT = m_V[(instruction & 0x0F00) >> 8];
            break;
        case 0x0018:  // Fx18 - LD ST, Vx
            m_ST = m_V[(instruction & 0x0F00) >> 8];
            break;
        case 0x001E:  // Fx1E - ADD I, Vx
            m_I += m_V[(instruction & 0x0F00) >> 8];
            break;
        case 0x0029:  // Fx29 - LD F, Vx
            m_I = 5 * m_V[(instruction & 0x0F00) >> 8];
            break;
        case 0x0033:  // Fx33 - LD B, Vx
        {
            const auto x = m_V[(instruction & 0x0F00) >> 8];
            m_memory[m_I] = x / 100;
            m_memory[static_cast<Memory::size_type>(m_I) + 1] = (x / 10) % 10;
            m_memory[static_cast<Memory::size_type>(m_I) + 2] = x % 10;
        } break;
        case 0x0055:  // Fx55 - LD [I], Vx
            for (int i = 0; i <= (instruction & 0x0F00) >> 8; i++) {
                m_memory[static_cast<Memory::size_type>(m_I) + i] = m_V[i];
            }
            break;
        case 0x0065:  // Fx65 - LD Vx, [I]
            for (int i = 0; i <= (instruction & 0x0F00) >> 8; i++) {
                m_V[i] = m_memory[static_cast<Memory::size_type>(m_I) + i];
            }
            break;
        }
        break;
    default:
        throw std::runtime_error("Unknown instruction");
    }
    m_PC += 2;
}

void Chip::DecrementTimers() {
    if (m_DT > 0) {
        m_DT--;
    }
    if (m_ST > 0) {
        m_ST--;
    }
}

bool Chip::IsSoundPlaying() const {
    return m_ST > 0;
}
