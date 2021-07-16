cmake_minimum_required(VERSION 3.13)
project(BaldWars_Game C)

set(CMAKE_C_STANDARD 11)

add_compile_options(
        "-Wall" "-Wpedantic" "-Wextra" "-fexceptions"
        "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
)

include_directories(
        src/game
        src/lib
        src/scripts
        src/utils
)

add_executable(
        BaldWars_Game
        src/game/game.c
        src/lib/cJSON.c
        src/utils/path_finding.c
        src/ai_tools/tools.c
        src/utils/utils.c
        src/scripts/user1.c
        src/scripts/user2.c
        src/main.c
)
