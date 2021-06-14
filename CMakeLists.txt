cmake_minimum_required(VERSION 3.19)
project(BaldWars_Game C)

set(CMAKE_C_STANDARD 11)

include_directories(src/game_function)
include_directories(src/json_file_management)
include_directories(src/path_finding)
include_directories(src/utils)

add_executable(BaldWars_Game
        src/game_function/game_function.c
        src/game_function/game_function.h
        src/game_function/game_struct.h
        src/json_file_management/cJSON.c
        src/json_file_management/cJSON.h
        src/json_file_management/jsonActionLog.c
        src/json_file_management/jsonActionLog.h
        src/path_finding/path_finding.c
        src/path_finding/path_finding.h
        src/utils/utils.c
        src/utils/utils.h
        src/main.c)
