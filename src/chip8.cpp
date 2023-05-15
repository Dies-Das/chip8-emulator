#include "chip8.h"
#include <stdexcept>
#include <cstdlib>

#include <time.h>
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

    while (true)
    {

        auto instr = this->fetch();
        this->execute(instr);
    }
};

int Chip8::execute(uint16_t instr){

    const uint16_t mask = 0xF000;
    auto fnib = mask & instr;
    fnib = fnib>>12;
    auto reg = 0;
    uint16_t reg1 = 0;
    uint16_t reg2 = 0;
    
    switch (fnib){
        case 0:
            if (instr==0x00E0){
                this->display.values={0};
            }
            else if (instr==0x00EE){
                uint8_t newinstr = this->stack.top();
                this->stack.pop();
                this->PC = newinstr;
            }
            break;
        case 1:
            this->PC = (instr&0x0FFF);
            break;
        case 2:
            this->stack.push(this->PC);
            this->PC=(instr&0x0FFF);
            break;
        case 3:
             reg = (instr & 0x0F00)>>8;
            if(this->regvar[reg]==(instr&0x00FF)){
                this->PC += 2;
            }
            break;
        case 4:
            reg = (instr & 0x0F00)>>8;
            if(this->regvar[reg]!=(instr&0x00FF)){
                this->PC += 2;
            }
            break;
        case 5:
            reg1 =(instr & 0x0F00)>>8;
            reg2 =(instr & 0x00F0)>>4;
            if(this->regvar[reg1]==this->regvar[reg2]){
                this->PC+=2;
            }
            break;
        case 6:
            reg = (instr & 0x0F00)>>8;
            this->regvar[reg]=(instr&0x00FF);
            break;
        case 7:
            reg = (instr & 0x0F00)>>8;
            this->regvar[reg]+=(instr&0x00FF);
            break;
        case 8:
            reg1 =(instr & 0x0F00)>>8;
            reg2 =(instr & 0x00F0)>>4;
            switch(instr&0x000F){
                case 0:
                    this->regvar[reg1]=this->regvar[reg2];
                    break;
                case 1:
                    this->regvar[reg1]|= this->regvar[reg2];
                    break;
                case 2:
                    this->regvar[reg1]&= this->regvar[reg2];
                    break;
                case 3:
                    this->regvar[reg1]^= this->regvar[reg2];
                    break;
                case 4:
                    if (255-this->regvar[reg1]>this->regvar[reg2]){
                        this->regvar[0xF]=1;
                    }
                    else{
                        this->regvar[0xF]=0;
                    }
                    this->regvar[reg1]+= this->regvar[reg2];
                    break;
                case 5:
                    if (this->regvar[reg1]>=this->regvar[reg2]){
                        this->regvar[0xF]=1;
                    }
                    else{
                        this->regvar[0xF]=0;
                    }
                    this->regvar[reg1]-= this->regvar[reg2];
                    break;
                case 6:
                    this->regvar[0xF]=this->regvar[reg1]&0x0001;
                    this->regvar[reg1]>>=1;
                    break;
                case 7:
                    if (255-this->regvar[reg2]>=this->regvar[reg1]){
                        this->regvar[0xF]=1;
                    }
                    else{
                        this->regvar[0xF]=0;
                    }
                    this->regvar[reg1]= this->regvar[reg2]-this->regvar[reg1];
                    break; 
                case 0xE:
                    this->regvar[0xF]=this->regvar[reg1]>>31;
                    this->regvar[reg1]<<=1;
                    break;
            }
        case 9:
            reg1 =(instr & 0x0F00)>>8;
            reg2 =(instr & 0x00F0)>>4;
            if(this->regvar[reg1]!=this->regvar[reg2]){
                this->PC+=2;
            }
            break;
        case 0xA:
            this->I = instr&0x0FFF;
            break;
        case 0xB:
            this->PC = instr&0x0FFF;    
            break;
        case 0xC:
            reg = (instr & 0x0F00)>>8;
            this->regvar[reg] = ((uint16_t)rand()) & (instr&0x00FF);
            break;
        case 0xD:
            reg1 =(instr & 0x0F00)>>8;
            reg2 =(instr & 0x00F0)>>4;
            auto x = this->regvar[reg1]&63;
            auto y = this->regvar[reg2]&31;
            this->regvar[0xF]=0;
            auto n = instr&0xF;
            for(int k=0; k<n;k++){
                const auto sdata = this->ram[this->I+k];
                

            }
            break;
        default:
            throw std::runtime_error("Execute: invalid instruction");
    }
    return 1;
};
uint16_t Chip8::fetch(){
    auto instr = 256*((uint16_t)this->ram[this->PC])+(uint16_t)this->ram[this->PC];
    this->PC += 2;
    return instr;


};