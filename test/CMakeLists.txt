add_executable(chip8-tests
        chip8_test.cpp
)
target_include_directories(chip8-tests PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)
# Additional compiler flags or libraries, if needed
# target_compile_options(chip8-tests PUBLIC ...)
# target_link_libraries(chip8-tests ...)

# Include and link Google Test
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
find_package(Curses REQUIRED)
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

target_link_libraries(chip8-tests gtest_main chip8-lib sfml-graphics
sfml-audio ncurses)
