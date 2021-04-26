# App - Tests threadpool

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(tests ${LOCAL_FILE_SRC})

target_include_directories(tests 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}/thread_pool)

target_link_libraries(tests 
                        PUBLIC 
                            thread_pool)

if (WIN32)
    target_compile_options(tests PRIVATE /W3 /WX)
else()
    target_compile_options(tests PRIVATE -w)
endif()