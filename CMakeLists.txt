cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(ftracer)
set(BINARY ftracer)
set(BINARY_VECTOR_TEST test_vector)
list(APPEND CMAKE_MODULE_PATH "modules/")

file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp")
file(GLOB VECTOR_TEST_SOURCES "test/vector/*.cpp")

find_package(yaml-cpp REQUIRED)
find_package(OpenEXR REQUIRED)

if (UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "-g -rdynamic -std=c++14 -pipe")
  set(CMAKE_CXX_FLAGS_RELEASE "-std=c++14 -pipe -O3 -DNDEBUG -march=native -flto")
elseif (MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT /I${BOOST_ROOT} /std:c++14")
  set(CMAKE_CXX_FLAGS_RELEASE "/nologo /DNDEBUG /MT /I${BOOST_ROOT} /std:c++14 /O2")
else()
  message(FATAL_ERROR "Unsupported platform (not in { UNIX, MSVC })")
endif()

add_executable(${BINARY} ${SOURCES})

target_include_directories(${BINARY} PRIVATE include ${OPENEXR_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIRS})

target_link_libraries(${BINARY} PRIVATE ${OPENEXR_LIBRARIES} ${YAML_CPP_LIBRARIES})
