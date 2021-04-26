# App - Tests threadpool

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(Tests ${LOCAL_FILE_SRC})

target_include_directories(Tests 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}/ThreadPool)

target_link_libraries(Tests 
                        PUBLIC 
                            ThreadPool)

if (WIN32)
    target_compile_options(Tests PRIVATE /W3 /WX)
else()
    target_compile_options(Tests PRIVATE -w)
endif()