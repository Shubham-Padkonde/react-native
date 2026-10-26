cmake_minimum_required(VERSION 3.22)
project(rawprops_validation C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_compile_options(-stdlib=libc++ -fsanitize=address -fno-omit-frame-pointer -g -O1)
add_link_options(-stdlib=libc++ -fsanitize=address)
add_compile_definitions(NDEBUG RN_BUILDING)
include(FetchContent)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(WITH_GFLAGS OFF CACHE BOOL "" FORCE)
set(FMT_TEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glog URL https://github.com/google/glog/archive/refs/tags/v0.3.5.tar.gz)
FetchContent_Declare(fmt URL https://github.com/fmtlib/fmt/archive/refs/tags/12.1.0.tar.gz)
FetchContent_Declare(fast_float URL https://github.com/fastfloat/fast_float/archive/refs/tags/v8.0.0.tar.gz)
FetchContent_Declare(double_conversion URL https://github.com/google/double-conversion/archive/refs/tags/v1.1.6.tar.gz)
FetchContent_Declare(boost_source URL https://archives.boost.io/release/1.83.0/source/boost_1_83_0.tar.gz)
FetchContent_Declare(folly_source URL https://github.com/facebook/folly/archive/refs/tags/v2024.11.18.00.tar.gz)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz)
FetchContent_MakeAvailable(glog fmt fast_float double_conversion googletest)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/double-conversion)
file(GLOB conversion_headers ${double_conversion_SOURCE_DIR}/src/*.h)
file(COPY ${conversion_headers} DESTINATION ${CMAKE_BINARY_DIR}/include/double-conversion)
target_include_directories(double-conversion PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>)
FetchContent_Populate(boost_source)
FetchContent_Populate(folly_source)
add_library(boost INTERFACE)
target_include_directories(boost INTERFACE ${boost_source_SOURCE_DIR})
set(REACT_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../packages/react-native/ReactCommon)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../private/react-native-fantom/tester/third-party/folly/CMakeLists.txt DESTINATION ${folly_source_SOURCE_DIR})
add_subdirectory(${folly_source_SOURCE_DIR} folly)
add_executable(rawprops_tests
 ${REACT_COMMON_DIR}/react/renderer/core/tests/RawPropsParserTest.cpp
 ${REACT_COMMON_DIR}/jsi/jsi/JSIDynamic.cpp
 ${REACT_COMMON_DIR}/jsi/jsi/jsi.cpp
 ${REACT_COMMON_DIR}/react/renderer/core/RawProps.cpp
 ${REACT_COMMON_DIR}/react/renderer/core/RawValue.cpp
 ${REACT_COMMON_DIR}/react/renderer/core/RawPropsKeyMap.cpp
 ${REACT_COMMON_DIR}/react/renderer/core/RawPropsParser.cpp)
target_include_directories(rawprops_tests PRIVATE
 ${REACT_COMMON_DIR}
 ${REACT_COMMON_DIR}/callinvoker
 ${REACT_COMMON_DIR}/cxxreact
 ${REACT_COMMON_DIR}/jsi
 ${REACT_COMMON_DIR}/react/debug
 ${REACT_COMMON_DIR}/react/renderer/core
 ${REACT_COMMON_DIR}/react/renderer/debug
 ${REACT_COMMON_DIR}/react/renderer/graphics
 ${REACT_COMMON_DIR}/react/renderer/graphics/platform/cxx
 ${REACT_COMMON_DIR}/react/renderer/mapbuffer
 ${REACT_COMMON_DIR}/react/utils
 ${REACT_COMMON_DIR}/runtimeexecutor)
target_link_libraries(rawprops_tests folly_runtime glog gtest_main)
