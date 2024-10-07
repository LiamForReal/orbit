cmake_minimum_required(VERSION 3.24.0)

project(orbit)

set(BOOST_ROOT "./boost")
set(BOOST_INCLUDEDIR "./boost/include/boost-1_86/")
set(Boost_USE_STATIC_RUNTIME ON)

if(MINGW)
    link_libraries(ws2_32 wsock32)
endif()

find_package(Boost 1.70 REQUIRED COMPONENTS system filesystem)

set(SOURCE_FILES
    main.cpp
)

set(HEADER_FILES

)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

if(Boost_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
    message(STATUS "Found BOOST ${Boost_VERSION_STRING}")
else()
    message(STATUS "Could not find BOOST")
endif()