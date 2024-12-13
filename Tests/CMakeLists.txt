# App - Tests threadpool

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(test_thread_pool ${LOCAL_FILE_SRC})

target_include_directories(test_thread_pool 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}/thread_pool)

target_link_libraries(test_thread_pool 
                        PUBLIC 
                            thread_pool)

if (WIN32)
    target_compile_options(test_thread_pool PRIVATE /W3 /WX)
else()
    target_compile_options(test_thread_pool PRIVATE -w)
endif()