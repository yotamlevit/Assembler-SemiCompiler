cmake_minimum_required(VERSION 3.26)
project(Assembler_SemiCompiler C)

set(CMAKE_C_STANDARD 90)

include_directories(include)

add_executable(Assembler_SemiCompiler
        include/tables.h
        include/constants_tables.h
        include/first_pass.h
        src/assembler.c
        src/tables.c
        src/first_pass.c
        src/output.c
        src/second_pass.c
        src/validators.c
        include/validators.h
        include/globals.h
        include/output.h
        include/second_pass.h
        src/utils.c
        include/utils.h
        include/status_codes.h
        src/status_codes.c
        src/logger.c
        include/logger.h
        src/preprocess.c
        include/preprocess.h
        src/hash_map.c
        include/hash_map.h)
