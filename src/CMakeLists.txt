cmake_minimum_required(VERSION 3.19)
project(baldwars-game C)

set(CMAKE_C_STANDARD 99)

add_executable(baldwars-game main.c utils/utils.c utils/utils.h path_finding/path_finding.c path_finding/path_finding.h)