cmake_minimum_required(VERSION 3.20)
project(visual-score-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set path to your SFML 2.5.1 folder
set(SFML_DIR "C:/Users/super/SFML-2.5.1/lib/cmake/SFML")

# Add source files
file(GLOB SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# Find SFML 2.5 components
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Link SFML
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
