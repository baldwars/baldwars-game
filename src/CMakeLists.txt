cmake_minimum_required(VERSION 3.13)
project(BaldWars_Game C)

set(CMAKE_C_STANDARD 11)

include_directories(
        game
        lib
        scripts
        utils
)

add_executable(BaldWars_Game
        game/game.c
        lib/cJSON.c
        utils/path_finding.c
        ai_tools/tools.c
        utils/utils.c
        scripts/user1.c
        scripts/user2.c
        main.c)
