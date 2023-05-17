#include "chip8.h"
#include "gtest/gtest.h"

// TEST(Chip8Test, Constructor) {
//   Chip8 chip8{};
//   // Test that the font is loaded correctly
//   std::array<uint8_t, 80> font = {
//       0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
//       0x20, 0x60, 0x20, 0x20, 0x70, // 1
//       0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
//       0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
//       0x90, 0x90, 0xF0, 0x10, 0x10, // 4
//       0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
//       0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
//       0xF0, 0x10, 0x20, 0x40, 0x40, // 7
//       0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
//       0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
//       0xF0, 0x90, 0xF0, 0x90, 0x90, // A
//       0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
//       0xF0, 0x80, 0x80, 0x80, 0xF0, // C
//       0xE0, 0x90, 0x90, 0x90, 0xE0, // D
//       0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
//       0xF0, 0x80, 0xF0, 0x80, 0x80  // F}
//   };
//   EXPECT_EQ(std::equal(chip8.ram.begin()+0x50, 
//                         chip8.ram.begin()+0x50+font.size(), 
//                         font.begin()), true);
// }



TEST(Chip8Test, Fetch) {
  // Test that fetch returns the correct instruction and increments PC
  Chip8 chip8("/home/tilman/coding/chip8-emulator/src/IBM Logo.ch8");
  chip8.ram[0x200] = 0x1;
  chip8.ram[0x201] = 0x1;
  chip8.PC = 0x200;
  uint16_t instr = chip8.fetch();
  EXPECT_EQ(instr, 0x0101);
  EXPECT_EQ(chip8.PC, 0x202);
}

TEST(Chip8Test, Draw) {
  // Test that fetch returns the correct instruction and increments PC
  Chip8 chip8("/home/tilman/coding/chip8-emulator/src/IBM Logo.ch8");
  chip8.I = 0x50;
    sf::Window window;
    chip8.regvar[0]=10;
    chip8.execute(0xD00A, window);
    for(int x=0; x<32; x++){
        for(int y=0; y<64; y++){
            std::cout << (int)chip8.display.values[x][y] << " ";
        }
        std::cout << std::endl;
    }
    
  //chip8.PC = 0x200;
  //uint16_t instr = chip8.fetch();
}

TEST(Chip8Test, ClearScreen) {
    Chip8 chip8("/home/tilman/coding/chip8-emulator/src/IBM Logo.ch8");
    chip8.display.values[0][0] = 1;
    sf::Window window;
    chip8.execute(0x00E0, window);
    
    for(int x=0; x<32; x++){
        for(int y=0; y<64; y++){
            ASSERT_EQ(chip8.display.values[x][y],0);
        }
    }

}

// TEST(Chip8Test, ReturnFromSubroutine) {
//     Chip8 chip8;
//     chip8.execute(0x22FF); // Call a function with address 0x2FF
//     ASSERT_EQ(chip8.PC, 0x2FF);
//     chip8.execute(0x00EE); // Return from function
//     ASSERT_EQ(chip8.PC, 0x000);
// }

// TEST(Chip8Test, JumpToAddress) {
//     Chip8 chip8;
//     chip8.execute(0x1290); // Jump to address 0x290
//     ASSERT_EQ(chip8.PC, 0x290);
// }

// TEST(Chip8Test, CallSubroutine) {
//     Chip8 chip8;
//     chip8.execute(0x21EE); // Call a function with address 0x1EE
//     ASSERT_EQ(chip8.stack.top(), 0x000); // Check that the return address is pushed onto the stack
//     ASSERT_EQ(chip8.PC, 0x1EE); // Check that the program counter is set to the function address
// }

// TEST(Chip8Test, SkipNextInstructionIfEqual) {
//     Chip8 chip8;
//     chip8.regvar[3] = 0x13;
//     chip8.execute(0x3313); // Skip next instruction if V3 == 0x13
//     ASSERT_EQ(chip8.PC, 0x002);
// }

// TEST(Chip8Test, SkipNextInstructionIfNotEqual) {
//     Chip8 chip8;
//     chip8.regvar[3] = 0x13;
//     chip8.execute(0x4399); // Skip next instruction if V3 != 0x99
//     ASSERT_EQ(chip8.PC, 0x002);
// }

// TEST(Chip8Test, SkipNextInstructionIfRegistersEqual) {
//     Chip8 chip8;
//     chip8.regvar[3] = 0x13;
//     chip8.regvar[4] = 0x13;
//     chip8.execute(0x5340); // Skip next instruction if V3 == V4
//     ASSERT_EQ(chip8.PC, 0x002);
// }

TEST(Chip8Test, LoadValueIntoRegister) {
    Chip8 chip8("/home/tilman/coding/chip8-emulator/src/IBM Logo.ch8");
    sf::Window window;
    chip8.execute(0x60AB, window); // Load the value 0xAB into V0
    ASSERT_EQ(chip8.regvar[0], 0xAB);
}
TEST(Chip8Test, LoadIndexValue) {
    Chip8 chip8("/home/tilman/coding/chip8-emulator/src/IBM Logo.ch8");
    sf::Window window;
    chip8.execute(0xAFFF, window); // Load the value 0xAB into V0
    ASSERT_EQ(chip8.I, 0xFFF);
}
TEST(Chip8Test, AddValueToRegister) {
    Chip8 chip8("/home/tilman/coding/chip8-emulator/src/IBM Logo.ch8");
    sf::Window window;
    chip8.regvar[1] = 0x7F;
    chip8.execute(0x7103, window); // Add the value 0x03 to V1
    ASSERT_EQ(chip8.regvar[1], 0x82);
}

// TEST(Chip8Test, AssignRegisterValue) {
//     Chip8 chip8;
//     chip8.regvar[1] = 0x77;
//     chip8.execute(0x8210); // Assign the value of V2 to V1
//     ASSERT_EQ(chip8.regvar[2], 0x77); // Check that V2 was assigned the correct value
// }

// TEST(Chip8Test, OrRegisterValues) {
//     Chip8 chip8;
//     chip8.regvar[1] = 0b1100;
//     chip8.regvar[2] = 0b0011;
//     chip8.execute(0x8121); // Bitwise OR V1 and V2, store result in V1
//     ASSERT_EQ(chip8.regvar[1], 0b1111);
// }