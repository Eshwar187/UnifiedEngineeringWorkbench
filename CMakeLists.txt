cmake_minimum_required(VERSION 3.16)
project(UnifiedEngineeringWorkbench LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(UnifiedEngineeringWorkbench WIN32
    src/main.cpp
    src/thermal/thermal_solver.cpp
)
target_include_directories(UnifiedEngineeringWorkbench PRIVATE include)

if(MSVC)
    target_compile_options(UnifiedEngineeringWorkbench PRIVATE /W4)
else()
    target_compile_options(UnifiedEngineeringWorkbench PRIVATE -Wall -Wextra -O2)
    target_link_options(UnifiedEngineeringWorkbench PRIVATE -static -static-libgcc -static-libstdc++)
endif()
