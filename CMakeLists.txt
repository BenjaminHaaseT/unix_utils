cmake_minimum_required(VERSION 3.26)
project(unix_utils C)

set(CMAKE_C_STANDARD 23)

add_executable(unix_utils main.c
        wcat.c
        wgrep.c
wzip.c
        wunzip.c)
