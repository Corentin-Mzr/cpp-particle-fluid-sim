cmake_minimum_required(VERSION 3.14)
project(SFML_TEST VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create a list of compiler warning flags for GCC/Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Add MSVC specific warnings (optional)
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /WX)  # Equivalent to high warning level and treat warnings as errors
endif()

# Specify output directories based on the build type
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

# Glob for source files
file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Detect and enable OpenMP
find_package(OpenMP REQUIRED)
if (OpenMP_CXX_FOUND)
    message(STATUS "Found OpenMP. Version: ${OpenMP_CXX_VERSION}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Create exe
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} 
PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML/lib/libsfml-graphics.a
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML/lib/libsfml-window.a
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML/lib/libsfml-system.a
PRIVATE
    opengl32
    ${OpenMP_CXX_LIBRARIES}
)


# Packaging configuration
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

