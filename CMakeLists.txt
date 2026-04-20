cmake_minimum_required(VERSION 3.20)

project(visual-score-engine VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to SFML (Windows)
set(SFML_DIR "C:/Users/super/SFML-2.5.1/lib/cmake/SFML")

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# GLAD
add_library(glad STATIC src/glad.c)
set_source_files_properties(src/glad.c PROPERTIES LANGUAGE C)
target_include_directories(glad PUBLIC include)

# Source files
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/pattern_1.cpp
    src/pattern_1_blur.cpp
    src/pattern_2.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Link SFML and GLAD
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    glad
    opengl32
)