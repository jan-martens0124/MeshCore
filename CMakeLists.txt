project("MeshCore")
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

### CPP ###
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

### GLM ###
find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

### QT ###
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
link_libraries(Qt5::Widgets)

### OpenGL ###
find_package(OpenGL REQUIRED)
link_libraries(OpenGL::GL)

### GLEW ###
add_definitions(-DGLEW_STATIC)
include_directories(${GLEW_DIR}/include)
link_libraries("${GLEW_DIR}/lib/Release/x64/glew32s.lib")

#find_package(GLEW REQUIRED)


file(GLOB SOURCES main.cpp utility/*.cpp core/*.cpp rendering/*.cpp)
add_executable(Renderer ${SOURCES})