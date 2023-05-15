#include <array>
#include <stack>
#include <cstdint>
#include <algorithm>

class Display
{
public:
    std::array<bool, 64 * 32> values = {};
};
class Chip8
{
public:
    std::array<uint8_t, 16> regvar = {};
    std::stack<uint8_t> stack = {};
    uint8_t soundtimer = 0;
    uint8_t delaytimer = 0;
    uint16_t I = 0;
    uint16_t PC = 0;


    Display display;
    std::array<uint8_t, 4096> ram = {};
    Chip8();
    void loop();
    uint16_t fetch();
    int execute(uint16_t instr);
};
