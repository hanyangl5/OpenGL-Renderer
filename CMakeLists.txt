cmake_minimum_required(VERSION 3.18)

project(Dredgen)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE HEADER_FILES "Dredgen/*.h")
file(GLOB_RECURSE SOURCE_FILES "Dredgen/*.cpp")

# source_group(TREE "${ENGINE_ROOT_DIR}/schema" FILES ${SCHEMA_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/assimp)
add_subdirectory(thirdparty/glad45)
add_subdirectory(thirdparty/nativefiledialog)
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})





target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/Dredgen
    thirdparty/glm/
    thirdparty/glfw/include
    thirdparty/assimp/include
    thirdparty/glad45/
    thirdparty/stb
    thirdparty/nativefiledialog/src
    thirdparty/nativefiledialog
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    glad45
    glfw
    nativefiledialog
    assimp
)

set_property(TARGET
    ${PROJECT_NAME}
    assimp
    glad45
    uninstall
    unit
    nativefiledialog
    zlibstatic
    UpdateAssimpLibsDebugSymbolsAndDLLs
    PROPERTY FOLDER "thirdparty")

set_property(TARGET
    ${PROJECT_NAME}
    Dredgen
    PROPERTY FOLDER "Dredgen")

find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs)
    doxygen_add_docs(
        doxygen
        Dredgen
        COMMENT "Generate html pages"
    )
endif(DOXYGEN_FOUND)
