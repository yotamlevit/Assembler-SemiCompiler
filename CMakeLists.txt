cmake_minimum_required(VERSION 3.26)
project(Assembler_SemiCompiler C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(Assembler_SemiCompiler
        include/assembler.h
        include/auxiliary.h
        include/constants_tables.h
        include/first_pass.h
        src/assembler.c
        src/auxiliary_functions.c
        src/first_pass.c
        src/memory_map.c
        src/second_pass.c)
