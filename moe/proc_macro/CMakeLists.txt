cmake_minimum_required(VERSION 3.10.0)
project(proc-macro VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE COMMON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../common/*.c)

file(
    GLOB_RECURSE VENDORED_QJS_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/qjs/quickjs/libregexp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/qjs/quickjs/libunicode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/qjs/quickjs/quickjs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/qjs/quickjs/cutils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/qjs/quickjs/dtoa.c)

add_executable(proc-macro proc_macro.c stb_include.c ${COMMON_SRC} ${VENDORED_QJS_SRC})
target_include_directories(proc-macro PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/stb)
target_include_directories(proc-macro PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/qjs/quickjs)
target_include_directories(proc-macro PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common)