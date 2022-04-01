cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)

project(C2 VERSION 0.0.0 LANGUAGES CXX)

if(WIN32)
	set(CMAKE_CXX_FLAGS_RELEASE "/MT")
	set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
endif()

## Protobuff
if(WIN32)
	set(PROTOBUF "C:/Users/CyberVuln/Desktop/Project/protobuf-cpp-3.19.4/protobuf-3.19.4/cmake/build")
	set(Protobuf_USE_STATIC_LIBS ON)
	include(${PROTOBUF}/cmake/protobuf-config.cmake)
	include(${PROTOBUF}/cmake/protobuf-module.cmake)
	include(${PROTOBUF}/cmake/protobuf-options.cmake)
	include(${PROTOBUF}/cmake/protobuf-targets.cmake)
	find_package(Protobuf REQUIRED HINTS ${PROTOBUF})
	include_directories(${Protobuf_INCLUDE_DIRS})
	link_libraries(${Protobuf_LIBRARIES})
else()
	set(PROTOBUF "/root/project/protobuf-cpp-3.19.4/protobuf-3.19.4/cmake/build")
	find_package(Protobuf REQUIRED HINTS ${PROTOBUF})
	include_directories("/root/project/protobuf-cpp-3.19.4/protobuf-3.19.4/src")
	set(Protobuf_LIBRARIES "/root/project/protobuf-cpp-3.19.4/protobuf-3.19.4/cmake/build/libprotobuf.a")
endif()

## Boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

## donut
if(WIN32)
	include_directories("C:/Users/CyberVuln/Desktop/Project/donut/include")
	set(Donut "C:/Users/CyberVuln/Desktop/Project/donut/lib/donut.lib")
else()
	include_directories("/root/project/donut/include")
	set(Donut "/root/project/donut/lib/donut.so")
endif()

## libs/libSocketHandler
add_subdirectory(libs/libSocketHandler)
include_directories(libs/libSocketHandler/src)

## libs/libProtobufMessages
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libs/libProtobufMessages/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/libs/libProtobufMessages/)
if(WIN32)
	execute_process(COMMAND cmd.exe /c "cd ${CMAKE_CURRENT_BINARY_DIR}/libs/libProtobufMessages/ && generate.bat")
else()
	execute_process(COMMAND bash -c "cd ${CMAKE_CURRENT_BINARY_DIR}/libs/libProtobufMessages/ && ./generate.sh")
endif()
add_subdirectory(libs/libProtobufMessages)

## thirdParty/cpp-httplib
add_subdirectory(thirdParty/cpp-httplib)
include_directories(thirdParty/cpp-httplib)

set(SOURCES_CORE
	src/CoreC2.cpp
	src/Listener.cpp
	src/ListenerTcp.cpp
	src/ListenerHttp.cpp
	src/Tools.cpp
)

## C2
add_executable(CoreC2 ${SOURCES_CORE})
if(WIN32)
	target_link_libraries(CoreC2 ${Boost_LIBRARIES} SocketHandler ProtobufMessages ${Donut})
else()
	target_link_libraries(CoreC2 ${Boost_LIBRARIES} SocketHandler ProtobufMessages ${Protobuf_LIBRARIES} pthread)
endif()

set(SOURCES_BEACON
	src/BeaconLaucher.cpp
    src/Beacon.cpp
	src/BeaconTcp.cpp
	src/BeaconHttp.cpp
	src/Tools.cpp
)

add_executable(BeaconLaucher ${SOURCES_BEACON})
if(WIN32)
	target_link_libraries(BeaconLaucher ${Boost_LIBRARIES} SocketHandler ProtobufMessages ${Donut})
else()
	target_link_libraries(BeaconLaucher ${Boost_LIBRARIES} SocketHandler ProtobufMessages ${Protobuf_LIBRARIES} pthread)
endif()

## tests
add_subdirectory(tests)

if(WIN32)
else()
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	execute_process(COMMAND bash -c "cd ${CMAKE_CURRENT_BINARY_DIR}/tests && ./generate.sh")
endif()