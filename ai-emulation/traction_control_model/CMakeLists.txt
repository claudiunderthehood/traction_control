cmake_minimum_required(VERSION 3.10)
project(traction_control)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_MAIN "Build the main executable" ON)

if(WIN32)
    message("Configuring for Windows (MSVC)")

    set(SDL2_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/SDL2/include")
    set(SDL2_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/SDL2/lib/x64")

    include_directories(${SDL2_INCLUDE_DIR})
    link_directories(${SDL2_LIBRARY_DIR})

    set(SDL2_LIBRARIES
        SDL2
        SDL2main
    )
else()
    message("Configuring for Unix-based OS")

    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

find_package(Torch REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(COMMON_SOURCES
    src/Vehicle.cpp
    src/TractionControl.cpp
    src/Simulation.cpp
    src/Visualizer.cpp
)

if(BUILD_MAIN)
    message("Building main executable")

    set(MAIN_SOURCES
        ${COMMON_SOURCES}
        src/main.cpp
    )

    add_executable(traction_control ${MAIN_SOURCES})

    if(WIN32)
        target_link_libraries(traction_control
            ${SDL2_LIBRARIES}
            "${TORCH_LIBRARIES}"
            imm32
            setupapi
            version
            winmm
            dxguid
            ole32
            gdi32
        )
        add_custom_command(TARGET traction_control POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${SDL2_LIBRARY_DIR}/SDL2.dll"
                $<TARGET_FILE_DIR:traction_control>
        )
        file(GLOB TORCH_DLLS
            "${CMAKE_PREFIX_PATH}/lib/*.dll"
        )
        foreach(DLL ${TORCH_DLLS})
            add_custom_command(TARGET traction_control POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DLL}"
                    $<TARGET_FILE_DIR:traction_control>
            )
        endforeach()
        add_custom_command(TARGET traction_control POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/mlp_model_traced.pt"
                $<TARGET_FILE_DIR:traction_control>
        )

    else()
        target_link_libraries(traction_control
            ${SDL2_LIBRARIES}
            "${TORCH_LIBRARIES}"
            pthread
        )
    endif()

else()
    message("Building data generator executable")

    set(DATA_SOURCES
        ${COMMON_SOURCES}
        src/data_generator.cpp
    )
    add_executable(data_generator ${DATA_SOURCES})
    if(WIN32)
        target_link_libraries(data_generator
            ${SDL2_LIBRARIES}
            "${TORCH_LIBRARIES}"
            imm32
            setupapi
            version
            winmm
            dxguid
            ole32
            gdi32
        )

        add_custom_command(TARGET data_generator POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${SDL2_LIBRARY_DIR}/SDL2.dll"
                $<TARGET_FILE_DIR:data_generator>
        )

        file(GLOB TORCH_DLLS
            "${CMAKE_PREFIX_PATH}/libtorch/lib/*.dll"
        )
        foreach(DLL ${TORCH_DLLS})
            add_custom_command(TARGET data_generator POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DLL}"
                    $<TARGET_FILE_DIR:data_generator>
            )
        endforeach()

        add_custom_command(TARGET data_generator POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/mlp_model_traced.pt"
                $<TARGET_FILE_DIR:data_generator>
        )

    else()
        target_link_libraries(data_generator
            ${SDL2_LIBRARIES}
            "${TORCH_LIBRARIES}"
            pthread
        )
    endif()
endif()

if(WIN32)
    message("SDL2_INCLUDE_DIR: ${SDL2_INCLUDE_DIR}")
    message("SDL2_LIBRARY_DIR: ${SDL2_LIBRARY_DIR}")
endif()

message("TORCH_LIBRARIES: ${TORCH_LIBRARIES}")