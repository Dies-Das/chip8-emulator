#include "chip8.h"
#include <stdexcept>
#include <cstdlib>
#include <string.h>
#include <time.h>
#include <fstream>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

uint8_t getchipkey(int key)
{
    switch (key)
    {
    case 49:
        return 1;
    case 50:
        return 2;
    case 51:
        return 3;
    case 52:
        return 12;
    case 113:
        return 4;
    case 119:
        return 5;
    case 101:
        return 6;
    case 114:
        return 13;
    case 97:
        return 7;
    case 115:
        return 8;
    case 100:
        return 9;
    case 102:
        return 14;
    case 121:
        return 10;
    case 120:
        return 0;
    case 99:
        return 11;
    case 118:
        return 15;
    default:
        return -1;
    }
}
Chip8::Chip8(std::string path)
{
    std::srand(time(0));
    const std::array<uint8_t, 80> font = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F}
    };
    std::copy(font.begin(), font.end(), &ram.at(0x50));
    std::ifstream inputFile(path, std::ifstream::binary);
    // get length of file
    inputFile.seekg(0, inputFile.end);
    int fileSize = inputFile.tellg();
    inputFile.seekg(0, inputFile.beg);

    // allocate memory for file content
    char *buffer = new char[fileSize];

    // read content of binary file
    inputFile.read(buffer, fileSize);

    // close input file
    inputFile.close();
    std::copy(buffer, buffer + fileSize, &ram.at(512));
    delete[] buffer;
};

void Chip8::loop()
{

    int sleeptime = (int)(3000);

    initscr();
    curs_set(0);
    nodelay(stdscr, true);
    keypad(stdscr, true);
    cbreak();
    noecho();
    unsigned int counter = 0;
    while (true)
    {
        auto val = getchipkey(wgetch(stdscr));
        if(val!=255)
            this->key = val;

        bool redraw = 0;

        auto instr = this->fetch();
        redraw = this->execute(instr);
        if (redraw)
        {
            for (int x = 0; x < 32; x++)

            {
                for (int y = 0; y < 64; y++)
                {
                    // sf::RectangleShape rect(sf::Vector2f(4, 4));
                    // rect.setPosition(sf::Vector2f(y * 4, x * 4));
                    if (this->display.values[x][y])
                    {
                        mvaddch(x, y, char(0x2588));
                        // std::cout << "1 ";
                        // rect.setFillColor(sf::Color(255, 255, 255, 255));
                    }
                    else
                    {
                        mvaddch(x, y, char(0x20));
                        // std::cout << "0 ";
                        // rect.setFillColor(sf::Color(0, 0, 0, 255));
                    }
                }
                // std::cout << std::endl;
            }
        }
        if (this->soundtimer > 0)
            beep();
        if (counter % 10 == 0)
        
        
        {
            refresh();
            if (this->delaytimer > 0)
                this->delaytimer--;
            if (this->soundtimer > 0)
                this->soundtimer--;
        }
        counter++;
        usleep(sleeptime);
    }
    endwin();
};

