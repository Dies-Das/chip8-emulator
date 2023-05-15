add_library(chip8-lib
        chip8.cpp
)

target_include_directories(chip8-lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(chip8-emulator
        main.cpp
)

target_link_libraries(chip8-emulator
        chip8-lib
)
