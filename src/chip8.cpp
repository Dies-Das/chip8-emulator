#include "chip8.h"
#include <stdexcept>
#include <cstdlib>
#include <string.h>
#include <time.h>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
Chip8::Chip8()
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
};

void Chip8::loop()
{
    int sleeptime = (int)(1000.0 / 60);
    sf::RenderWindow window(sf::VideoMode(256, 128), "Chip8");
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            bool redraw = 0;
            if (event.type == sf::Event::Closed)
                window.close();
            auto instr = this->fetch();
            redraw = this->execute(instr);
            if (redraw)
            {
                for (int x = 0; x < 64; x++)
                {
                    for (int y = 0; y < 32; y++)
                    {
                        sf::RectangleShape rect(sf::Vector2f(4, 4));
                        rect.setPosition(sf::Vector2f(x * 4, y * 4));
                        if (this->display.values[x][y])
                        {
                            rect.setFillColor(sf::Color(255, 255, 255, 255));
                        }
                        else
                        {
                            rect.setFillColor(sf::Color(0, 0, 0, 255));
                        }
                        window.draw(rect);
                    }
                }
            }
            usleep(sleeptime);
        }
    }
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
            uint8_t newinstr = this->stack.top();
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
        if (this->regvar[reg] == (instr & 0x00FF))
        {
            this->PC += 2;
        }
        break;
    case 4:
        reg = (instr & 0x0F00) >> 8;
        if (this->regvar[reg] != (instr & 0x00FF))
        {
            this->PC += 2;
        }
        break;
    case 5:
        reg1 = (instr & 0x0F00) >> 8;
        reg2 = (instr & 0x00F0) >> 4;
        if (this->regvar[reg1] == this->regvar[reg2])
        {
            this->PC += 2;
        }
        break;
    case 6:
        reg = (instr & 0x0F00) >> 8;
        this->regvar[reg] = (instr & 0x00FF);
        break;
    case 7:
        reg = (instr & 0x0F00) >> 8;
        this->regvar[reg] += (instr & 0x00FF);
        break;
    case 8:
        reg1 = (instr & 0x0F00) >> 8;
        reg2 = (instr & 0x00F0) >> 4;
        switch (instr & 0x000F)
        {
        case 0:
            this->regvar[reg1] = this->regvar[reg2];
            break;
        case 1:
            this->regvar[reg1] |= this->regvar[reg2];
            break;
        case 2:
            this->regvar[reg1] &= this->regvar[reg2];
            break;
        case 3:
            this->regvar[reg1] ^= this->regvar[reg2];
            break;
        case 4:
            if (255 - this->regvar[reg1] > this->regvar[reg2])
            {
                this->regvar[0xF] = 1;
            }
            else
            {
                this->regvar[0xF] = 0;
            }
            this->regvar[reg1] += this->regvar[reg2];
            break;
        case 5:
            if (this->regvar[reg1] >= this->regvar[reg2])
            {
                this->regvar[0xF] = 1;
            }
            else
            {
                this->regvar[0xF] = 0;
            }
            this->regvar[reg1] -= this->regvar[reg2];
            break;
        case 6:
            this->regvar[0xF] = this->regvar[reg1] & 0x0001;
            this->regvar[reg1] >>= 1;
            break;
        case 7:
            if (255 - this->regvar[reg2] >= this->regvar[reg1])
            {
                this->regvar[0xF] = 1;
            }
            else
            {
                this->regvar[0xF] = 0;
            }
            this->regvar[reg1] = this->regvar[reg2] - this->regvar[reg1];
            break;
        case 0xE:
            this->regvar[0xF] = this->regvar[reg1] >> 31;
            this->regvar[reg1] <<= 1;
            break;
        }
    case 9:
        reg1 = (instr & 0x0F00) >> 8;
        reg2 = (instr & 0x00F0) >> 4;
        if (this->regvar[reg1] != this->regvar[reg2])
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
        this->regvar[reg] = ((uint16_t)rand()) & (instr & 0x00FF);
        break;
    case 0xD:
        reg1 = (instr & 0x0F00) >> 8;
        reg2 = (instr & 0x00F0) >> 4;
        x = this->regvar[reg1] & 63;
        y = this->regvar[reg2] & 31;
        this->regvar[0xF] = 0;
        n = instr & 0xF;

        for (int k = 0; k < n; k++)
        {
            const auto sdata = this->ram[this->I + k];
            uint8_t whichbit = 128;
            for (int j = 0; j < 8; j++)
            {
                auto sbit = whichbit & sdata;
                sbit >>= (7 - j);
                if (sbit && this->display.values[x][y])
                {
                    this->display.values[x][y] = 0;
                    this->regvar[0xF] = 1;
                }
                else if (sbit && !this->display.values[x][y])
                {
                    this->display.values[x][y] = 1;
                }
                if (x == 63)
                {
                    break;
                }
                x++;
            }
            if (y == 31)
            {
                break;
            }
            y++;
        }
        redraw = 1;
        break;
    case 0xE:
        reg = (instr & 0x0F00) >> 8;
        if ((instr & 0x00FF) == 0x9E)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->regvar[reg])))
            {
                this->PC += 2;
            }
        }
        else if ((instr & 0x00FF) == 0xA1)
        {
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->regvar[reg])))
            {
                this->PC += 2;
            }
        }
        break;
    case 0xF:
        reg = (instr & 0x0F00) >> 8;
        switch (instr & 0x00FF)
        {
        case 0x07:
            this->regvar[reg] = this->delaytimer;
            break;
        case 0x15:
            this->delaytimer = this->regvar[reg];
            break;
        case 0x18:
            this->soundtimer = this->regvar[reg];
            break;
        case 0x1E:

            if (this->regvar[reg] + this->I > 0x0FFF)
            {
                this->regvar[0xF] = 1;
            }
            this->I += this->regvar[reg];
            break;
        case 0x0A:
            if(this->block){
                
            }
            else{
                this->PC -= 2;
                this->block=true;
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
    auto instr = 256 * ((uint16_t)this->ram[this->PC]) + (uint16_t)this->ram[this->PC];
    this->PC += 2;
    return instr;
};