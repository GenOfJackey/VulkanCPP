cmake_minimum_required(VERSION 3.10.0)
project(VulkanCPP VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILE_FEATURES OFF)

add_subdirectory(external/SDL3)

find_package(Vulkan REQUIRED)
find_package(slang CONFIG HINTS "$ENV{VULKAN_SDK}/lib/cmake")
find_package(glm REQUIRED)

add_executable(VulkanCPP src/main.cpp)

target_link_libraries(VulkanCPP PRIVATE 
        Vulkan::Vulkan
        SDL3::SDL3
        glm::glm
)

target_include_directories(VulkanCPP PRIVATE
        "${CMAKE_SOURCE_DIR}/includes"
        "${CMAKE_SOURCE_DIR}/external/SDL3/include"
)

# set(RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist")
# set(LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist")
# set(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist")

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
