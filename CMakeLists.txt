cmake_minimum_required(VERSION 3.10.0)
project(moe-toolchain VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(moe/proc_macro)

