cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)

project(C2 VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

## Protobuff
set(PROTOBUF "C:/Users/CyberVuln/Desktop/Project/protobuf-cpp-3.19.4/protobuf-3.19.4/cmake/build")
set(Protobuf_USE_STATIC_LIBS ON)
include(${PROTOBUF}/cmake/protobuf-config.cmake)
include(${PROTOBUF}/cmake/protobuf-module.cmake)
include(${PROTOBUF}/cmake/protobuf-options.cmake)
include(${PROTOBUF}/cmake/protobuf-targets.cmake)
find_package(Protobuf REQUIRED HINTS ${PROTOBUF})
include_directories(${Protobuf_INCLUDE_DIRS})
link_libraries(${Protobuf_LIBRARIES})

## Boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

## donut
include_directories("C:/Users/CyberVuln/Desktop/Project/donut/include")
set(Donut "C:/Users/CyberVuln/Desktop/Project/donut/lib/donut.lib")

## libs/libSocketHandler
add_subdirectory(libs/libSocketHandler)
include_directories(libs/libSocketHandler/src)

## libs/libProtobufMessages
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libs/libProtobufMessages/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/libs/libProtobufMessages/)
execute_process(COMMAND cmd.exe -c "cd ${CMAKE_CURRENT_BINARY_DIR}/libs/libProtobufMessages/ && ./generate.bat")
add_subdirectory(libs/libProtobufMessages)

## thirdParty/cpp-httplib
add_subdirectory(thirdParty/cpp-httplib)
include_directories(thirdParty/cpp-httplib)



## C2
add_executable(Beacon "src/Beacon.cpp")
target_link_libraries(Beacon ${Boost_LIBRARIES} SocketHandler ProtobufMessages)

add_executable(CoreC2 "src/CoreC2.cpp")
target_link_libraries(CoreC2 ${Boost_LIBRARIES} SocketHandler ProtobufMessages)

## tests
add_subdirectory(tests)

