cmake_minimum_required(VERSION 3.13)
project(BaldWars_Game C)

set(CMAKE_C_STANDARD 11)

include_directories(game)
include_directories(lib)
include_directories(utils)
include_directories(scripts)

add_executable(BaldWars_Game
        game/game.c
        game/game.h
        lib/cJSON.c
        lib/cJSON.h
        utils/path_finding.c
        utils/path_finding.h
        ai_tools/tools.c
        ai_tools/tools.h
        utils/utils.c
        utils/utils.h
        scripts/user1.c
        scripts/user2.c
        scripts/scripts.h
        main.c)
