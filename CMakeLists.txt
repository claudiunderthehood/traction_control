cmake_minimum_required(VERSION 3.10)
project(traction_control)

set(CMAKE_CXX_STANDARD 17)

# Set SDL2 paths (relative to the project directory)
set(SDL2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/SDL2/include)
set(SDL2_LIBRARY ${CMAKE_SOURCE_DIR}/SDL2/lib/libSDL2.a)
set(SDL2_MAIN_LIBRARY ${CMAKE_SOURCE_DIR}/SDL2/lib/libSDL2main.a)

# Add include directories
include_directories(${SDL2_INCLUDE_DIR}) # Add SDL2 include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source files
set(SOURCES
    src/Vehicle.cpp
    src/TractionControl.cpp
    src/Simulation.cpp
    src/Visualizer.cpp
    src/main.cpp
)

# Add executable
add_executable(traction_control ${SOURCES})

# Link SDL2 libraries and Windows system libraries
target_link_libraries(traction_control
    mingw32
    ${SDL2_MAIN_LIBRARY}
    ${SDL2_LIBRARY}
    -mwindows
    -limm32
    -lsetupapi
    -lversion
    -lwinmm
    -ldxguid
    -lole32
    -lgdi32
)

# Copy SDL2.dll to output directory
add_custom_command(TARGET traction_control POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/SDL2/SDL2.dll $<TARGET_FILE_DIR:traction_control>
)

message("SDL2_INCLUDE_DIR: ${SDL2_INCLUDE_DIR}")