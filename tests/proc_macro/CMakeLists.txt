cmake_minimum_required(VERSION 3.10.0)
project(proc-macro-debug-test VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE GENERATED_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.gen.c)

add_executable(proc-macro-debug-test debug_test.c ${GENERATED_SRC})
target_include_directories(proc-macro-debug-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../moe/proc_macro/include)