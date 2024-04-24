#include "chip.hpp"

Chip::Chip()
    : m_rng(std::random_device{}()),
      m_distribution(0, 255),
      m_keyboard{},
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
        m_V[(instruction & 0x0F00) >> 8] =
            static_cast<uint8_t>(m_distribution(m_rng) & (instruction & 0x00FF));
        break;
    case 0xD000:  // Dxyn - DRW Vx, Vy, nibble
        // todo: read n bytes from memory starting at address I, then display those bytes
        // as sprites at (Vx, Vy), sprites are XORed onto the existing screen, if this
        // erases any pixels, set VF to 1, otherwise set VF to 0
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
            m_V[(instruction & 0x0F00) >> 8] = m_DT;
            break;
        case 0x000A:  // Fx0A - LD Vx, K
            // todo: wait for key press and store its value in Vx
            break;
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
            m_memory[m_I] = m_V[(instruction & 0x0F00) >> 8] / 100;
            m_memory[m_I + 1] = (m_V[(instruction & 0x0F00) >> 8] / 10) % 10;
            m_memory[m_I + 2] = m_V[(instruction & 0x0F00) >> 8] % 100;
            break;
        case 0x0055:  // Fx55 - LD [I], Vx
            for (int i = 0; i <= (instruction & 0x0F00) >> 8; i++) {
                m_memory[m_I + i] = m_V[i];
            }
            break;
        case 0x0065:  // Fx65 - LD Vx, [I]
            for (int i = 0; i <= (instruction & 0x0F00) >> 8; i++) {
                m_V[i] = m_memory[m_I + i];
            }
            break;
        }
        break;
    }
}
