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
link_libraries(glm::glm)

### Boost ###
find_package(Boost REQUIRED)
link_libraries(Boost::boost)

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
find_package(GLEW REQUIRED)
link_libraries(GLEW::glew)

### GSL ###
find_package(GSL REQUIRED)
link_libraries(GSL::gsl)

file(GLOB SOURCES tasks/*.cpp utility/*.cpp core/*.cpp rendering/*.cpp geometric/*.cpp acceleration/*.cpp)
add_executable(A-SimpleRenderDemo renderDemo.cpp ${SOURCES})
add_executable(B-SimpleCollisionDemo collisionDemo.cpp ${SOURCES})
add_executable(C-TaskDemo taskDemo.cpp ${SOURCES})