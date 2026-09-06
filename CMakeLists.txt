cmake_minimum_required(VERSION 3.14)
project(MetalGearPC C)

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Download and compile SDL2 automatically
include(FetchContent)
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.0
)
FetchContent_MakeAvailable(SDL2)

# Our source files
set(SOURCES
    src/main.c
    src/memory.c
    src/vdp.c
    src/game.c
)

# Create the executable
add_executable(metal_gear_pc ${SOURCES})

# Link SDL2 to our executable
# sdl2main is needed on Windows to wrap the WinMain entry point
target_link_libraries(metal_gear_pc PRIVATE SDL2::SDL2main SDL2::SDL2-static)

# Ensure the executable is placed in the root build directory
set_target_properties(metal_gear_pc PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")