#include <iostream>
#include "chip8.h"
#include <SFML/Window.hpp>
int main(int argc, char *argv[]){
    std::string path(argv[1]);
    //std::string path("/home/tilman/coding/chip8-emulator/src/TETRIS");
    
    Chip8 chip(path);
    chip.loop();

return 0;
};