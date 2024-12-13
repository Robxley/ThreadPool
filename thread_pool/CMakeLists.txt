GET_FILENAME_COMPONENT(LAST_FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
project(${LAST_FOLDER_NAME})
set(${PROJECT_NAME}_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "📚 Library: ${PROJECT_NAME} - ${PROJECT_NAME}_DIR: ${${PROJECT_NAME}_DIR}")

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_library(thread_pool STATIC ${LOCAL_FILE_SRC})

target_include_directories(thread_pool 
                            PUBLIC
                                ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    target_compile_options(thread_pool PRIVATE /W3 /WX)
else()
    target_compile_options(thread_pool PRIVATE -w)
endif()