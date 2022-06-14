cmake_minimum_required(VERSION 3.13)
project(touchcursor_linux)
set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
        src/service/binding.cc
        src/service/binding.h
        src/service/config.cc
        src/service/config.h
        src/service/emit.cc
        src/service/emit.h
        src/service/KeyCodes.cc
        src/service/KeyCodes.h
        src/service/keys.cc
        src/service/keys.h
        src/service/mapper.cc
        src/service/mapper.h
        src/service/queue.cc
        src/service/queue.h
)

set(TEST_FILES
        src/test/testKeyCodes.cc
        ${SOURCE_FILES}
)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(touchcursor_linux src/service/main.cc ${SOURCE_FILES})
target_compile_features(touchcursor_linux PUBLIC cxx_std_20)

find_package(Catch2 3 REQUIRED)
include(CTest)
include(Catch)

add_library(test_sources OBJECT ${TEST_FILES})

# These tests can use the Catch2-provided main
add_executable(tests src/test/test.cc)
target_compile_features(tests PUBLIC cxx_std_20)
target_link_libraries(tests PUBLIC Catch2::Catch2WithMain)
target_link_libraries(tests PUBLIC test_sources)

catch_discover_tests(tests)

# These tests need their own main
#add_executable(custom-main-tests test.cpp test-main.cpp)
#target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)

