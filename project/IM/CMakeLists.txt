cmake_minimum_required(VERSION 3.1)

# 1.语言标准与预处理器
set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

# 2.project name，指定项目的名称，一般和项目的文件夹名称对应
project(IM)

# 3.head file path，头文件目录
INCLUDE_DIRECTORIES(include)
file(GLOB_RECURSE pipe_header_files  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
source_group("Header Files" FILES ${pipe_header_files})

# 4.source directory，源文件目录
AUX_SOURCE_DIRECTORY(src DIR_SRCS)

# 5.set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
SET(TEST_MATH ${DIR_SRCS})

# 6.add executable file，添加要编译的可执行文件
ADD_EXECUTABLE(${PROJECT_NAME} ${TEST_MATH}  ${pipe_header_files})

# C/C++ 常规 附加包含目录
include_directories(../../include)
include_directories(../../contrib/protobuf/src)
include_directories(../../contrib/civetweb/include)
include_directories(../../build/contrib/ipv6-parse)
include_directories(../../build/generated)
include_directories(../../contrib/SDL-mirror/include)
include_directories(D:/boost_1_75_0/stage/lib)
include_directories(D:/boost_1_75_0)
include_directories(D:/Boost/lib)
include_directories(D:/Boost/include)

# C/C++ 常规 附加包含目录
link_directories(D:/boost_1_75_0)
link_directories(D:/Boost/lib)
link_directories(D:/Boost/include)

# VC++目录 库目录
link_libraries(D:/Boost/lib)

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