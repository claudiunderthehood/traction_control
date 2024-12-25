cmake_minimum_required(VERSION 3.10)
project(traction_control)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    message("Configuring for Windows")

    set(SDL2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/SDL2/include)
    set(SDL2_LIBRARY ${CMAKE_SOURCE_DIR}/SDL2/lib/libSDL2.a)
    set(SDL2_MAIN_LIBRARY ${CMAKE_SOURCE_DIR}/SDL2/lib/libSDL2main.a)
    
    include_directories(${SDL2_INCLUDE_DIR})
else()
    message("Configuring for Unix-based OS")
    
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

option(BUILD_MAIN "Build the main executable" ON)

if(BUILD_MAIN)
    message("Building main executable")
    set(SOURCES
        src/Vehicle.cpp
        src/TractionControl.cpp
        src/Simulation.cpp
        src/Visualizer.cpp
        src/main.cpp
    )
    add_executable(traction_control ${SOURCES})
    
    if(WIN32)
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

        add_custom_command(TARGET traction_control POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/SDL2/SDL2.dll $<TARGET_FILE_DIR:traction_control>
        )
    else()
        target_link_libraries(traction_control ${SDL2_LIBRARIES})
    endif()
else()
    message("Building data generator executable")
    set(SOURCES
        src/Vehicle.cpp
        src/TractionControl.cpp
        src/Simulation.cpp
        src/Visualizer.cpp
        src/data_generator.cpp
    )
    add_executable(data_generator ${SOURCES})

    if(WIN32)
        target_link_libraries(data_generator
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

        add_custom_command(TARGET data_generator POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/SDL2/SDL2.dll $<TARGET_FILE_DIR:data_generator>
        )
    else()
        target_link_libraries(data_generator ${SDL2_LIBRARIES})
    endif()
endif()

if(WIN32)
    message("SDL2_INCLUDE_DIR: ${SDL2_INCLUDE_DIR}")
else()
    message("SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
endif()