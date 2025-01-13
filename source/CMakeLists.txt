# ###########################################################################################
# Define the minimum required version of cmake
cmake_minimum_required(VERSION 3.16.3)

# Define the project
project(
    executable
    VERSION 1.0
    DESCRIPTION "Graphics engine using vulcan API"
    HOMEPAGE_URL "https://github.com/GomaaMohamed"
    LANGUAGES CXX)

# Define standard and flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g")

# Find packages
# Find GLFW
find_package(glfw3 QUIET)
if (NOT glfw3_FOUND)
    message(STATUS "GLFW not found. Installing GLFW...")
    execute_process(COMMAND sudo apt-get install -y libglfw3-dev)
    find_package(glfw3 REQUIRED)
endif()
# Find GLM
find_package(glm QUIET)
if (NOT glm_FOUND)
    message(STATUS "GLM not found. Installing GLM...")
    execute_process(COMMAND sudo apt-get install -y libglm-dev)
    find_package(glm REQUIRED)
endif()
# Find Vulkan
find_package(Vulkan QUIET)
if (NOT Vulkan_FOUND)
    message(STATUS "Vulkan not found. Installing Vulkan...")
    execute_process(COMMAND sudo apt-get install -y vulkan-sdk)
    find_package(Vulkan REQUIRED)
endif()

# Add the sources of unit testing
set(SRCS
    src/main.cpp
    src/core/Application.cpp
    src/core/PhysicalDevices.cpp
    src/core/VkCore.cpp
)

# Define an executable target
add_executable(${PROJECT_NAME})
target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
target_link_options(${PROJECT_NAME} PRIVATE -g -O0)

# Add the include directries for a target
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC include
    PRIVATE src
    PRIVATE src/core
    PRIVATE include/core)

# Add the sources to the target
target_sources(
    ${PROJECT_NAME}
    PRIVATE ${SRCS})

# Link with gl lib
target_link_libraries(${PROJECT_NAME} glfw Vulkan::Vulkan glm)

# Add shader compiling
set(SHADERS_SRC
    shaders/shader.vert
    shaders/shader.frag)

add_custom_command(TARGET executable POST_BUILD
    DEPENDS ${SHADERS_SRC}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND echo "------- Built successfully ---------"
    COMMAND rm -rf spir_v && mkdir spir_v
    COMMAND glslc ../source/shaders/shader.vert -o spir_v/vert.spv && glslc ../source/shaders/shader.frag -o spir_v/frag.spv)

# Add definitions
target_compile_definitions(executable PRIVATE SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")