# Lib - thread_pool

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_library(thread_pool STATIC ${LOCAL_FILE_SRC})

target_include_directories(thread_pool 
                            PRIVATE
                                ${PROJECT_SOURCE_DIR})

if (WIN32)
    target_compile_options(thread_pool PRIVATE /W3 /WX)
else()
    target_compile_options(thread_pool PRIVATE -w)
endif()