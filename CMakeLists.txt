cmake_minimum_required(VERSION 3.23)
project(ATM_sys)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -lpthread")

include_directories(.)

add_executable(ATM_sys
        account.cpp
        account.h
        main.cpp
        MD5.cpp
        MD5.h
        record.cpp
        record.h
        system.cpp
        system.h snack.h)

