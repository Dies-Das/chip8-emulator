#include <array>
#include <stack>
#include <cstdint>
#include <algorithm>
#include <string>
#include <ncurses.h>
class Display
{
public:
    bool values[32][64] = {0};
};
class Chip8
{
public:
    std::array<uint8_t, 16> V = {};
    std::stack<uint16_t> stack = {};
    uint8_t soundtimer = 0;
    uint8_t delaytimer = 0;
    uint16_t I = 0;
    uint16_t PC = 512;
    uint8_t key = 255;
    bool block = 0;

    Display display;
    std::array<unsigned char, 4096> ram = {};
    Chip8(std::string path);
    void loop();
    uint16_t fetch();
    int execute(uint16_t instr);
};
