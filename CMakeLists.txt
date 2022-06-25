cmake_minimum_required(VERSION 3.13)
project(touchcursor_linux)
set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
        src/service/binding.cc
        src/service/binding.h
        src/service/config.cc
        src/service/config.h
        src/service/emit.h
        src/service/KeyCodes.cc
        src/service/KeyCodes.h
        src/service/keys.cc
        src/service/keys.h
        src/service/mapper.cc
        src/service/mapper.h
        src/service/queue.cc
        src/service/queue.h
        src/service/Bindings.cc
        src/service/Bindings.h)

set(RELEASE_FILES
        src/service/emit.cc
        ${SOURCE_FILES})

set(TEST_FILES
        src/tests/emit.cc
        src/tests/testKeyCodes.cc
        src/tests/testBindings.cc
        src/tests/testConfiguration.cc
        src/tests/testEmit.cc
        ${SOURCE_FILES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY out)
add_executable(touchcursor src/service/main.cc ${RELEASE_FILES})
target_compile_features(touchcursor PUBLIC cxx_std_20)

find_package(Catch2 3 QUIET)
if (Catch2_FOUND)
        include(CTest)
        include(Catch)

        add_library(test_sources OBJECT ${TEST_FILES})

        # These tests can use the Catch2-provided main
        add_executable(tests src/tests/test.cc)
        target_compile_features(tests PUBLIC cxx_std_20)
        target_link_libraries(tests PUBLIC Catch2::Catch2WithMain)
        target_link_libraries(tests PUBLIC test_sources)

        catch_discover_tests(tests)

        # These tests need their own main
        #add_executable(custom-main-tests test.cpp test-main.cpp)
        #target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)
endif ()

set(SERVICEPATH ~/.config/systemd/user)
set(SERVICEFILE touchcursor.service)
set(CONFIGPATH ~/.config/touchcursor)
set(CONFIGFILE touchcursor.conf)
set(TARGET touchcursor)
set(OUTPATH ./out)
set(INSTALLPATH /usr/bin)
set(SERVICE touchcursor.service)

add_custom_target(deploy
        COMMAND ${CMAKE_COMMAND} -E echo "Stopping the service."
        COMMAND systemctl --user stop ${SERVICE}
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "Copying application to ${INSTALLPATH}."
        COMMAND ${CMAKE_COMMAND} -E echo "This action requires sudo privileges."
        COMMAND sudo cp ${OUTPATH}/${TARGET} ${INSTALLPATH}
        COMMAND sudo chmod u+s ${INSTALLPATH}/${TARGET}
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "Copying default configuration file to ${CONFIGPATH}/${CONFIGFILE}."
        COMMAND mkdir -p ${CONFIGPATH}
        COMMAND cp -n ${CONFIGFILE} ${CONFIGPATH}
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "Copying service file to ${SERVICEPATH}."
        COMMAND mkdir -p ${SERVICEPATH}
        COMMAND cp -f ${SERVICEFILE} ${SERVICEPATH}
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "Enabling and starting the service."
        COMMAND systemctl --user daemon-reload
        COMMAND systemctl --user enable ${SERVICE}
        COMMAND systemctl --user start ${SERVICE}
)
