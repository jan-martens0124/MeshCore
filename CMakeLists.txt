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
link_libraries(GLEW::GLEW)

### GSL ###
find_package(GSL REQUIRED)
link_libraries(GSL::gsl)

file(GLOB SOURCES tasks/*.cpp utility/*.cpp core/*.cpp rendering/*.cpp geometric/*.cpp acceleration/*.cpp external/quickhull/*.cpp)

add_library(MeshCore STATIC ${SOURCES})

### SimpleRenderDemo.exe ###
add_executable(A-SimpleRenderDemo demos/renderDemo.cpp ${SOURCES})
target_link_libraries(A-SimpleRenderDemo MeshCore)

### SimpleCollisionDemo.exe ###
add_executable(B-SimpleCollisionDemo demos/collisionDemo.cpp ${SOURCES})
target_link_libraries(B-SimpleCollisionDemo MeshCore)

### TaskDemo.exe ###
add_executable(C-TaskDemo demos/taskDemo.cpp ${SOURCES})
target_link_libraries(C-TaskDemo MeshCore)

### ConvexHull.exe ###
add_executable(D-ConvexHull demos/convexHullDemo.cpp ${SOURCES})
target_link_libraries(D-ConvexHull MeshCore)

### DistanceDemo.exe ###
add_executable(E-DistanceDemo demos/distanceDemo.cpp ${SOURCES})
target_link_libraries(E-DistanceDemo MeshCore)

### Tests ###
enable_testing()
add_executable(TestConvexHull tests/TestConvexHull.cpp)
target_link_libraries(TestConvexHull MeshCore)
add_test(NAME TestConvexHull COMMAND TestConvexHull)