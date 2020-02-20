cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(ftracer)
set(BINARY ftracer)
set(BINARY_VECTOR_TEST test_vector)
list(APPEND CMAKE_MODULE_PATH "modules/")

file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp")
file(GLOB VECTOR_TEST_SOURCES "test/vector/*.cpp")

find_package(yaml-cpp REQUIRED)
find_package(OpenEXR REQUIRED)
find_package(ASSIMP REQUIRED)

if (UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "-g -rdynamic -march=native -std=c++14 -pipe")
  set(CMAKE_CXX_FLAGS_RELEASE
    "-std=c++14 -pipe -flto -O3 -DNDEBUG -march=native -funroll-loops -rdynamic"
    )
else()
  message(FATAL_ERROR "Unsupported platform (GNU/Linux only)")
endif()

add_executable(${BINARY} ${SOURCES})

target_include_directories(${BINARY} PRIVATE
  include
  ${OPENEXR_INCLUDE_DIRS}
  ${YAML_CPP_INCLUDE_DIRS}
  ${ASSIMP_INCLUDE_DIRS}
  )

target_link_libraries(${BINARY} PRIVATE
  ${OPENEXR_LIBRARIES}
  ${YAML_CPP_LIBRARIES}
  ${ASSIMP_LIBRARIES}
  )
