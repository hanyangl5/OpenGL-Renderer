

function(configure_compile_options proj_name)
set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    if(MSVC)
        message("using MSVC under windows")

        target_compile_options(${proj_name} PRIVATE /std:c++17)
        target_compile_options(${proj_name} PRIVATE /MP /permissive /w14640 /W4 /WX /external:anglebrackets /external:W0 /GR-)

    else()
        message("using GCC/Clang under windows")
        target_compile_options(${proj_name} PRIVATE -std=c++17)
        target_compile_options(${proj_name} PRIVATE -Wall -Wextra -Werror -Wshadow -Wnon-virtual-dtor -pedantic Weverything -fno-rtti)
    endif()
elseif(LINUX)
    message("using GCC/Clang under LINUX")
    target_compile_options(${proj_name} PRIVATE -std=c++17)
    target_compile_options(${proj_name} PRIVATE -Wall -Wextra -Werror -Wshadow -Wnon-virtual-dtor -pedantic Weverything -fno-rtti)
endif()

# disable exceptions
# MSVC string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # disable exception
# GCC/Clang  -fno-exceptions
endfunction()

