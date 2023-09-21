#include "chip.hpp"

Chip::Chip()
    : m_keyboard{},
      m_display{},
      m_memory{},
      m_stack{},
      m_V{},
      m_PC{},
      m_I{},
      m_SP{},
      m_DT{},
      m_ST{} {
}

void Chip::ProcessInstruction(uint16_t instruction) {
    switch (instruction & 0xF000) {
    case 0x0000:
        if (instruction == 0x00E0) {  // 00E0 - CLS
            m_display = {};
        }
        else if (instruction == 0x00EE) {  // 00EE - RET
            m_PC = m_stack.top();
            m_stack.pop();
            m_SP--;
        }
        else {  // 0nnn - SYS addr
            // jump to machine code routine at nnn
            // todo: prolly ignore this
        }
        break;
    case 0x1000:  // 1nnn - JP addr
        m_PC = instruction & 0x0FFF;
        break;
    case 0x2000:  // 2nnn - CALL addr
        m_SP++;
        m_stack.push(m_PC);
        m_PC = instruction & 0x0FFF;
        break;
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
        {
            unsigned int sum =
                m_V[(instruction & 0x0F00) >> 8] + m_V[(instruction & 0x00F0) >> 4];
            m_V[(instruction & 0x0F00) >> 8] = sum;
            m_V[0xF] = (sum & 0x0100) >> 8;
        } break;
        case 0x0005:  // 8xy5 - SUB Vx, Vy
        {
            bool borrowed =
                m_V[(instruction & 0x0F00) >> 8] < m_V[(instruction & 0x00F0) >> 4];
            m_V[(instruction & 0x0F00) >> 8] -= m_V[(instruction & 0x00F0) >> 4];
            m_V[0xF] = borrowed ? 0 : 1;
        } break;
        case 0x0006:  // 8xy6 - SHR Vx, Vy
            m_V[0xF] = m_V[(instruction & 0x0F00) >> 8] & 0x01;
            m_V[(instruction & 0x0F00) >> 8] >>= 1;
            break;
        case 0x0007:  // 8xy7 - SUBN Vx, Vy
        {
            bool borrowed =
                m_V[(instruction & 0x0F00) >> 8] > m_V[(instruction & 0x00F0) >> 4];
            m_V[(instruction & 0x00F0) >> 4] -= m_V[(instruction & 0x0F00) >> 8];
            m_V[0xF] = borrowed ? 0 : 1;
        } break;
        case 0x000E:  // 8xyE - SHL Vx, Vy
            m_V[0xF] = (m_V[(instruction & 0x0F00) >> 8] & 0x80) >> 7;
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
        break;
    case 0xD000:  // Dxyn - DRW Vx, Vy, nibble
        break;
    case 0xE000:
        switch (instruction & 0x00FF) {
        case 0x009E:  // Ex9E - SKP Vx
            if (m_keyboard[m_V[(instruction & 0x0F00) >> 8]]) {
                m_PC += 2;
            }
            break;
        case 0x00A1:  // ExA1 - SKNP Vx
            if (!m_keyboard[m_V[(instruction & 0x0F00) >> 8]]) {
                m_PC += 2;
            }
            break;
        }
        break;
    case 0xF000:
        switch (instruction & 0x00FF) {
        case 0x0007:  // Fx07 - LD Vx, DT
            break;
        case 0x000A:  // Fx0A - LD Vx, K
            break;
        case 0x0015:  // Fx15 - LD DT, Vx
            break;
        case 0x0018:  // Fx18 - LD ST, Vx
            break;
        case 0x001E:  // Fx1E - ADD I, Vx
            break;
        case 0x0029:  // Fx29 - LD F, Vx
            break;
        case 0x0033:  // Fx33 - LD B, Vx
            break;
        case 0x0055:  // Fx55 - LD [I], Vx
            break;
        case 0x0065:  // Fx65 - LD Vx, [I]
            break;
        }
        break;
    }
}
