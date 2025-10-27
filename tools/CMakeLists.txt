cmake_minimum_required(VERSION 3.20)

project(sunshine_tools)

include_directories("${CMAKE_SOURCE_DIR}")

add_executable(dxgi-info dxgi.cpp)
set_target_properties(dxgi-info PROPERTIES CXX_STANDARD 23)
target_link_libraries(dxgi-info
        ${CMAKE_THREAD_LIBS_INIT}
        dxgi
        ${PLATFORM_LIBRARIES})
target_compile_options(dxgi-info PRIVATE ${SUNSHINE_COMPILE_OPTIONS})

add_executable(audio-info audio.cpp)
set_target_properties(audio-info PROPERTIES CXX_STANDARD 23)
target_link_libraries(audio-info
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ksuser
        ${PLATFORM_LIBRARIES})
target_compile_options(audio-info PRIVATE ${SUNSHINE_COMPILE_OPTIONS})

add_executable(sunshinesvc sunshinesvc.cpp)
set_target_properties(sunshinesvc PROPERTIES CXX_STANDARD 23)
target_link_libraries(sunshinesvc
        ${CMAKE_THREAD_LIBS_INIT}
        wtsapi32
        ${PLATFORM_LIBRARIES})
target_compile_options(sunshinesvc PRIVATE ${SUNSHINE_COMPILE_OPTIONS})

add_executable(virtual-keyboard virtual-keyboard.cpp)
set_target_properties(virtual-keyboard PROPERTIES CXX_STANDARD 23)
target_compile_definitions(virtual-keyboard PRIVATE UNICODE _UNICODE)
target_link_libraries(virtual-keyboard
        ${CMAKE_THREAD_LIBS_INIT}
        shell32
        advapi32
        ole32
        oleaut32
        uuid
        ${PLATFORM_LIBRARIES})
target_compile_options(virtual-keyboard PRIVATE ${SUNSHINE_COMPILE_OPTIONS})
# 为 MinGW 设置正确的入口点和子系统
if(MINGW)
    target_link_options(virtual-keyboard PRIVATE -municode -static -static-libgcc -static-libstdc++)
endif()