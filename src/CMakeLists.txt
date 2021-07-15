cmake_minimum_required(VERSION 3.19)
project(BaldWars_Game C)

set(CMAKE_C_STANDARD 11)

include_directories(game)
include_directories(lib)
include_directories(path_finding)
include_directories(utils)

add_executable(BaldWars_Game
        game/game.c
        game/game.h
        game/game_struct.h
        lib/cJSON.c
        lib/cJSON.h
        path_finding/path_finding.c
        path_finding/path_finding.h
        ai_tools/tools.c
        ai_tools/tools.h
        utils/utils.c
        utils/utils.h
        main.c)
