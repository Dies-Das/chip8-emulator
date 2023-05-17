#include <array>
#include <stack>
#include <cstdint>
#include <algorithm>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
class Display
{
public:
    bool values[64][32] = {0};
};
class Chip8
{
public:
    std::array<uint8_t, 16> regvar = {};
    std::stack<uint8_t> stack = {};
    uint8_t soundtimer = 0;
    uint8_t delaytimer = 0;
    uint16_t I = 0;
    uint16_t PC = 512;
    bool block = 0;

    Display display;
    std::array<char, 4096> ram = {};
    Chip8(std::string path);
    void loop();
    uint16_t fetch();
    int execute(uint16_t instr, sf::Window &window);
};
