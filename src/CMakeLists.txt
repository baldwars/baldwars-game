cmake_minimum_required(VERSION 3.19)
project(BaldWars_Game C)

set(CMAKE_C_STANDARD 11)

include_directories(game)
include_directories(json_file_management)
include_directories(path_finding)
include_directories(utils)

add_executable(BaldWars_Game
        game/game.c
        game/game.h
        game/game_struct.h
        json_file_management/cJSON.c
        json_file_management/cJSON.h
        json_file_management/jsonActionLog.c
        json_file_management/jsonActionLog.h
        path_finding/path_finding.c
        path_finding/path_finding.h
        player_functions/functions.c
        player_functions/functions.h
        utils/utils.c
        utils/utils.h
        main.c)
