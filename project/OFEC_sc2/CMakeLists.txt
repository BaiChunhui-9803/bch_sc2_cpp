cmake_minimum_required(VERSION 3.10)
project(OFEC_sc2)
find_package(Threads)
set(CMAKE_CXX_FLAGS -pthread)
set(OFEC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
IF (WIN32)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs")
ENDIF (WIN32)
add_definitions(-DOFEC_DIR="${OFEC_DIR}")
#include(SrcFiles.cmake)
include(SrcFiles_test.cmake)
target_compile_features(OFEC_sc2 PRIVATE cxx_std_17)

#if (LIBTORCH_REQUIRED)
#    if(CMAKE_BUILD_TYPE MATCHES "Debug")
#        set(Torch_DIR "$ENV{LIBTORCH_DIR_DEBUG}/share/cmake/Torch")
#    elseif (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
#        set(Torch_DIR "$ENV{LIBTORCH_DIR_RELEASE}/share/cmake/Torch")
#    else()
#        message(STATUS "build type unknown")
#    endif()
#    find_package(Torch REQUIRED)
#    target_link_libraries(${CMAKE_PROJECT_NAME} ${TORCH_LIBRARIES})
#endif (LIBTORCH_REQUIRED)

# C/C++ 常规 附加包含目录
include_directories(../../include)
include_directories(../../contrib/protobuf/src)
include_directories(../../contrib/civetweb/include)
include_directories(../../build/contrib/ipv6-parse)
include_directories(../../build/generated)
include_directories(../../contrib/SDL-mirror/include)

# 链接器 输入 附加依赖项
target_link_libraries(${PROJECT_NAME} 
../../bin/sc2apid
../../bin/sc2libd
../../bin/sc2utilsd
../../bin/sc2rendererd
../../bin/sc2protocold
../../bin/libprotobufd
../../bin/civetweb
../../contrib/ipv6-parse/bin/ipv6-parse
../../bin/SDL2
winmm.lib
imm32.lib
version.lib
dinput8.lib
)