int Chip8::execute(uint16_t instr)
{

    const uint16_t mask = 0xF000;
    auto fnib = mask & instr;
    fnib = fnib >> 12;
    auto reg = 0;
    uint16_t reg1 = 0;
    uint16_t reg2 = 0;
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t n = 0;
    int redraw = 0;
    uint8_t carry = 0;
    switch (fnib)
    {
    case 0:
        if (instr == 0x00E0)
        {
            memset(this->display.values, 0, 64 * 32);
            redraw = 1;
        }
        else if (instr == 0x00EE)
        {
            uint16_t newinstr = this->stack.top();
            this->stack.pop();
            this->PC = newinstr;
        }
        break;
    case 1:
        this->PC = (instr & 0x0FFF);
        break;
    case 2:
        this->stack.push(this->PC);
        this->PC = (instr & 0x0FFF);
        break;
    case 3:
        reg = (instr & 0x0F00) >> 8;
        if (this->V[reg] == (instr & 0x00FF))
        {
            this->PC += 2;
        }
        break;
    case 4:
        reg = (instr & 0x0F00) >> 8;
        if (this->V[reg] != (instr & 0x00FF))
        {
            this->PC += 2;
        }
        break;
    case 5:
        reg1 = (instr & 0x0F00) >> 8;
        reg2 = (instr & 0x00F0) >> 4;
        if (this->V[reg1] == this->V[reg2])
        {
            this->PC += 2;
        }
        break;
    case 6:
        reg = (instr & 0x0F00) >> 8;
        this->V[reg] = (instr & 0x00FF);
        break;
    case 7:
        reg = (instr & 0x0F00) >> 8;
        this->V[reg] += (instr & 0x00FF);
        break;
    case 8:
        reg1 = (instr & 0x0F00) >> 8;
        reg2 = (instr & 0x00F0) >> 4;
        switch (instr & 0x000F)
        {
        case 0:
            this->V[reg1] = this->V[reg2];
            break;
        case 1:
            this->V[reg1] |= this->V[reg2];
            break;
        case 2:
            this->V[reg1] &= this->V[reg2];
            break;
        case 3:
            this->V[reg1] ^= this->V[reg2];
            break;
        case 4:
            if (255 - this->V[reg1] < this->V[reg2])
            {
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            this->V[reg1] += this->V[reg2];
            this->V[0xF] = carry;
            break;
        case 5:

            if (this->V[reg1] >= this->V[reg2])
            {
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            this->V[reg1] -= this->V[reg2];
            this->V[0xF] = carry;
            break;
        case 6:
            this->V[0xF] = this->V[reg1] & 0x0001;
            this->V[reg1] >>= 1;
            break;
        case 7:
            if (this->V[reg2] >= this->V[reg1])
            {
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            this->V[reg1] = this->V[reg2] - this->V[reg1];
            this->V[0xF] = carry;
            break;
        case 0xE:
            carry = this->V[reg1] >> 7;
            this->V[reg1] <<= 1;
            this->V[0xF] = carry;
            break;
        }
        break;
    case 9:
        reg1 = (instr & 0x0F00) >> 8;
        reg2 = (instr & 0x00F0) >> 4;
        if (this->V[reg1] != this->V[reg2])
        {
            this->PC += 2;
        }
        break;
    case 0xA:
        this->I = instr & 0x0FFF;
        break;
    case 0xB:
        this->PC = instr & 0x0FFF;
        break;
    case 0xC:
        reg = (instr & 0x0F00) >> 8;
        this->V[reg] = ((uint16_t)rand()) & (instr & 0x00FF);
        break;
    case 0xD:
        reg1 = (instr & 0x0F00) >> 8;
        reg2 = (instr & 0x00F0) >> 4;

        x = this->V[reg2] & 31;
        this->V[0xF] = 0;
        n = instr & 0xF;

        for (int k = 0; k < n; k++)
        {
            const auto sdata = this->ram[this->I + k];
            uint8_t whichbit = 128;
            y = this->V[reg1] & 63;
            for (int j = 0; j < 8; j++)
            {
                auto sbit = whichbit & sdata;
                sbit >>= (7 - j);
                if (sbit && this->display.values[x][y])
                {
                    this->display.values[x][y] = 0;
                    this->V[0xF] = 1;
                }
                else if (sbit && !this->display.values[x][y])
                {
                    this->display.values[x][y] = 1;
                }
                if (y == 63)
                {
                    break;
                }
                y++;
                whichbit >>= 1;
            }
            if (x == 31)
            {
                break;
            }
            x++;
        }
        redraw = 1;
        break;
    case 0xE:
        reg = (instr & 0x0F00) >> 8;
        // std::cout << (int)this->key << std::endl;
        // std::cout << "opcode: " << std::hex << (int) (instr & 0x00FF) << std::endl;
        if ((instr & 0x00FF) == 0x9E)
        {

            int ch = this->key;
            // int ch = wgetch(stdscr);
            if (!(ch == 255))
            {
                // std::cout << "character code: " << ch << "Desired code: " << (int)this->V[reg] << std::endl;
                if ((ch) == this->V[reg])
                {
                    this->PC += 2;
                    this->key=255;
                }
            }
        }
        else if ((instr & 0x00FF) == 0xA1)
        {
            int ch = this->key;
            // int ch = wgetch(stdscr);
            if(true)// (!(ch == 255))
            {
                // std::cout << "character code: " << ch << std::endl;
                if (!((ch) == this->V[reg]))
                {
                    this->PC += 2;
                }
            }
        }
        break;
    case 0xF:
        reg = (instr & 0x0F00) >> 8;

        switch (instr & 0x00FF)
        {
        case 0x07:
            this->V[reg] = this->delaytimer;
            break;
        case 0x15:
            this->delaytimer = this->V[reg];
            break;
        case 0x18:
            this->soundtimer = this->V[reg];
            break;
        case 0x1E:

            if (this->V[reg] + this->I > 0x0FFF)
            {
                this->V[0xF] = 1;
            }
            this->I += this->V[reg];
            break;
        case 0x0A:
            if (this->block)
            {
                // int ch = wgetch(stdscr);
                int ch = this->key;
                // std::cout << "character code: " << ch << std::endl;
                if (ch != 255)
                {
                    reg = (instr & 0x0F00) >> 8;
                    this->V[reg] = getchipkey(ch);
                    this->block = false;
                    this->PC += 2;
                }
            }
            else
            {
                this->PC -= 2;
                this->block = true;
            }
            break;
        case 0x29:
            reg = (instr & 0x0F00) >> 8;
            this->I = 0x50 + (this->V[reg] & 0xF) * 5;
            break;
        case 0x33:
            reg = (instr & 0x0F00) >> 8;
            this->ram[this->I] = this->V[reg] / 100;
            this->ram[this->I + 1] = (this->V[reg] % 100) / 10;
            this->ram[this->I + 2] = this->V[reg] % 10;
            break;
        case 0x55:
            reg = (instr & 0x0F00) >> 8;
            for (int k = 0; k <= reg; k++)
            {
                this->ram[this->I + k] = this->V[k];
            }
            break;
        case 0x65:
            reg = (instr & 0x0F00) >> 8;
            for (int k = 0; k <= reg; k++)
            {
                this->V[k] = this->ram[this->I + k];
            }
            break;

        default:
            throw std::runtime_error("Invalid F instruction");
            break;
        }
        break;
    default:
        throw std::runtime_error("Execute: invalid instruction");
    }
    
    return redraw;
};
uint16_t Chip8::fetch()
{
    auto val1 = (uint16_t)this->ram[this->PC];
    auto val2 = (uint16_t)this->ram[this->PC + 1];
    auto instr = 256 * (val1) + val2;
    this->PC += 2;
    return instr;
};