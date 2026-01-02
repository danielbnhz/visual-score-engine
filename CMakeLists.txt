cmake_minimum_required(VERSION 3.20)

project(visual-score-engine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to SFML (Windows)
set(SFML_DIR "C:/Users/super/SFML-2.5.1/lib/cmake/SFML")

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Source files
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SFML
target_link_libraries(${PROJECT_NAME}
    PRIVATE sfml-graphics sfml-window sfml-system
)
