cmake_minimum_required(VERSION 3.24.0)

project(orbit)

set(BOOST_ROOT "./boost")
set(BOOST_INCLUDEDIR "./boost/include/boost-1_86/")
set(Boost_USE_STATIC_RUNTIME ON)

if(MINGW)
    link_libraries(ws2_32 wsock32)
endif()

find_package(Boost 1.70 REQUIRED COMPONENTS system filesystem)

set(CLIENT_SOURCE_FILES
	./utils/SerializerRequests.cpp
	./utils/DeserializerResponses.cpp
	./utils/utils.cpp
	./networking/client.cpp
	./networking/WSAInitializer.cpp
)

set(CLIENT_HEADER_FILES
	./utils/json.hpp
	./utils/utils.hpp
	./utils/SerializerRequests.h
	./utils/DeserializerResponses.h
	./networking/client.h
	./networking/WSAInitializer.h
)

set(SERVER_SOURCE_FILES
    	./networking/docker_manager.cpp
    	./networking/server.cpp
	./networking/WSAInitializer.cpp
	./utils/SerializerResponses.cpp
	./utils/DeserializerRequests.cpp
	./utils/utils.cpp
)

set(SERVER_HEADER_FILES
    	./networking/docker_manager.h
	./networking/server.h
	./networking/WSAInitializer.h
	./utils/SerializerResponses.h
	./utils/DeserializerRequests.h
	./utils/json.hpp
	./utils/utils.hpp
)

set(NODE_SOURCE_FILES
	./networking/node.cpp
	./networking/WSAInitializer.cpp
	./utils/utils.cpp
)

set(NODE_HEADER_FILES
	./networking/node.h
	./networking/WSAInitializer.h
	./utils/json.hpp
	./utils/utils.hpp
)

add_executable(client ${CLIENT_SOURCE_FILES} ${CLIENT_HEADER_FILES})

add_executable(server ${SERVER_SOURCE_FILES} ${SERVER_HEADER_FILES})

add_executable(node ${NODE_SOURCE_FILES} ${NODE_HEADER_FILES})

if(Boost_FOUND)
    target_include_directories(client PUBLIC ${Boost_INCLUDE_DIR})
    target_link_libraries(client ${Boost_LIBRARIES})
	
	target_include_directories(server PUBLIC ${Boost_INCLUDE_DIR})
    target_link_libraries(server ${Boost_LIBRARIES})
	
	target_include_directories(node PUBLIC ${Boost_INCLUDE_DIR})
    target_link_libraries(node ${Boost_LIBRARIES})

    message(STATUS "Found BOOST ${Boost_VERSION_STRING}")
else()
    message(STATUS "Could not find BOOST")
endif()