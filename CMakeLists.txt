cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

# Set project name and version
project(threadpool VERSION "0.1.0" LANGUAGES CXX)

# Set default runtime directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Add CMake modules folder path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Set C++ standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


# Display build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


# __________________________________________
# __ Libraries


# External libraries
find_package(Threads REQUIRED)


# __________________________________________
# __ Subdirectories

add_subdirectory(ThreadPool)
add_subdirectory(Tests)