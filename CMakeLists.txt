cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(llmtarot)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(llmtarot main.cpp ${SRC_FILES})
target_include_directories(llmtarot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(llmtarot jsoncpp curl)
