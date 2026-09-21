cmake_minimum_required(VERSION 3.22)
project(sht40 C)

add_library(sht40 INTERFACE)

target_include_directories(sht40 INTERFACE
        Inc
)

target_sources(sht40 INTERFACE
        Src/sht40.c
)