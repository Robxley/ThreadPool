# Lib - ThreadPool

# Create toolkit source files list
#FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_library(ThreadPool INTERFACE)

target_include_directories(ThreadPool 
                            INTERFACE
                                ${PROJECT_SOURCE_DIR})

if (WIN32)
    target_compile_options(ThreadPool INTERFACE /W3 /WX)
else()
    target_compile_options(ThreadPool INTERFACE -w)
endif()