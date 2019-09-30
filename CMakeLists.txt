cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(tracer)
set(BINARY tracer)

file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

find_package(Boost COMPONENTS system filesystem thread REQUIRED)

if (UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pipe")
  set(CMAKE_CXX_FLAGS_RELEASE "-pipe -O3 -DNDEBUG -march=native -flto")
elseif (MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT /I${BOOST_ROOT} /std:c++14")
  set(CMAKE_CXX_FLAGS_RELEASE "/nologo /MT /I${BOOST_ROOT} /std:c++14 /O2")
else()
  message(FATAL_ERROR "Unsupported platform (not in { UNIX, MSVC })")
endif()

add_executable(${BINARY} ${SOURCES})

target_include_directories(${BINARY} PRIVATE include)
