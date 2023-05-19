cmake_minimum_required(VERSION 3.8.0)
project(nativefiledialog)

set(SOURCES nativefiledialog/src/nfd_common.c)

# Add specific implementations
if (WIN32)
    list(APPEND SOURCES nativefiledialog/src/nfd_win.cpp)
endif()

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC nativefiledialog/src/include)

# install(TARGETS nativefiledialog DESTINATION lib)