
cmake_minimum_required(VERSION 3.29)
project(SGame C)

# Set the C standard to C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Include directories for SDL2_image and SDL2_ttf
include_directories(/opt/homebrew/Cellar/sdl2_image/2.8.2_2/include/SDL2)
include_directories(/opt/homebrew/Cellar/sdl2_ttf/2.22.0/include/SDL2)

# Link directories for SDL2_image and SDL2_ttf
link_directories(/opt/homebrew/Cellar/sdl2_image/2.8.2_2/lib)
link_directories(/opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib)

# Find and link against libcurl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

# Add the source files for the project
add_executable(SGame
        main.c
)

# Link SDL2, SDL2_image, SDL2_ttf, and CURL libraries to the project
target_link_libraries(SGame ${SDL2_LIBRARIES} SDL2_image SDL2_ttf ${CURL_LIBRARIES})