cmake_minimum_required(VERSION 3.24)
project(TtpTak LANGUAGES C CXX RC)
if(NOT WIN32 OR NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR "ttp_tak requires MSVC and -A Win32")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(cmake/legacy_windows.cmake)
include(cmake/size_release.cmake)
include(cmake/codec.cmake)
include(cmake/version.cmake)
add_library(ttp_tak SHARED src/plugin.cpp src/reader.cpp src/ttp_tak.def
  "${CMAKE_CURRENT_BINARY_DIR}/generated/version.rc")
add_dependencies(ttp_tak ttp_tak_version)
target_include_directories(ttp_tak PUBLIC include PRIVATE src)
target_compile_definitions(ttp_tak PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN
  _WIN32_WINNT=0x0501 _CRT_SECURE_NO_WARNINGS)
target_compile_options(ttp_tak PRIVATE /W4 /permissive- /EHsc /GR- /utf-8 /fp:precise /arch:SSE2)
target_link_libraries(ttp_tak PRIVATE codec ole32 uuid shlwapi)
set_target_properties(ttp_tak PROPERTIES PREFIX "" OUTPUT_NAME "ttp_tak")
add_custom_command(TARGET ttp_tak POST_BUILD
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/check_legacy_imports.py"
    "$<TARGET_FILE:ttp_tak>"
    --exports "${ttplayer_yy_thunks_SOURCE_DIR}/Config/x86/5.1.2600.txt"
    --exports "${ttplayer_yy_thunks_SOURCE_DIR}/Config/x86/6.1.7600.txt"
    --report "$<TARGET_FILE_DIR:ttp_tak>/legacy-imports.json" VERBATIM)
install(TARGETS ttp_tak RUNTIME DESTINATION AddIn COMPONENT Runtime)

