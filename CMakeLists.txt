cmake_minimum_required(VERSION 3.10)
project(MyOpenGLApp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Output binaries straight into build/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# --- Automatically find all C/C++ source and header files ---
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.hpp"
)

# --- Include paths ---
include_directories(${CMAKE_SOURCE_DIR}/include)

# --- Library search path ---
link_directories(${CMAKE_SOURCE_DIR}/lib)

# --- Executable Target ---
# Including HEADERS here makes them show up in IDE project trees (VS/CLion)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# --- Link libraries ---
target_link_libraries(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a
    opengl32
    gdi32
)



# --- Copy glfw3.dll next to the built exe ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/glfw3.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# --- Copy shaders next to the built exe ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)