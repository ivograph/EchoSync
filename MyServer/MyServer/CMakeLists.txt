# CMakeList.txt : CMake project for MyServer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("MyServer")

# Add source to this project's executable.
#add_subdirectory ("./")
add_executable (${PROJECT_NAME} "MyServer.cpp" "MyServer.h")
add_definitions(-D_WIN32_WINNT=0x0601)
target_include_directories(${PROJECT_NAME} PRIVATE C:/boost_1_80_0/boost_1_80_0)
target_link_directories(${PROJECT_NAME} PRIVATE C:/boost_1_80_0/boost_1_80_0/stage/lib)
# TODO: Add tests and install targets if needed.
