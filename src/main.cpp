#include <iostream>
#include "chip8.h"
#include <SFML/Window.hpp>
int main(){
    Chip8 chip("/home/tilman/coding/chip8-emulator/src/IBM Logo.ch8");
    chip.loop();

return 0;
};