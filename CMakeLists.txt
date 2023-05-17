cmake_minimum_required(VERSION 3.15)
project(chip8-emulator)

set(CMAKE_CXX_STANDARD 17)
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

#add_subdirectory(src)
add_subdirectory(test)
add_library(chip8-lib
        src/chip8.cpp
)

target_include_directories(chip8-lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(chip8-emulator
        src/main.cpp
)

target_link_libraries(chip8-emulator
        PRIVATE
        sfml-graphics
        sfml-audio
        chip8-lib
)